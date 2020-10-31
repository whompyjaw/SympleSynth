/*
  ==============================================================================

    MasterAmp.cpp
    Created: 30 Oct 2020 6:18:34pm
    Author:  Nathan

  ==============================================================================
*/

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "MasterAmp.h"

//==============================================================================
MasterAmp::MasterAmp(SympleSynthAudioProcessor& p) 
    :   audioProcessor(p),
        amplifier(p)
{
    setSize(400, 200);

    // init amp parameter names struct
    SympleADSRParameterNames ampParameters;
    ampParameters.attack = "AMP_ATTACK";
    ampParameters.decay = "AMP_DECAY";
    ampParameters.sustain = "AMP_SUSTAIN";
    ampParameters.release = "AMP_RELEASE";
    amplifier.setParameters(ampParameters);

    // Add Amp & Label
    addAndMakeVisible(amplifier);

    addAndMakeVisible(ampLabel);
    ampLabel.setText("Amplifier Envelope", juce::dontSendNotification);
    ampLabel.setJustificationType(juce::Justification::centred);
    ampLabel.attachToComponent(&amplifier, false);

    // Add Master Gain & Label
    addAndMakeVisible(masterGainSlider);
    masterGainSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    masterGainSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    masterGainSlider.setPopupDisplayEnabled(true, true, this);
    masterGainSlider.setTextValueSuffix(" db");

    addAndMakeVisible(gainLabel);
    gainLabel.setText("Master Gain", juce::dontSendNotification);
    gainLabel.setJustificationType(juce::Justification::centred);
    gainLabel.attachToComponent(&masterGainSlider, false);
}

MasterAmp::~MasterAmp()
{
    masterGainValue.reset();
}

void MasterAmp::paint (juce::Graphics& g)
{
}

void MasterAmp::resized()
{
    juce::Rectangle<int> area(0, 0, getWidth(), getHeight());
    auto margin = 5;
    auto labelMargin = gainLabel.getHeight();

    // Set Amp Bounds
    auto ampArea = area.removeFromBottom(area.getHeight() / 2).reduced(margin);
    auto ampHeight = ampArea.getHeight() - labelMargin;
    auto ampY = ampArea.getY() + labelMargin;
    amplifier.setBounds(ampArea.getX(), ampY, ampArea.getWidth(), ampHeight);

    // Set Master Gain Bounds
    auto gainArea = area.removeFromBottom(area.getHeight()).reduced(margin);
    masterGainSlider.setBounds(gainArea.getX(), gainArea.getY() + labelMargin, gainArea.getWidth(), ampHeight);
}

void MasterAmp::setParameters(MasterAmpParameterNames& params) 
{
    masterGainValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getTree(), params.gain, masterGainSlider);
}
