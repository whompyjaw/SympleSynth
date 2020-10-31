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
    setSize (400, 400);
    
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
    filterAmountDial.setTextValueSuffix("%");

    addAndMakeVisible(amountLabel);
    amountLabel.setText("Env Amount", juce::dontSendNotification);
    amountLabel.setJustificationType(juce::Justification::centred);
    amountLabel.attachToComponent(&filterAmountDial, false);

    // init amp parameter names struct
    SympleADSRParameterNames ampParameters;
    ampParameters.attack = "FILTER_ATTACK";
    ampParameters.decay = "FILTER_DECAY";
    ampParameters.sustain = "FILTER_SUSTAIN";
    ampParameters.release = "FILTER_RELEASE";
    amplifier.setParameters(ampParameters);

    //  Add Amp & Label
    addAndMakeVisible(amplifier);

    addAndMakeVisible(ampLabel);
    ampLabel.setText("Filter Envelope", juce::dontSendNotification);
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
}

void SympleFilterComponent::resized()
{
    juce::Rectangle<int> area(0,0, getWidth(), getHeight());
    auto filterWidth = getWidth() / 3;
    auto margin = 5;
    auto labelMargin = amountLabel.getHeight();

    // Set Amp Bounds
    auto ampArea = area.removeFromBottom(getHeight() / 2).reduced(margin);
    auto ampHeight = ampArea.getHeight() - labelMargin;
    auto ampY = ampArea.getY() + labelMargin;

    amplifier.setBounds(ampArea.getX(), ampY, ampArea.getWidth(), ampHeight);

    // Set Filter Dial Bounds
    auto cutoffArea = area.removeFromLeft(filterWidth).reduced(margin);
    auto resArea = area.removeFromLeft(filterWidth).reduced(margin);
    auto amountArea = area.removeFromLeft(filterWidth).reduced(margin);

    filterCutoffDial.setBounds(cutoffArea.getX(), cutoffArea.getY() + labelMargin, cutoffArea.getWidth(), ampHeight);
    filterResDial.setBounds(resArea.getX(), resArea.getY() + labelMargin, resArea.getWidth(), ampHeight);
    filterAmountDial.setBounds(amountArea.getX(), amountArea.getY() + labelMargin, amountArea.getWidth(), ampHeight);
}

void SympleFilterComponent::setParameters(SympleFilterParameterNames& params) {
    filterCutoffValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getTree(), params.cutoff, filterCutoffDial);
    filterResValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getTree(), params.resonance, filterResDial);
    filterAmountValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getTree(), params.amount, filterAmountDial);
}
