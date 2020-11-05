/*
  ==============================================================================

    LfoInterface.cpp
    Created: 4 Nov 2020 9:27:19pm
    Author:  woz

  ==============================================================================
*/

#include <JuceHeader.h>
#include "LfoInterface.h"

LfoInterface::LfoInterface(SympleSynthAudioProcessor& p) : audioProcessor(p)
{
    setSize(400, 200);
    
    //  add lfo frequency dial
    addAndMakeVisible(&frequencyDial);
    frequencyDial.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    frequencyDial.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    frequencyDial.setPopupDisplayEnabled(true, true, this);
    frequencyDial.setTextValueSuffix(" hz");
    
    // add frequency label
    addAndMakeVisible(frequencyLabel);
    frequencyLabel.setText("Frequency", juce::dontSendNotification);
    frequencyLabel.setJustificationType(juce::Justification::centred);
    frequencyLabel.attachToComponent(&frequencyDial, false);
    
    frequencyValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getTree(), "LFO_FREQUENCY", frequencyDial);
}

LfoInterface::~LfoInterface()
{
    frequencyValue.reset();
}

void LfoInterface::paint (juce::Graphics&)
{
}

void LfoInterface::resized()
{
    juce::Rectangle<int> area(0, 0, getWidth(), getHeight());
    auto margin = 5;
    auto labelMargin = frequencyLabel.getHeight();

    auto frequencyArea = area.removeFromBottom(area.getHeight() / 2).reduced(margin);
    auto frequencyHeight = frequencyArea.getHeight() - labelMargin;
    frequencyDial.setBounds(frequencyArea.getX(), frequencyArea.getY() + labelMargin, frequencyArea.getWidth() / 3, frequencyHeight);
}
