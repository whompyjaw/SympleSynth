/*
  ==============================================================================

    SympleFilterComponent.cpp
    Created: 15 Oct 2020 5:40:08pm
    Author:  woz

  ==============================================================================
*/

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "SympleFilterComponent.h"

SympleFilterComponent::SympleFilterComponent(SympleSynthAudioProcessor& p) : audioProcessor(p)
{
    setSize (400, 300);
    
    //  Add Filter Dials
    filterCutoffDial.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    filterCutoffDial.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    filterCutoffDial.setPopupDisplayEnabled(true, true, this);
    filterCutoffDial.setTextValueSuffix(" hz");
    addAndMakeVisible(&filterCutoffDial);

    filterResDial.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    filterResDial.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    filterResDial.setPopupDisplayEnabled(true, true, this);
    filterResDial.setTextValueSuffix("%");
    addAndMakeVisible(&filterResDial);

    filterAmountDial.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    filterAmountDial.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    filterAmountDial.setPopupDisplayEnabled(true, true, this);
    filterAmountDial.setTextValueSuffix("%");
    addAndMakeVisible(&filterAmountDial);
}

SympleFilterComponent::~SympleFilterComponent()
{
    filterCutoffValue.reset();
    filterResValue.reset();
    filterAmountValue.reset();
}

void SympleFilterComponent::paint(juce::Graphics& g)
{
    juce::Rectangle<int> titleArea(0, 10, getWidth(), 20);

    g.setFont(15.0f);
    //g.fillAll(juce::Colours::black);
    g.setColour(juce::Colours::white);

    // Draw Filter Dials
    g.drawText("Filter", titleArea, juce::Justification::centredTop);
    g.drawText("Cutoff", 46, 70, 50, 25, juce::Justification::centredLeft);
    g.drawText("Resonance", 107, 70, 70, 25, juce::Justification::centredLeft);
    g.drawText("Envelope Amount", 187, 70, 120, 25, juce::Justification::centredLeft);
}

void SympleFilterComponent::resized()
{
    int knobRadius = 70;
    filterCutoffDial.setBounds(30, 90, knobRadius, knobRadius);
    filterResDial.setBounds(100, 90, knobRadius, knobRadius);
    filterAmountDial.setBounds(200, 90, knobRadius, knobRadius);
}

void SympleFilterComponent::setParameters(SympleFilterParameterNames& params) {
    filterCutoffValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getTree(), params.cutoff, filterCutoffDial);
    filterResValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getTree(), params.resonance, filterResDial);
    filterAmountValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getTree(), params.amount, filterAmountDial);
}
