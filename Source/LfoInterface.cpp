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
    
    // add lfo amount dial
    addAndMakeVisible(&amountDial);
    amountDial.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    amountDial.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    amountDial.setPopupDisplayEnabled(true, true, this);
    amountDial.setTextValueSuffix(" semitones");
    
    // add frequency label
    addAndMakeVisible(frequencyLabel);
    frequencyLabel.setText("Frequency", juce::dontSendNotification);
    frequencyLabel.setJustificationType(juce::Justification::centred);
    frequencyLabel.attachToComponent(&frequencyDial, false);
    
    // add amount label
    addAndMakeVisible(amountLabel);
    amountLabel.setText("Amount", juce::dontSendNotification);
    amountLabel.setJustificationType(juce::Justification::centred);
    amountLabel.attachToComponent(&amountDial, false);
    
    frequencyValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getTree(), "LFO_FREQUENCY", frequencyDial);
    amountValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getTree(), "LFO_AMOUNT", amountDial);
}

LfoInterface::~LfoInterface()
{
    frequencyValue.reset();
    amountValue.reset();
}

void LfoInterface::paint (juce::Graphics&)
{
}

void LfoInterface::resized()
{
    juce::Rectangle<int> area(0, 0, getWidth(), getHeight());
    auto margin = 5;
    auto labelMargin = frequencyLabel.getHeight();
    auto rowHeight = area.getHeight() / 2;
    auto colWidth = area.getWidth() / 3;

    auto oscArea = area.removeFromTop(rowHeight).reduced(margin);
    auto paramsArea = area.removeFromTop(rowHeight).reduced(margin);
    auto knobHeight = paramsArea.getHeight() - labelMargin;
    
    auto frequencyArea = paramsArea.removeFromLeft(colWidth);
    auto blankArea = paramsArea.removeFromLeft(colWidth);
    auto amountArea = paramsArea.removeFromLeft(colWidth);
    frequencyDial.setBounds(frequencyArea.getX(), frequencyArea.getY() + labelMargin, colWidth, knobHeight);
    amountDial.setBounds(amountArea.getX(), amountArea.getY() + labelMargin, colWidth, knobHeight);
}