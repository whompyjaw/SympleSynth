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
    osc1OctaveValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getTree(), "OSC_1_OCTAVE", octDial);
    
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
    osc1SemitoneValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getTree(), "OSC_1_SEMITONE", semiDial);

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
    osc1FineTuneValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getTree(), "OSC_1_FINE_TUNE", fineDial);

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
    osc1WaveTypeValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getTree(), "OSC_1_WAVE_TYPE", waveDial);

    addAndMakeVisible(waveLabel);
    waveLabel.setText("Wave Type", juce::dontSendNotification);
    waveLabel.setJustificationType(juce::Justification::centred);
    waveLabel.attachToComponent(&waveDial, false);

    addAndMakeVisible(tuningLabel);
    tuningLabel.setText("Tuning", juce::dontSendNotification);
    tuningLabel.setJustificationType(juce::Justification::centred);
}

OscInterface::~OscInterface()
{
}

void OscInterface::paint(juce::Graphics &g)
{
}

void OscInterface::resized()
{
    juce::Rectangle<int> area(0, 0, getWidth(), getHeight());
    auto dialAreaWidth = getWidth() / 3;
    auto margin = 5;
    auto labelMargin = octLabel.getHeight();

    // Set Wave Bounds
    auto waveArea = area.removeFromTop(getHeight() / 2).reduced(margin);
    auto waveHeight = waveArea.getHeight() - labelMargin;
    auto waveY = waveArea.getY() + labelMargin;

    waveDial.setBounds(waveArea.getX(), waveY, waveArea.getWidth(), waveHeight);

    // Set Tuning Label Bounds
    tuningLabel.setBounds(area.removeFromTop(labelMargin));

    // Set Tuning Dial Bounds
    auto dialWidth = (area.getWidth() - labelMargin) / 4;

    auto octArea = area.removeFromLeft(dialAreaWidth).reduced(margin);
    auto semiArea = area.removeFromLeft(dialAreaWidth).reduced(margin);
    auto fineArea = area.removeFromLeft(dialAreaWidth).reduced(margin);

    octDial.setBounds(octArea.getX(), octArea.getY() + labelMargin, dialWidth, dialWidth);
    semiDial.setBounds(semiArea.getX(), semiArea.getY() + labelMargin, dialWidth, dialWidth);
    fineDial.setBounds(fineArea.getX(), fineArea.getY() + labelMargin, dialWidth, dialWidth);

}

//void OscInterface::setLayoutParams()
//{
//    juce::NormalisableRange<float> oscillatorOctaveParams (-2, 2, 1);
//    audioProcessor.parameters.push_back(std::make_unique<juce::AudioParameterFloat>("OSC_1_OCTAVE", "Octave 1", oscillatorOctaveParams, 0, "Octave"));
//}

