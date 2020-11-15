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
    setSize(400, 600);
    
    addAndMakeVisible(&waveDial);
    waveDial.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    waveDial.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);

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

    addAndMakeVisible(waveLabel);
    waveLabel.setText("Sine", juce::dontSendNotification);
    waveLabel.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(waveLabel2);
    waveLabel2.setText("Saw", juce::dontSendNotification);
    waveLabel2.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(waveLabel3);
    waveLabel3.setText("Square", juce::dontSendNotification);
    waveLabel3.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(waveLabel4);
    waveLabel4.setText("Triangle", juce::dontSendNotification);
    waveLabel4.setJustificationType(juce::Justification::centred);

    frequencyValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getTree(), "LFO_FREQUENCY", frequencyDial);
    amountValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getTree(), "LFO_AMOUNT", amountDial);
    waveTypeValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getTree(), "LFO_WAVE_TYPE", waveDial);
}

LfoInterface::~LfoInterface()
{
    frequencyValue.reset();
    amountValue.reset();
    waveTypeValue.reset();
}

void LfoInterface::paint (juce::Graphics& g)
{
    //g.fillAll(juce::Colours::white);
}

void LfoInterface::resized()
{
    juce::Rectangle<int> area(0, 0, getWidth(), getHeight());
    auto margin = 5;
    auto labelMargin = amountLabel.getHeight();

    // Set Wave Bounds
    auto waveLabelArea = area.removeFromTop(getHeight() / 4).reduced(margin);
    auto waveHeight = labelMargin;
    auto waveLabelY = waveLabelArea.getY() + waveLabelArea.getHeight() / 2;
    auto dialAreaWidth = waveLabelArea.getWidth() / 4;

    auto sineArea = waveLabelArea.removeFromLeft(dialAreaWidth);
    auto sawArea = waveLabelArea.removeFromLeft(dialAreaWidth);
    auto squareArea = waveLabelArea.removeFromLeft(dialAreaWidth);
    auto triArea = waveLabelArea.removeFromLeft(dialAreaWidth);

    waveLabel.setBounds(sineArea.getX(), waveLabelY, dialAreaWidth, waveHeight);
    waveLabel2.setBounds(sawArea.getX(), waveLabelY, dialAreaWidth, waveHeight);
    waveLabel3.setBounds(squareArea.getX(), waveLabelY, dialAreaWidth, waveHeight);
    waveLabel4.setBounds(triArea.getX(), waveLabelY, dialAreaWidth, waveHeight);

    auto waveSliderArea = area.removeFromTop(getHeight() / 4).reduced(margin);
    waveSliderArea.removeFromLeft(dialAreaWidth / 2 - margin * 2);
    waveSliderArea.removeFromRight(dialAreaWidth / 2 - margin * 2);

    auto waveY = waveSliderArea.getY();

    waveDial.setBounds(waveSliderArea.getX(), waveY, waveSliderArea.getWidth(), waveHeight);

    auto dialWidth = (area.getWidth() - labelMargin) / 2;
    
    auto frequencyArea = area.removeFromLeft(dialWidth).reduced(margin);
    auto amountArea = area.removeFromLeft(dialWidth).reduced(margin);
    
    frequencyDial.setBounds(frequencyArea.getX(), frequencyArea.getY() + labelMargin, dialWidth, dialWidth/1.5);
    amountDial.setBounds(amountArea.getX(), amountArea.getY() + labelMargin, dialWidth, dialWidth/1.5);
}
