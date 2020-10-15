/*
  ==============================================================================

    SympleSympleADSRComponent.h
    Created: 15 Oct 2020 5:39:45pm
    Author:  woz

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class SympleADSRComponent : public juce::Component,
                            private juce::Slider::Listener
{
public:
    SympleADSRComponent(juce::ADSR::Parameters&);
    ~SympleADSRComponent();

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void sliderValueChanged(juce::Slider* slider) override;
    
    juce::Slider attack;
    juce::Slider decay;
    juce::Slider sustain;
    juce::Slider release;
    
    void useParameters(juce::ADSR::Parameters&);
    
    juce::ADSR::Parameters& params;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SympleADSRComponent)
};
