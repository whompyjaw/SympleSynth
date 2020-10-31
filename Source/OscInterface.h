/*
  ==============================================================================

    OscEditor.h
    Created: 28 Oct 2020 8:52:13pm
    Author:  Whompy Jaw

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class OscInterface : public juce::Component
{
public:
    OscInterface(SympleSynthAudioProcessor&);
    ~OscInterface();
    
    void paint (juce::Graphics&) override;
    void resized() override;

    
    
    
private:
    using TreeSliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    juce::Slider osc1OctaveDial;
    std::unique_ptr<TreeSliderAttachment> osc1OctaveValue;
    juce::Slider osc1SemitoneDial;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc1SemitoneValue;
    juce::Slider osc1FineTuneDial;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc1FineTuneValue;
    juce::Slider osc1WaveTypeDial;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc1WaveTypeValue;
    
//    void setLayoutParams();
    
    
    SympleSynthAudioProcessor& audioProcessor;
};
