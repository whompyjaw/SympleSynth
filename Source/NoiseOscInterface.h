/*
  ==============================================================================

    NoiseOscInterface.h
    Created: 18 Nov 2020 10:37:22pm
    Author:  woz

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class NoiseOscInterface  : public juce::Component
{
public:
    NoiseOscInterface(SympleSynthAudioProcessor&);
    ~NoiseOscInterface() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void setAttachmentParameter(std::string&);
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> noiseValue;

private:
    SympleSynthAudioProcessor& audioProcessor;

    juce::Slider noiseDial;
    juce::Label noiseLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NoiseOscInterface)
};
