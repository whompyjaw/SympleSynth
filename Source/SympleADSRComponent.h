/*
  ==============================================================================

    SympleSympleADSRComponent.h
    Created: 15 Oct 2020 5:39:45pm
    Author:  woz

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class SympleADSRComponent : public juce::Component
{
public:
    SympleADSRComponent(SympleSynthAudioProcessor&);
    ~SympleADSRComponent();

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attackValue;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> decayValue;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sustainValue;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> releaseValue;

private:
    juce::Slider attack;
    juce::Slider decay;
    juce::Slider sustain;
    juce::Slider release;
    
    SympleSynthAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SympleADSRComponent)
};
