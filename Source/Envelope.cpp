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
    setSize (400, 300);
    
    // Draw Attack Slider & Label
    attack.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    attack.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    attack.setPopupDisplayEnabled(true, true, this);
    attack.setTextValueSuffix (" sec");
    addAndMakeVisible (&attack);

    addAndMakeVisible(attackLabel);
    attackLabel.setText("Attack", juce::dontSendNotification);
    attackLabel.setJustificationType(juce::Justification::centred);
    attackLabel.attachToComponent(&attack, false);
    
    // Draw Decay Slider
    decay.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    decay.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    decay.setPopupDisplayEnabled(true, true, this);
    decay.setTextValueSuffix (" sec");
    addAndMakeVisible (&decay);

    addAndMakeVisible(decayLabel);
    decayLabel.setText("Decay", juce::dontSendNotification);
    decayLabel.setJustificationType(juce::Justification::centred);
    decayLabel.attachToComponent(&decay, false);

    // Draw Sustain Slider
    sustain.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    sustain.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    sustain.setPopupDisplayEnabled(true, true, this);
    sustain.setTextValueSuffix ("%");
    addAndMakeVisible (&sustain);

    addAndMakeVisible(sustainLabel);
    sustainLabel.setText("Sustain", juce::dontSendNotification);
    sustainLabel.setJustificationType(juce::Justification::centred);
    sustainLabel.attachToComponent(&sustain, false);

    // Draw Release Slider
    release.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    release.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    release.setPopupDisplayEnabled(true, true, this);
    release.setTextValueSuffix (" sec");
    addAndMakeVisible (&release);

    addAndMakeVisible(releaseLabel);
    releaseLabel.setText("Release", juce::dontSendNotification);
    releaseLabel.setJustificationType(juce::Justification::centred);
    releaseLabel.attachToComponent(&release, false);
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
    //g.fillAll(juce::Colours::midnightblue);
}

void SympleADSRComponent::resized()
{
    juce::Rectangle<int> area(0, 0, getWidth(), getHeight());
    auto knobWidth = getWidth() / 4;
    auto margin = 5;
    auto labelMargin = attackLabel.getHeight();

    auto attackArea = area.removeFromLeft(knobWidth).reduced(margin);
    auto decayArea = area.removeFromLeft(knobWidth).reduced(margin);
    auto sustainArea = area.removeFromLeft(knobWidth).reduced(margin);
    auto releaseArea = area.removeFromLeft(knobWidth).reduced(margin);

    auto knobHeight = attackArea.getHeight() - labelMargin;
    auto knobY = attackArea.getY() + labelMargin;

    attack.setBounds(attackArea.getX(), knobY, attackArea.getWidth(), knobHeight);
    decay.setBounds(decayArea.getX(), knobY, decayArea.getWidth(), knobHeight);
    sustain.setBounds(sustainArea.getX(), knobY, sustainArea.getWidth(), knobHeight);
    release.setBounds(releaseArea.getX(), knobY, releaseArea.getWidth(), knobHeight);
}

void SympleADSRComponent::setParameters(SympleADSRParameterNames& params) {
    attackValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getTree(), params.attack, attack);
    decayValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getTree(), params.decay, decay);
    sustainValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getTree(), params.sustain, sustain);
    releaseValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getTree(), params.release, release);
}
