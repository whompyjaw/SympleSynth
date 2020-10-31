/*
  ==============================================================================

    MasterAmp.h
    Created: 30 Oct 2020 6:18:34pm
    Author:  Nathan

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Envelope.h"

//==============================================================================
/*
*/
struct MasterAmpParameterNames {
public:
    std::string gain;
};


class MasterAmp  : public juce::Component
{
public:
    MasterAmp(SympleSynthAudioProcessor&);
    ~MasterAmp();

    void paint (juce::Graphics&) override;
    void resized() override;
    void setParameters(MasterAmpParameterNames&);


    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> masterGainValue;

private:
    SympleADSRComponent amplifier;
    juce::Label ampLabel;

    juce::Slider masterGainSlider;
    juce::Label gainLabel;

    SympleSynthAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MasterAmp)
};
