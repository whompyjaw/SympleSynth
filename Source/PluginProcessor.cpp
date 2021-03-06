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
        synth.addVoice(new SynthVoice(tree, lfoBuffer));
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
    
    // prepare lfo
    lfo.setSampleRate(sampleRate);
    lfo.setFrequency(tree.getRawParameterValue("LFO_FREQUENCY")->load());
    int oscMode = tree.getParameterAsValue("LFO_WAVE_TYPE").getValue();
    lfo.setMode(static_cast<OscillatorMode> (oscMode));
    lfoBuffer = juce::dsp::AudioBlock<float> (heapBlock, 1, samplesPerBlock);
    lfoBuffer.clear();
    
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
    
    // prepare lfo for synth processing
    lfoBuffer.clear();
    float lfoFrequency = tree.getRawParameterValue("LFO_FREQUENCY")->load();
    int oscMode = tree.getParameterAsValue("LFO_WAVE_TYPE").getValue();
    lfo.setMode(static_cast<OscillatorMode> (oscMode));
    lfo.setFrequency(lfoFrequency);
    if (lfoFrequency < 0.0005)
    {
        lfoBuffer.fill(-1.0f);
    }
    else
    {
        lfo.generate(lfoBuffer, buffer.getNumSamples(), 0.0);
    }
    
    // This needs to be before this process loop.
    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
    float gainValue = tree.getRawParameterValue("MASTER_GAIN")->load();
    for (int channel = 0; channel < totalNumOutputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            channelData[sample] = channelData[sample] * juce::Decibels::decibelsToGain(gainValue);
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
//    std::vector<std::unique_ptr<juce::RangedAudioParameter>> parameters;

    juce::NormalisableRange<float> masterGainRange = juce::NormalisableRange<float>(-120.0f, 0.0f);
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("MASTER_GAIN", "MasterGain", masterGainRange, -20.0f));

    // filter knob ranges
    juce::NormalisableRange<float> cutoffRange = juce::NormalisableRange<float>(10.0f, 20000.0f);
    juce::NormalisableRange<float> resRange = juce::NormalisableRange<float>(0.0f, 100.0f);
    juce::NormalisableRange<float> amountRange = juce::NormalisableRange<float>(0.0f, 100.0f);
    cutoffRange.setSkewForCentre(1000.0f);

    // filter parameters
    juce::NormalisableRange<float> envelopeAmountRange(0, 100, 1);
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("FILTER_1_CUTOFF", "Cutoff", cutoffRange, 8000.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("FILTER_1_RESONANCE", "Resonance", resRange, 0.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("FILTER_1_AMOUNT", "Amount", envelopeAmountRange, 0));

    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("FILTER_2_CUTOFF", "Cutoff", cutoffRange, 8000.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("FILTER_2_RESONANCE", "Resonance", resRange, 0.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("FILTER_2_AMOUNT", "Amount", envelopeAmountRange, 0));

    // envelope knob ranges
    juce::NormalisableRange<float> attackRange = juce::NormalisableRange<float>(0.01f, 10.0f);
    juce::NormalisableRange<float> decayRange = juce::NormalisableRange<float>(0.0f, 10.0f);
    juce::NormalisableRange<float> sustainRange = juce::NormalisableRange<float>(0.0f, 100.0f);
    juce::NormalisableRange<float> releaseRange = juce::NormalisableRange<float>(0.0f, 10.0f);
    juce::NormalisableRange<float> filterMode(0, 5, 1);
    attackRange.setSkewForCentre(0.35f);
    decayRange.setSkewForCentre(0.35f);
    releaseRange.setSkewForCentre(0.35f);
    
    // amp envelope parameters
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("AMP_ATTACK", "Attack", attackRange, 0.01f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("AMP_DECAY", "Decay", decayRange, 1.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("AMP_SUSTAIN", "Sustain", sustainRange, 100.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("AMP_RELEASE", "Release", releaseRange, 0.1f));
    
    // filter envelope parameters
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("FILTER_1_MODE", "Filter 1 Mode", filterMode, 0));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("FILTER_1_ATTACK", "Attack", attackRange, 0.001f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("FILTER_1_DECAY", "Decay", decayRange, 1.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("FILTER_1_SUSTAIN", "Sustain", sustainRange, 100.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("FILTER_1_RELEASE", "Release", releaseRange, 0.1f));

    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("FILTER_2_MODE", "Filter 2 Mode", filterMode, 0));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("FILTER_2_ATTACK", "Attack", attackRange, 0.001f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("FILTER_2_DECAY", "Decay", decayRange, 1.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("FILTER_2_SUSTAIN", "Sustain", sustainRange, 100.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("FILTER_2_RELEASE", "Release", releaseRange, 0.1f));

    // oscillator 1 defaults
    juce::NormalisableRange<float> oscillatorOctaveParams (-2, 2, 1);
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("OSC_1_OCTAVE", "Octave 1", oscillatorOctaveParams, 0, "Octave"));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("OSC_2_OCTAVE", "Octave 2", oscillatorOctaveParams, 0, "Octave"));

    juce::NormalisableRange<float> oscillatorSemitone (-12, 12, 1);
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("OSC_1_SEMITONE", "Semitone 1", oscillatorSemitone, 0, "Semitone"));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("OSC_2_SEMITONE", "Semitone 2", oscillatorSemitone, 0, "Semitone"));

    juce::NormalisableRange<float> oscillatorFineTune (-100, 100, 1);
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("OSC_1_FINE_TUNE", "Fine Tune 1", oscillatorFineTune, 0, "Fine Tune"));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("OSC_2_FINE_TUNE", "Fine Tune 2", oscillatorFineTune, 0, "Fine Tune"));

    juce::NormalisableRange<float> oscillatorWaveType (0, 3, 1);
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("OSC_1_WAVE_TYPE", "Wave Type 1", oscillatorWaveType, 1, "Wave Type"));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("OSC_2_WAVE_TYPE", "Wave Type 2", oscillatorWaveType, 1, "Wave Type"));

    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("OSC_1_GAIN", "Gain 1", masterGainRange, -20.0f, "Gain"));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("OSC_2_GAIN", "Gain 2", masterGainRange, -20.0f, "Gain"));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("NOISE_1_GAIN",
                                                                     "Noise Gain 1",
                                                                     masterGainRange,
                                                                     -120.0f,
                                                                     "Gain",
                                                                     juce::AudioProcessorParameter::genericParameter,
                                                                     [](float value, int) { return juce::String (value, 1); }));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("NOISE_2_GAIN",
                                                                     "Noise Gain 2",
                                                                     masterGainRange,
                                                                     -120.0f,
                                                                     "Gain",
                                                                     juce::AudioProcessorParameter::genericParameter,
                                                                     [](float value, int) { return juce::String (value, 1); }));

    // lfo parameters
    juce::NormalisableRange<float> lfoFrequencyRange = juce::NormalisableRange<float>(0.0f, 200.0f);
    lfoFrequencyRange.setSkewForCentre(10.0f);
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("LFO_FREQUENCY", "LFO Frequency", lfoFrequencyRange, 0.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("LFO_AMOUNT", "LFO Amount", envelopeAmountRange, 0));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("LFO_WAVE_TYPE", "LFO Wave Type", oscillatorWaveType, 1));

    return { parameters.begin(), parameters.end() };
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SympleSynthAudioProcessor();
}
