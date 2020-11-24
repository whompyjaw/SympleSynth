/*
  ==============================================================================

    SympleFilterComponent.cpp
    Created: 15 Oct 2020 5:40:08pm
    Author:  woz

  ==============================================================================
*/

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "FilterInterface.h"

SympleFilterComponent::SympleFilterComponent(SympleSynthAudioProcessor& p) 
    :   audioProcessor(p),
        envelope(p)
{
    setSize (400, 400);
    
    addAndMakeVisible(&filterModeDial);
    filterModeDial.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    filterModeDial.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    
    addAndMakeVisible(filterModeLabel);
    filterModeLabel.setText("Filter Mode", juce::dontSendNotification);
    filterModeLabel.setJustificationType(juce::Justification::centred);
    filterModeLabel.attachToComponent(&filterModeDial, false);
    
    //  Add Filter Cutoff Dial & Label
    addAndMakeVisible(&filterCutoffDial);
    filterCutoffDial.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    filterCutoffDial.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    filterCutoffDial.setPopupDisplayEnabled(true, true, this);
    filterCutoffDial.setTextValueSuffix(" hz");

    addAndMakeVisible(cutoffLabel);
    cutoffLabel.setText("Cutoff", juce::dontSendNotification);
    cutoffLabel.setJustificationType(juce::Justification::centred);
    cutoffLabel.attachToComponent(&filterCutoffDial, false);

    //  Add Filter Res Dial & Label
    addAndMakeVisible(&filterResDial);
    filterResDial.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    filterResDial.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    filterResDial.setPopupDisplayEnabled(true, true, this);
    filterResDial.setTextValueSuffix("%");

    addAndMakeVisible(resLabel);
    resLabel.setText("Resonance", juce::dontSendNotification);
    resLabel.setJustificationType(juce::Justification::centred);
    resLabel.attachToComponent(&filterResDial, false);

    //  Add Env Amount Dial & Label
    addAndMakeVisible(&filterAmountDial);
    filterAmountDial.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    filterAmountDial.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    filterAmountDial.setPopupDisplayEnabled(true, true, this);
    filterAmountDial.setTextValueSuffix(" semitones");

    addAndMakeVisible(amountLabel);
    amountLabel.setText("Env Amount", juce::dontSendNotification);
    amountLabel.setJustificationType(juce::Justification::centred);
    amountLabel.attachToComponent(&filterAmountDial, false);
    
    fontSize = 11.0;
    addAndMakeVisible(filterModeLabelLP12);
    filterModeLabelLP12.setText("LP12", juce::dontSendNotification);
    filterModeLabelLP12.setFont(juce::Font(fontSize));
    filterModeLabelLP12.attachToComponent(&filterModeDial, false);
    //filterModeLabelLP12.setColour(juce::Label::backgroundColourId, juce::Colours::red);
    
    addAndMakeVisible(filterModeLabelHP12);
    filterModeLabelHP12.setText("HP12", juce::dontSendNotification);
    filterModeLabelHP12.setFont(juce::Font(fontSize));
    filterModeLabelHP12.attachToComponent(&filterModeDial, false);

    addAndMakeVisible(filterModeLabelBP12);
    filterModeLabelBP12.setText("BP12", juce::dontSendNotification);
    filterModeLabelBP12.setFont(juce::Font(fontSize));
    filterModeLabelBP12.attachToComponent(&filterModeDial, false);
    
    addAndMakeVisible(filterModeLabelLP24);
    filterModeLabelLP24.setText("LP24", juce::dontSendNotification);
    filterModeLabelLP24.setFont(juce::Font(fontSize));
    filterModeLabelLP24.attachToComponent(&filterModeDial, false);
    
    addAndMakeVisible(filterModeLabelHP24);
    filterModeLabelHP24.setText("HP24", juce::dontSendNotification);
    filterModeLabelHP24.setFont(juce::Font(fontSize));
    filterModeLabelHP24.attachToComponent(&filterModeDial, false);
    
    addAndMakeVisible(filterModeLabelBP24);
    filterModeLabelBP24.setText("BP24", juce::dontSendNotification);
    filterModeLabelBP24.setFont(juce::Font(fontSize));
    filterModeLabelBP24.attachToComponent(&filterModeDial, false);
    
    
    addAndMakeVisible(envelope);

    addAndMakeVisible(envLabel);
    envLabel.setText("Filter Envelope", juce::dontSendNotification);
    envLabel.setJustificationType(juce::Justification::centred);
    envLabel.attachToComponent(&envelope, false);
}

SympleFilterComponent::~SympleFilterComponent()
{
    filterCutoffValue.reset();
    filterResValue.reset();
    filterAmountValue.reset();
    filterModeValue.reset();
}

void SympleFilterComponent::paint(juce::Graphics& g)
{
//    g.fillAll(juce::Colours::grey);

}

void SympleFilterComponent::resized()
{
    juce::Rectangle<int> area(0,0, getWidth(), getHeight());
    auto filterWidth = getWidth() / 4;
    auto margin = 5;
    auto labelMargin = amountLabel.getHeight();

    // Set Amp Bounds
    auto envArea = area.removeFromBottom(getHeight() / 2).reduced(margin);
    auto envHeight = envArea.getHeight() - labelMargin;
    auto envY = envArea.getY() + labelMargin;

    envelope.setBounds(envArea.getX(), envY, envArea.getWidth(), envHeight);

    // Set Filter Dial Bounds
    auto modeArea = area.removeFromLeft(filterWidth).reduced(margin);
    auto cutoffArea = area.removeFromLeft(filterWidth).reduced(margin);
    auto resArea = area.removeFromLeft(filterWidth).reduced(margin);
    auto amountArea = area.removeFromLeft(filterWidth).reduced(margin);


    filterModeDial.setBounds(modeArea.getX()+5, modeArea.getY() + labelMargin, modeArea.getWidth(), envHeight);
    filterCutoffDial.setBounds(cutoffArea.getX(), cutoffArea.getY() + labelMargin, cutoffArea.getWidth(), envHeight);
    filterResDial.setBounds(resArea.getX(), resArea.getY() + labelMargin, resArea.getWidth(), envHeight);
    filterAmountDial.setBounds(amountArea.getX(), amountArea.getY() + labelMargin, amountArea.getWidth(), envHeight);
    

    int rotaryLabelWidth = 30;
    int rotaryLabelHeight = 15;
    filterModeLabelLP12.setBounds(filterModeDial.getX() + 4, filterModeDial.getY()+2 + filterModeDial.getHeight() - labelMargin - 14, rotaryLabelWidth, rotaryLabelHeight);
    filterModeLabelHP12.setBounds(filterModeDial.getX() - 10, filterModeDial.getY() + filterModeDial.getHeight() - labelMargin - 40, rotaryLabelWidth, rotaryLabelHeight);
    filterModeLabelBP12.setBounds(filterModeDial.getX() + 4 , filterModeDial.getY() + filterModeDial.getHeight() - labelMargin - 62, rotaryLabelWidth, rotaryLabelHeight);
    
    
    filterModeLabelLP24.setBounds(filterModeDial.getX() + filterModeDial.getWidth() - 34, filterModeDial.getY() + filterModeDial.getHeight() - labelMargin - 62, rotaryLabelWidth, rotaryLabelHeight);
    filterModeLabelHP24.setBounds(filterModeDial.getX() + filterModeDial.getWidth() - 21, filterModeDial.getY() + filterModeDial.getHeight() - labelMargin - 40, rotaryLabelWidth, rotaryLabelHeight);
    filterModeLabelBP24.setBounds(filterModeDial.getX() + filterModeDial.getWidth() - 34, filterModeDial.getY()+2 + filterModeDial.getHeight() - labelMargin - 14, rotaryLabelWidth, rotaryLabelHeight);
}

void SympleFilterComponent::setParameters(SympleFilterParameterNames& params, SympleADSRParameterNames& envNames) {
    filterCutoffValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getTree(), params.cutoff, filterCutoffDial);
    filterResValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getTree(), params.resonance, filterResDial);
    filterAmountValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getTree(), params.amount, filterAmountDial);
    filterModeValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getTree(), params.mode, filterModeDial);
    envelope.setParameters(envNames);
}
