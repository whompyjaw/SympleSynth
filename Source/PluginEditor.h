/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "SympleADSRComponent.h"

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

    juce::Slider filterCutoffDial;
    juce::Slider filterResDial;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterCutoffValue;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterResValue;

private:
    juce::MidiKeyboardComponent keyboardComponent;
    juce::Slider masterGainSlider;
    SympleSynthAudioProcessor& audioProcessor;
    
    SympleADSRComponent amplifier;
    SympleADSRComponent filterAmplifier;


   /* juce::ComboBox midiInputList;
    juce::Label midiInputListLabel;
    int lastInputIndex = 0;*/

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SympleSynthAudioProcessorEditor)
};
