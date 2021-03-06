/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Voice.h"

//==============================================================================
/**
*/
class SympleSynthAudioProcessor : public juce::AudioProcessor
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

    juce::MidiKeyboardState& getKeyboardState();
    void prepareVoices(juce::dsp::ProcessSpec&);
    juce::AudioProcessorValueTreeState& getTree() { return tree; }
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> parameters;
    
    Oscillator lfo;
    juce::dsp::AudioBlock<float> lfoBuffer;

private:
    const int VOICE_COUNT = 5;
    
    juce::Synthesiser synth;
    juce::MidiKeyboardState keyboardState;

    juce::AudioProcessorValueTreeState tree;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();

    float lastSampleRate;
    juce::HeapBlock<char> heapBlock;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SympleSynthAudioProcessor)
};
