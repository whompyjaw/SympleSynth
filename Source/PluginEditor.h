/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Envelope.h"
#include "Filter.h"
#include "MasterAmp.h"

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
    juce::MidiKeyboardComponent keyboardComponent;
    juce::Slider masterGainSlider;
    SympleSynthAudioProcessor& audioProcessor;
    
    SympleFilterComponent filter;
    //SympleADSRComponent amplifier;
    MasterAmp amplifier;
    //SympleADSRComponent filterAmplifier;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SympleSynthAudioProcessorEditor)
};
