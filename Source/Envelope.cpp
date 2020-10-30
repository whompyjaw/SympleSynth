/*
  ==============================================================================

    SympleADSRComponent.cpp
    Created: 15 Oct 2020 5:40:08pm
    Author:  woz

  ==============================================================================
*/

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Envelope.h"

SympleADSRComponent::SympleADSRComponent(SympleSynthAudioProcessor& p) : audioProcessor(p)
{
    setSize (400, 100);
    
    // Draw Attack Slider
    attack.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    attack.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    attack.setPopupDisplayEnabled(true, true, this);
    attack.setTextValueSuffix (" sec");
    addAndMakeVisible (&attack);
    
    // Draw Decay Slider
    decay.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    decay.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    decay.setPopupDisplayEnabled(true, true, this);
    decay.setTextValueSuffix (" sec");
    addAndMakeVisible (&decay);

    // Draw Sustain Slider
    sustain.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    sustain.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    sustain.setPopupDisplayEnabled(true, true, this);
    sustain.setTextValueSuffix ("%");
    addAndMakeVisible (&sustain);

    // Draw Release Slider
    release.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    release.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    release.setPopupDisplayEnabled(true, true, this);
    release.setTextValueSuffix (" sec");
    addAndMakeVisible (&release);
}

SympleADSRComponent::~SympleADSRComponent()
{
    attackValue.reset();
    decayValue.reset();
    sustainValue.reset();
    releaseValue.reset();
}

void SympleADSRComponent::paint(juce::Graphics& g)
{
    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("A", 80, 60, 10, 15, juce::Justification::centred, 1);
    g.drawFittedText ("D", 155, 60, 10, 15, juce::Justification::centred, 1);
    g.drawFittedText ("S", 230, 60, 10, 15, juce::Justification::centred, 1);
    g.drawFittedText ("R", 305, 60, 10, 15, juce::Justification::centred, 1);
}

void SympleADSRComponent::resized()
{
    auto area = getLocalBounds();
    auto knobRadius = 70;
    auto knobMargin = 5;

    // attack.setBounds(area.removeFromLeft(knobRadius).reduced(knobMargin));
    attack.setBounds (50, 0, knobRadius, knobRadius);
    decay.setBounds (125, 0, knobRadius, knobRadius);
    sustain.setBounds (200, 0, knobRadius, knobRadius);
    release.setBounds (275, 0, knobRadius, knobRadius);
}

void SympleADSRComponent::setParameters(SympleADSRParameterNames& params) {
    attackValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getTree(), params.attack, attack);
    decayValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getTree(), params.decay, decay);
    sustainValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getTree(), params.sustain, sustain);
    releaseValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getTree(), params.release, release);
}
