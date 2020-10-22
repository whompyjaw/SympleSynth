/*
  ==============================================================================

    SympleSympleFilterComponent.h
    Created: 21 Oct 2020
    Author: Nathan Johnson

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>


struct SympleFilterParameterNames {
public:
    std::string cutoff;
    std::string resonance;
};

class SympleFilterComponent : public juce::Component
{
public:
    SympleFilterComponent(SympleSynthAudioProcessor&);
    ~SympleFilterComponent();

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void setParameters(SympleFilterParameterNames&);
    

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterCutoffValue;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterResValue;

private:
    juce::Slider filterCutoffDial;
    juce::Slider filterResDial;
    
    SympleSynthAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SympleFilterComponent)
};
