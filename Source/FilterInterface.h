/*
  ==============================================================================

    SympleSympleFilterComponent.h
    Created: 21 Oct 2020
    Author: Nathan Johnson

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "Envelope.h"


struct SympleFilterParameterNames {
public:
    std::string cutoff;
    std::string resonance;
    std::string amount;
    std::string mode;
};

class SympleFilterComponent : public juce::Component
{
public:
    SympleFilterComponent(SympleSynthAudioProcessor&);
    ~SympleFilterComponent();

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void setParameters(SympleFilterParameterNames&, SympleADSRParameterNames&);
    

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterCutoffValue;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterResValue;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterAmountValue;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterModeValue;


private:
    juce::Slider filterCutoffDial;
    juce::Label cutoffLabel;

    juce::Slider filterResDial;
    juce::Label resLabel;

    juce::Slider filterAmountDial;
    juce::Label amountLabel;
    
    juce::Slider filterModeDial;
    juce::Label filterModeLabel;

    SympleADSRComponent envelope;
    juce::Label envLabel;
    
    juce::Label filterModeLabelLP12;
    
    SympleSynthAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SympleFilterComponent)
};
