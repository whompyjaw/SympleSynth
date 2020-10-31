/*
  ==============================================================================

    SympleFilterComponent.cpp
    Created: 15 Oct 2020 5:40:08pm
    Author:  woz

  ==============================================================================
*/

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Filter.h"

SympleFilterComponent::SympleFilterComponent(SympleSynthAudioProcessor& p) 
    :   audioProcessor(p),
        amplifier(p)
{
    setSize (400, 200);
    
    //  Add Filter Dials & Labels
    addAndMakeVisible(&filterCutoffDial);
    filterCutoffDial.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    filterCutoffDial.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    filterCutoffDial.setPopupDisplayEnabled(true, true, this);
    filterCutoffDial.setTextValueSuffix(" hz");

    addAndMakeVisible(cutoffLabel);
    cutoffLabel.setText("Cutoff", juce::dontSendNotification);
    cutoffLabel.setJustificationType(juce::Justification::centred);
    cutoffLabel.attachToComponent(&filterCutoffDial, false);
    
    addAndMakeVisible(&filterResDial);
    filterResDial.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    filterResDial.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    filterResDial.setPopupDisplayEnabled(true, true, this);
    filterResDial.setTextValueSuffix("%");

    addAndMakeVisible(resLabel);
    resLabel.setText("Resonance", juce::dontSendNotification);
    resLabel.setJustificationType(juce::Justification::centred);
    resLabel.attachToComponent(&filterResDial, false);

    addAndMakeVisible(&filterAmountDial);
    filterAmountDial.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    filterAmountDial.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    filterAmountDial.setPopupDisplayEnabled(true, true, this);
    filterAmountDial.setTextValueSuffix("%");

    addAndMakeVisible(amountLabel);
    amountLabel.setText("Env Amount", juce::dontSendNotification);
    amountLabel.setJustificationType(juce::Justification::centred);
    amountLabel.attachToComponent(&filterAmountDial, false);

    // init filter amp parameter names struct
    SympleADSRParameterNames ampParameters;
    ampParameters.attack = "FILTER_ATTACK";
    ampParameters.decay = "FILTER_DECAY";
    ampParameters.sustain = "FILTER_SUSTAIN";
    ampParameters.release = "FILTER_RELEASE";
    amplifier.setParameters(ampParameters);

    addAndMakeVisible(amplifier);

    addAndMakeVisible(ampLabel);
    ampLabel.setText("Amplifier", juce::dontSendNotification);
    ampLabel.setJustificationType(juce::Justification::centred);
    ampLabel.attachToComponent(&amplifier, false);
}

SympleFilterComponent::~SympleFilterComponent()
{
    filterCutoffValue.reset();
    filterResValue.reset();
    filterAmountValue.reset();
}

void SympleFilterComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
}

void SympleFilterComponent::resized()
{
    juce::Rectangle<int> area(0,0, getWidth(), getHeight());
    auto filterWidth = getWidth() / 3;
    auto margin = 5;
    auto labelMargin = 30;
    auto ampHeight = getHeight() / 2;

    auto ampArea = area.removeFromBottom(ampHeight).reduced(margin);
    amplifier.setBounds(ampArea.getX(), ampArea.getY() + labelMargin, ampArea.getWidth(), 100);

    auto cutoffArea = area.removeFromLeft(filterWidth).reduced(margin);
    auto resArea = area.removeFromLeft(filterWidth).reduced(margin);
    auto amountArea = area.removeFromLeft(filterWidth).reduced(margin);

    filterCutoffDial.setBounds(cutoffArea.getX(), cutoffArea.getY() + labelMargin, cutoffArea.getWidth(), 100);
    filterResDial.setBounds(resArea.getX(), resArea.getY() + labelMargin, resArea.getWidth(), 100);
    filterAmountDial.setBounds(amountArea.getX(), amountArea.getY() + labelMargin, amountArea.getWidth(), 100);
}

void SympleFilterComponent::setParameters(SympleFilterParameterNames& params) {
    filterCutoffValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getTree(), params.cutoff, filterCutoffDial);
    filterResValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getTree(), params.resonance, filterResDial);
    filterAmountValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getTree(), params.amount, filterAmountDial);
}
