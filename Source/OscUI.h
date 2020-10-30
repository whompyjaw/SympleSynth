/*
  ==============================================================================

    OscUI.h
    Created: 28 Oct 2020 8:52:13pm
    Author:  Whompy Jaw

  ==============================================================================
*/
#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class OscUI  : public juce::Component
{
public:
    OscUI (SympleSynthAudioProcessor&);
    ~OscUI() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
//    void setParameters();

    // Oscillator 1 tuning section
    juce::Slider osc1OctaveDial;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc1OctaveValue;
    juce::Slider osc1SemitoneDial;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc1SemitoneValue;
    juce::Slider osc1FineTuneDial;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc1FineTuneValue;
    juce::Slider osc1WaveTypeDial;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc1WaveTypeValue;
    
private:
    SympleSynthAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OscUI)
};
