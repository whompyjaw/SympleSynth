/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Envelope.h"
#include "FilterInterface.h"
#include "MasterAmp.h"
#include "OscInterface.h"
#include "LfoInterface.h"

//==============================================================================
/**
*/
class SympleSynthAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Slider::Listener
{
public:
    SympleSynthAudioProcessorEditor (SympleSynthAudioProcessor&);
    ~SympleSynthAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    void sliderValueChanged(juce::Slider* slider) override;

    // Filter
    juce::Slider filterCutoffDial;
    juce::Slider filterResDial;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterCutoffValue;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterResValue;
    SympleFilterParameterNames filterParameters;
    

private:
    juce::MidiKeyboardComponent keyboardComponent;
    SympleSynthAudioProcessor& audioProcessor;
    
    OscInterface osc1;
    OscInterface osc2;
    SympleFilterComponent filter;
    SympleFilterComponent filter2;
    MasterAmp amplifier;
    LfoInterface lfo;

    juce::Label titleLabel;

    juce::Label osc1Label;
    juce::Label osc2Label;

    juce::Label filter1Label;
    juce::Label filter2Label;

    juce::Label ampLabel;
    juce::Label lfoLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SympleSynthAudioProcessorEditor)
};
