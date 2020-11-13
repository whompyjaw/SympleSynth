/*
  ==============================================================================

    OscEditor.cpp
    Created: 28 Oct 2020 8:52:13pm
    Author:  Whompy Jaw

  ==============================================================================
*/

#include "OscInterface.h"

OscInterface::OscInterface(SympleSynthAudioProcessor &p) : audioProcessor(p)
{
    setSize(400, 600);

    // Add Octave Dial & Label
    addAndMakeVisible(&octDial);
    octDial.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    octDial.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    octDial.setPopupDisplayEnabled(true, true, this);
    octDial.setTextValueSuffix(" octave");
    
    addAndMakeVisible(octLabel);
    octLabel.setText("Octave", juce::dontSendNotification);
    octLabel.setJustificationType(juce::Justification::centred);
    octLabel.attachToComponent(&octDial, false);

    // Add Semitone Dial & Label
    addAndMakeVisible(&semiDial);
    semiDial.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    semiDial.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    semiDial.setPopupDisplayEnabled(true, true, this);
    semiDial.setTextValueSuffix(" semitones");

    addAndMakeVisible(semiLabel);
    semiLabel.setText("Semitone", juce::dontSendNotification);
    semiLabel.setJustificationType(juce::Justification::centred);
    semiLabel.attachToComponent(&semiDial, false);

    // Add Fine Tune Dial & Label
    addAndMakeVisible(&fineDial);
    fineDial.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    fineDial.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    fineDial.setPopupDisplayEnabled(true, true, this);
    fineDial.setTextValueSuffix(" cents");

    addAndMakeVisible(fineLabel);
    fineLabel.setText("Fine Tune", juce::dontSendNotification);
    fineLabel.setJustificationType(juce::Justification::centred);
    fineLabel.attachToComponent(&fineDial, false);

    // Add Wave Type Dial & Label
    addAndMakeVisible(&waveDial);
    waveDial.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    waveDial.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    waveDial.setPopupDisplayEnabled(true, true, this);
    waveDial.setTextValueSuffix(" wave");

    addAndMakeVisible(waveLabel);
    waveLabel.setText("Wave Type", juce::dontSendNotification);
    waveLabel.setJustificationType(juce::Justification::centred);
    waveLabel.attachToComponent(&waveDial, false);

    // Add Gain Dial & Label
    addAndMakeVisible(&gainDial);
    gainDial.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    gainDial.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    gainDial.setPopupDisplayEnabled(true, true, this);
    gainDial.setTextValueSuffix(" db");

    addAndMakeVisible(gainLabel);
    gainLabel.setText("Gain", juce::dontSendNotification);
    gainLabel.setJustificationType(juce::Justification::centred);
    gainLabel.attachToComponent(&gainDial, false);


    addAndMakeVisible(tuningLabel);
    tuningLabel.setText("Tuning", juce::dontSendNotification);
    tuningLabel.setJustificationType(juce::Justification::centred);

    // create wave select buttons
    for (int i = 0; i < 4; ++i)
    {
        auto* button = waveSelectButtons.add(new juce::TextButton());
        switch (i) {
            case 0:
                button->setButtonText("Sine");
                button->onClick = [this] { setWaveType(0); };
                break;
            case 1:
                button->setButtonText("Saw");
                button->onClick = [this] { setWaveType(1); };
                break;
            case 2:
                button->setButtonText("Square");
                button->onClick = [this] { setWaveType(2); };
                break;
            case 3:
                button->setButtonText("Tri");
                button->onClick = [this] { setWaveType(3); };
                break;
        }
        button->setRadioGroupId(groupId);
        button->setClickingTogglesState(true);
        addAndMakeVisible(*button);
    }
}

OscInterface::~OscInterface()
{
    octValue.reset();
    semiValue.reset();
    fineValue.reset();
    waveValue.reset();
    gainValue.reset();
}

void OscInterface::paint(juce::Graphics &g)
{
}

void OscInterface::resized()
{
    juce::Rectangle<int> area(0, 0, getWidth(), getHeight());
    auto dialAreaWidth = getWidth() / 4;
    auto margin = 5;
    auto labelMargin = octLabel.getHeight();

    // Set Wave Bounds
    auto waveArea = area.removeFromTop(getHeight() / 2).reduced(margin);
    auto waveHeight = waveArea.getHeight() - labelMargin;
    auto waveY = waveArea.getY() + labelMargin;
    auto waveWidth = waveArea.getWidth() / 4;

    // mount wave select buttons to the UI
    for (juce::TextButton* button : waveSelectButtons) {
        auto buttonArea = waveArea.removeFromLeft(waveWidth);
        button->setBounds(buttonArea.getX(), waveY, waveWidth, waveHeight);
    }

    // Set Tuning Label Bounds
    tuningLabel.setBounds(area.removeFromTop(labelMargin));

    // Set Tuning Dial Bounds
    auto dialWidth = (area.getWidth() - labelMargin) / 4;

    auto octArea = area.removeFromLeft(dialAreaWidth).reduced(margin);
    auto semiArea = area.removeFromLeft(dialAreaWidth).reduced(margin);
    auto fineArea = area.removeFromLeft(dialAreaWidth).reduced(margin);
    auto gainArea = area.removeFromLeft(dialAreaWidth).reduced(margin);

    octDial.setBounds(octArea.getX(), octArea.getY() + labelMargin, dialWidth, dialWidth);
    semiDial.setBounds(semiArea.getX(), semiArea.getY() + labelMargin, dialWidth, dialWidth);
    fineDial.setBounds(fineArea.getX(), fineArea.getY() + labelMargin, dialWidth, dialWidth);
    gainDial.setBounds(gainArea.getX(), gainArea.getY() + labelMargin, dialWidth, dialWidth);
}

void OscInterface::setParameters(SympleOscParameterNames& params)
{
    octValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getTree(), params.octave, octDial);
    semiValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getTree(), params.semitone, semiDial);
    fineValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getTree(), params.finetune, fineDial);
    waveValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getTree(), params.wavetype, waveDial);
    gainValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getTree(), params.gain, gainDial);
    waveParameterName = params.wavetype;
}

void OscInterface::setWaveType(int value)
{
    int currentValue = (int) audioProcessor.getTree().getParameterAsValue(waveParameterName).getValue();
    if (currentValue != value) {
        waveDial.setValue(value);
    }
}

