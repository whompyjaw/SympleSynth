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
                       ), lowPassFilter(juce::dsp::IIR::Coefficients<float>::makeLowPass(44100, 20000.0f, 0.1f)),
                       tree(*this, nullptr, "PARAMETERS", createParameters())
#endif
{
    // initialize amplifier parameters
    ampParameters = {0.001, 1.0, 1.0, 0.2};
    filterAmpParameters = {0.001, 1.0, 1.0, 0.2};
    filterAmp.setParameters(filterAmpParameters);

    synth.clearVoices();
    for (int i = 0; i < VOICE_COUNT; ++i)
    {
        synth.addVoice(new SineWaveVoice(ampParameters, filterAmp));
    }

    synth.clearSounds();
    synth.addSound(new SineWaveSound());
    
    setUpValueTreeListeners();
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

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();

    filterAmp.setSampleRate(sampleRate);
    lowPassFilter.prepare(spec);
    lowPassFilter.reset();
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

/*
 *  Code for this block is adapted from the JUCE DSP tutorial for LFO filter
 *  cutoff triggering. The specific code is available under the heading
 *  "Modulating the signal with an LFO" numbers 5, 6, 7 at:
 *  https://docs.juce.com/master/tutorial_dsp_introduction.html
 *
 *  This function processes an audio block with the filter sections
*/
void SympleSynthAudioProcessor::filterNextBlock(juce::AudioBuffer<float>& buffer)
{
    juce::dsp::AudioBlock<float> block(buffer);
    size_t numSamples = block.getNumSamples();

    size_t updateRate = FILTER_UPDATE_RATE;
    size_t updateCounter = updateRate;
    size_t read = 0;
    while (read < numSamples) {
        auto max = juce::jmin((size_t) numSamples - read, updateCounter);
        auto subBlock = block.getSubBlock (read, max);

        lowPassFilter.process(juce::dsp::ProcessContextReplacing<float>(subBlock));

        read += max;
        updateCounter -= max;
        float nextAmpSample = filterAmp.getNextSample();

        if (updateCounter == 0)
        {
            updateCounter = updateRate;
            float freq = tree.getRawParameterValue("CUTOFF")->load();
            float res = tree.getRawParameterValue("RESONANCE")->load();
            auto cutOffFreqHz = juce::jmap (nextAmpSample, 0.0f, 1.0f, freq, 20000.0f);
            *lowPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(lastSampleRate, cutOffFreqHz, res);
        }
    }
}

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

    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples()); // This needs to be before this process loop.
    for (int channel = 0; channel < totalNumOutputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            channelData[sample] = channelData[sample] * juce::Decibels::decibelsToGain(masterGain);
        }
    }
    midiMessages.clear();
    filterNextBlock(buffer);
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

juce::ADSR::Parameters& SympleSynthAudioProcessor::getAmpParameters()
{
    return ampParameters;
}

void SympleSynthAudioProcessor::setAmpParameters(juce::ADSR::Parameters& params)
{
    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        dynamic_cast<SineWaveVoice*>(synth.getVoice(i))->setAmpParameters (params);
    }
}

void SympleSynthAudioProcessor::setUpValueTreeListeners()
{
    // amp listeners
    tree.addParameterListener("AMP_ATTACK", this);
    tree.addParameterListener("AMP_DECAY", this);
    tree.addParameterListener("AMP_SUSTAIN", this);
    tree.addParameterListener("AMP_RELEASE", this);
    
    // filter adsr listeners
    tree.addParameterListener("FILTER_ATTACK", this);
    tree.addParameterListener("FILTER_DECAY", this);
    tree.addParameterListener("FILTER_SUSTAIN", this);
    tree.addParameterListener("FILTER_RELEASE", this);
}

void SympleSynthAudioProcessor::parameterChanged(const juce::String& paramName, float newValue)
{
    ampParameters.attack = tree.getRawParameterValue("AMP_ATTACK")->load();
    ampParameters.decay = tree.getRawParameterValue("AMP_DECAY")->load();
    ampParameters.sustain = tree.getRawParameterValue("AMP_SUSTAIN")->load() / 100;
    ampParameters.release = tree.getRawParameterValue("AMP_RELEASE")->load();
    
    filterAmpParameters.attack = tree.getRawParameterValue("FILTER_ATTACK")->load();
    filterAmpParameters.decay = tree.getRawParameterValue("FILTER_DECAY")->load();
    filterAmpParameters.sustain = tree.getRawParameterValue("FILTER_SUSTAIN")->load() / 100;
    filterAmpParameters.release = tree.getRawParameterValue("FILTER_RELEASE")->load() / 100;
    
    filterAmp.setParameters(filterAmpParameters);
    setAmpParameters(ampParameters);
}

juce::AudioProcessorValueTreeState::ParameterLayout SympleSynthAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> parameters;

    // filter parameters
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("CUTOFF", "Cutoff", 10.0f, 20000.0f, 20000.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("RESONANCE", "Resonance", 0.1f, 10.0f, 0.1f));
    
    // adsr knob ranges
    juce::NormalisableRange<float> attackRange = juce::NormalisableRange<float>(0.0f, 10.0f);
    juce::NormalisableRange<float> decayRange = juce::NormalisableRange<float>(0.0f, 10.0f);
    juce::NormalisableRange<float> sustainRange = juce::NormalisableRange<float>(0.0f, 100.0f);
    juce::NormalisableRange<float> releaseRange = juce::NormalisableRange<float>(0.0f, 10.0f);
    attackRange.setSkewForCentre(0.35f);
    decayRange.setSkewForCentre(0.35f);
    releaseRange.setSkewForCentre(0.35f);
    
    // amp parameters
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("AMP_ATTACK", "Attack", attackRange, 0.001f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("AMP_DECAY", "Decay", decayRange, 1.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("AMP_SUSTAIN", "Sustain", sustainRange, 100.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("AMP_RELEASE", "Release", releaseRange, 0.1f));
    
    // filter amp parameters
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("FILTER_ATTACK", "Attack", attackRange, 0.001f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("FILTER_DECAY", "Decay", decayRange, 1.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("FILTER_SUSTAIN", "Sustain", sustainRange, 100.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("FILTER_RELEASE", "Release", releaseRange, 0.1f));

    return { parameters.begin(), parameters.end() };
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SympleSynthAudioProcessor();
}
