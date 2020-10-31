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
    juce::Slider octDial;
    juce::Label octLabel;

    std::unique_ptr<TreeSliderAttachment> osc1OctaveValue;
    juce::Slider semiDial;
    juce::Label semiLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc1SemitoneValue;
    juce::Slider fineDial;
    juce::Label fineLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc1FineTuneValue;
    juce::Slider waveDial;
    juce::Label waveLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc1WaveTypeValue;
    
//    void setLayoutParams();
    
    
    SympleSynthAudioProcessor& audioProcessor;
};
