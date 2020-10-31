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
    std::unique_ptr<TreeSliderAttachment> osc1OctaveValue;
    juce::Slider osc1OctaveDial;
//    std::unique_prt<TreeSliderAttachment> osc1WaveTypeValue;
    
//    void setLayoutParams();
    
    
    SympleSynthAudioProcessor& audioProcessor;
};
