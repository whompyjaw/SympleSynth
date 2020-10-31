/*
  ==============================================================================

    SympleSympleADSRComponent.h
    Created: 15 Oct 2020 5:39:45pm
    Author:  woz

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

struct SympleADSRParameterNames {
public:
    std::string attack;
    std::string decay;
    std::string sustain;
    std::string release;
};

class SympleADSRComponent : public juce::Component
{
public:
    SympleADSRComponent(SympleSynthAudioProcessor&);
    ~SympleADSRComponent();

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void setParameters(SympleADSRParameterNames&);
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attackValue;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> decayValue;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sustainValue;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> releaseValue;

private:
    juce::Slider attack;
    juce::Label attackLabel;

    juce::Slider decay;
    juce::Label decayLabel;

    juce::Slider sustain;
    juce::Label sustainLabel;

    juce::Slider release;
    juce::Label releaseLabel;
    
    SympleSynthAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SympleADSRComponent)
};
