/*
  ==============================================================================

    OscEditor.cpp
    Created: 28 Oct 2020 8:52:13pm
    Author:  Whompy Jaw

  ==============================================================================
*/

#include "OscInterface.h"

OscInterface::OscInterface(SympleSynthAudioProcessor &p)
    : audioProcessor(p),
      noise(p)
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
    waveDial.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    waveDial.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    waveDial.setColour(juce::Slider::thumbColourId, juce::Colours::floralwhite);

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

    // add noise dial
    addAndMakeVisible(&noise);
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
    auto margin = 5;
    auto labelMargin = octLabel.getHeight();

    // Set Wave Bounds
    auto waveLabelArea = area.removeFromTop(getHeight() / 4).reduced(margin);
    auto waveHeight = labelMargin;
    auto waveLabelY = waveLabelArea.getY() + waveLabelArea.getHeight()/2;
    auto dialAreaWidth = waveLabelArea.getWidth() / 4;
    auto fourFifthsWidth = ((4.0 * waveLabelArea.getWidth()) / 5.0);
    auto labelAreaWidth = fourFifthsWidth / 4;

    auto sineArea = waveLabelArea.removeFromLeft(labelAreaWidth);
    auto sawArea = waveLabelArea.removeFromLeft(labelAreaWidth);
    auto squareArea = waveLabelArea.removeFromLeft(labelAreaWidth);
    auto triArea = waveLabelArea.removeFromLeft(labelAreaWidth);

    waveLabel.setBounds(sineArea.getX(), waveLabelY, labelAreaWidth, waveHeight);
    waveLabel2.setBounds(sawArea.getX(), waveLabelY, labelAreaWidth, waveHeight);
    waveLabel3.setBounds(squareArea.getX(), waveLabelY, labelAreaWidth, waveHeight);
    waveLabel4.setBounds(triArea.getX(), waveLabelY, labelAreaWidth, waveHeight);

    auto waveSliderArea = area.removeFromTop(getHeight() / 4).reduced(margin);
    auto noiseWidth = waveSliderArea.getWidth() - fourFifthsWidth;
    auto noiseArea = waveSliderArea.removeFromRight(noiseWidth);
    waveSliderArea.removeFromLeft(labelAreaWidth / 2 - margin * 2);
    waveSliderArea.removeFromRight(labelAreaWidth / 2 - margin * 2);

    auto waveY = waveSliderArea.getY();

    waveDial.setBounds(waveSliderArea.getX(), waveY, waveSliderArea.getWidth(), waveHeight);
    
    // add noise knob
    int noiseMargin = 15;
    noise.setBounds(noiseArea.getX() - noiseMargin, waveLabelY - 9, noiseWidth + noiseMargin, 3 * waveHeight);

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
    noise.setAttachmentParameter(params.noise);
}

