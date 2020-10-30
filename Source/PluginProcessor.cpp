/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

    Sources: https://github.com/TheAudioProgrammer/juceIIRFilter

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SympleSynthAudioProcessor::SympleSynthAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), tree(*this, nullptr, "PARAMETERS", createParameters())
#endif
{
    // initialize the synth with x number of voices
    synth.clearVoices();
    for (int i = 0; i < VOICE_COUNT; ++i)
    {
        synth.addVoice(new SynthVoice(tree));
    }

    synth.clearSounds();
    synth.addSound(new SynthSound());
}

SympleSynthAudioProcessor::~SympleSynthAudioProcessor()
{
}

//==============================================================================
const juce::String SympleSynthAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SympleSynthAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SympleSynthAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SympleSynthAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SympleSynthAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SympleSynthAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SympleSynthAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SympleSynthAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SympleSynthAudioProcessor::getProgramName (int index)
{
    return {};
}

void SympleSynthAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void SympleSynthAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you 
    juce::ignoreUnused(samplesPerBlock); // clear out any unused samples from last key press
    lastSampleRate = sampleRate; // this is in case the sample rate is changed while the synth is being used so it doesn't 
    synth.setCurrentPlaybackSampleRate(lastSampleRate);
    
    // prepare voices with buffer/sample rate
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    
    prepareVoices(spec);
}

/* Gets called when the application is closed. */
void SympleSynthAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    keyboardState.reset();

}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SympleSynthAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

/* This block gets called sampleRate / bufferSize times per second.
So 44100 / 512 = 86 times per second
44100 / 64 = 689 times per second */
void SympleSynthAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    buffer.clear();
    keyboardState.processNextMidiBuffer(midiMessages, 0,
        buffer.getNumSamples(), true);
    
    // This needs to be before this process loop.
    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
    for (int channel = 0; channel < totalNumOutputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            channelData[sample] = channelData[sample] * juce::Decibels::decibelsToGain(masterGain);
        }
    }
    midiMessages.clear();
}

//==============================================================================
bool SympleSynthAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SympleSynthAudioProcessor::createEditor()
{
    return new SympleSynthAudioProcessorEditor (*this);
}

//==============================================================================
void SympleSynthAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void SympleSynthAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

juce::MidiKeyboardState& SympleSynthAudioProcessor::getKeyboardState()
{
    return keyboardState;
}

void SympleSynthAudioProcessor::prepareVoices(juce::dsp::ProcessSpec& spec)
{
    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        dynamic_cast<SynthVoice*>(synth.getVoice(i))->prepare(spec);
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout SympleSynthAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> parameters;

    // filter knob ranges
    juce::NormalisableRange<float> cutoffRange = juce::NormalisableRange<float>(10.0f, 20000.0f);
    juce::NormalisableRange<float> resRange = juce::NormalisableRange<float>(1.0f, 100.0f);
    juce::NormalisableRange<float> amountRange = juce::NormalisableRange<float>(1.0f, 100.0f);
    cutoffRange.setSkewForCentre(1000.0f);

    // filter parameters
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("CUTOFF", "Cutoff", cutoffRange, 20000.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("RESONANCE", "Resonance", resRange, 1.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("AMOUNT", "Amount", amountRange, 30.0f));

    // envelope knob ranges
    juce::NormalisableRange<float> attackRange = juce::NormalisableRange<float>(0.0f, 10.0f);
    juce::NormalisableRange<float> decayRange = juce::NormalisableRange<float>(0.0f, 10.0f);
    juce::NormalisableRange<float> sustainRange = juce::NormalisableRange<float>(0.0f, 100.0f);
    juce::NormalisableRange<float> releaseRange = juce::NormalisableRange<float>(0.0f, 10.0f);
    attackRange.setSkewForCentre(0.35f);
    decayRange.setSkewForCentre(0.35f);
    releaseRange.setSkewForCentre(0.35f);
    
    // amp envelope parameters
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("AMP_ATTACK", "Attack", attackRange, 0.001f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("AMP_DECAY", "Decay", decayRange, 1.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("AMP_SUSTAIN", "Sustain", sustainRange, 100.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("AMP_RELEASE", "Release", releaseRange, 0.1f));
    
    // filter envelope parameters
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("FILTER_ATTACK", "Attack", attackRange, 0.001f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("FILTER_DECAY", "Decay", decayRange, 1.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("FILTER_SUSTAIN", "Sustain", sustainRange, 100.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("FILTER_RELEASE", "Release", releaseRange, 0.1f));

    setOscParams(parameters);
    
    return { parameters.begin(), parameters.end() };
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SympleSynthAudioProcessor();
}

void SympleSynthAudioProcessor::setOscParams(std::vector<std::unique_ptr<juce::RangedAudioParameter>>& parameters)
{
    // oscillator 1 defaults
    juce::NormalisableRange<float> oscillatorOctaveParams (-2, 2, 1);
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("OSC_1_OCTAVE", "Octave 1", oscillatorOctaveParams, 0, "Octave"));
    
    juce::NormalisableRange<float> oscillatorSemitone (-12, 12, 1);
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("OSC_1_SEMITONE", "Semitone 1", oscillatorSemitone, 0, "Semitone"));
    
    juce::NormalisableRange<float> oscillatorFineTune (-100, 100, 1);
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("OSC_1_FINE_TUNE", "Fine Tune 1", oscillatorFineTune, 0, "Fine Tune"));
    
    juce::NormalisableRange<float> oscillatorWaveType (0, 3, 1);
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("OSC_1_WAVE_TYPE", "Wave Type 1", oscillatorWaveType, 1, "Wave Type"));
}
