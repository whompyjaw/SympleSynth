/*
  ==============================================================================

    NoiseOscInterface.cpp
    Created: 18 Nov 2020 10:37:33pm
    Author:  woz

  ==============================================================================
*/

#include "NoiseOscInterface.h"

NoiseOscInterface::NoiseOscInterface(SympleSynthAudioProcessor& p) : audioProcessor(p)
{
    setSize(400, 600);
    
    addAndMakeVisible(&noiseDial);
    noiseDial.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    noiseDial.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    noiseDial.setPopupDisplayEnabled(true, true, this);
    noiseDial.setTextValueSuffix(" db");

    // add frequency label
    addAndMakeVisible(noiseLabel);
    noiseLabel.setText("Noise", juce::dontSendNotification);
    noiseLabel.setJustificationType(juce::Justification::centred);
    noiseLabel.attachToComponent(&noiseDial, false);
}

NoiseOscInterface::~NoiseOscInterface()
{
    noiseValue.reset();
}

void NoiseOscInterface::paint (juce::Graphics& g)
{
}

void NoiseOscInterface::resized()
{
    juce::Rectangle<int> area(0, 0, getWidth(), getHeight());
    auto margin = 5;
    auto labelMargin = noiseLabel.getHeight() - margin;
    int popupMargin = labelMargin + 15; // add extra margin for popup display
    noiseDial.setBounds(area.getX() + margin,
                        area.getY() + popupMargin,
                        area.getWidth() - margin,
                        area.getHeight() - labelMargin - margin);
}

void NoiseOscInterface::setAttachmentParameter(std::string& parameter)
{
    noiseValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getTree(), parameter, noiseDial);
}
