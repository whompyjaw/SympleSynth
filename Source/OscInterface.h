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
    void setParams();
    
    using TreeSliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<TreeSliderAttachment> osc1OctaveValue;
    
private:
    juce::Slider osc1OctaveDial;
    SympleSynthAudioProcessor& audioProcessor;
};
