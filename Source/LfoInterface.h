/*
  ==============================================================================

    LfoInterface.h
    Created: 4 Nov 2020 9:27:32pm
    Author:  woz

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/*
*/
class LfoInterface  : public juce::Component
{
public:
    LfoInterface(SympleSynthAudioProcessor&);
    ~LfoInterface() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> frequencyValue;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> amountValue;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> waveTypeValue;

private:
    SympleSynthAudioProcessor& audioProcessor;

    juce::Slider frequencyDial;
    juce::Label frequencyLabel;
    
    juce::Slider amountDial;
    juce::Label amountLabel;

    juce::Slider waveDial;
    juce::Label waveLabel;
    juce::Label waveLabel2;
    juce::Label waveLabel3;
    juce::Label waveLabel4;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LfoInterface)
};
