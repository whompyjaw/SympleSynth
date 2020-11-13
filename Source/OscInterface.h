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

struct SympleOscParameterNames {
public:
    std::string octave;
    std::string semitone;
    std::string finetune;
    std::string wavetype;
    std::string gain;
};


class OscInterface : public juce::Component
{
public:
    OscInterface(SympleSynthAudioProcessor&);
    ~OscInterface();
    
    void paint (juce::Graphics&) override;
    void resized() override;
    void setParameters(SympleOscParameterNames&);

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> octValue;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> semiValue;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> fineValue;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> waveValue;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainValue;

private:
    juce::Label tuningLabel;

    juce::Slider octDial;
    juce::Label octLabel;

    juce::Slider semiDial;
    juce::Label semiLabel;

    juce::Slider fineDial;
    juce::Label fineLabel;

    juce::Slider waveDial;
    juce::Label waveLabel;

    juce::Slider gainDial;
    juce::Label gainLabel;    
    
    SympleSynthAudioProcessor& audioProcessor;
};
