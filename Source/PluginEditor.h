/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "SympleADSRComponent.h"
#include "SympleFilterComponent.h"

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



private:
    juce::MidiKeyboardComponent keyboardComponent;
    juce::Slider masterGainSlider;
    SympleSynthAudioProcessor& audioProcessor;
    
    SympleFilterComponent filter;
    SympleADSRComponent amplifier;
    SympleADSRComponent filterAmplifier;


   /* juce::ComboBox midiInputList;
    juce::Label midiInputListLabel;
    int lastInputIndex = 0;*/

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SympleSynthAudioProcessorEditor)
};
