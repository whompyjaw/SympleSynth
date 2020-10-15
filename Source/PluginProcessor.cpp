/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

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
                       )
#endif
{
    // initialize amplifier parameters
    ampParameters = {0.001, 1.0, 1.0, 0.2};

    synth.clearVoices();
    for (int i = 0; i < 8; ++i)
    {
        synth.addVoice(new SineWaveVoice());
    }

    synth.clearSounds();
    synth.addSound(new SineWaveSound());
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
    //synth.prepareToPlay(samplesPerBlock, sampleRate);
    lastSampleRate = sampleRate; // this is in case the sample rate is changed while the synth is being used so it doesn't 
    synth.setCurrentPlaybackSampleRate(lastSampleRate);
}

/* Gets called when the application is closed. */
void SympleSynthAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    keyboardState.reset();
    /*synthAudioSource.releaseResources();*/

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
    buffer.clear();
    keyboardState.processNextMidiBuffer (midiMessages, 0,
                                         buffer.getNumSamples(), true);
    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
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

juce::ADSR::Parameters& SympleSynthAudioProcessor::getAmpParameters()
{
    return ampParameters;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SympleSynthAudioProcessor();
}
