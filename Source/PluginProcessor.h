/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "SympleVoice.h"

//==============================================================================
/**
*/
class SympleSynthAudioProcessor : public juce::AudioProcessor,
                                  juce::AudioProcessorValueTreeState::Listener
{
public:
    //==============================================================================
    SympleSynthAudioProcessor();
    ~SympleSynthAudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    float masterGain { 0.5f }; // Q: What is this syntax? Never seen
    
    void setUpValueTreeListeners();

    juce::MidiKeyboardState& getKeyboardState();
    juce::ADSR::Parameters& getAmpParameters();
    void setAmpParameters(juce::ADSR::Parameters&);
    void parameterChanged(const juce::String&, float) override;

    void filterNextBlock(juce::AudioBuffer<float>&);

    juce::AudioProcessorValueTreeState& getTree() { return tree; }


private:
    const int VOICE_COUNT = 64;
    const int FILTER_UPDATE_RATE = 100; // the number of samples each filter setting will process
    
    juce::Synthesiser synth;
    
    juce::MidiKeyboardState keyboardState;
    
    juce::ADSR::Parameters ampParameters;
    juce::ADSR::Parameters filterAmpParameters;
    juce::ADSR filterAmp;
    SympleVoice* synthVoice;

    juce::dsp::ProcessorDuplicator <juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients <float>> lowPassFilter;

    juce::AudioProcessorValueTreeState tree;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();

    float lastSampleRate;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SympleSynthAudioProcessor)
};
