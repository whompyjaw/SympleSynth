/*
  ==============================================================================

    SympleADSRComponent.cpp
    Created: 15 Oct 2020 5:40:08pm
    Author:  woz

  ==============================================================================
*/

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "SympleADSRComponent.h"

SympleADSRComponent::SympleADSRComponent(SympleSynthAudioProcessor& p) : audioProcessor(p)
{
    setSize (350, 100);
    
    // attack
    attack.setSliderStyle (juce::Slider::LinearBarVertical);
    attack.setTextBoxStyle (juce::Slider::TextBoxAbove, false, 90, 30);
    attack.setPopupDisplayEnabled (true, false, this);
    attack.setTextValueSuffix (" sec");
    addAndMakeVisible (&attack);
    
    decay.setSliderStyle (juce::Slider::LinearBarVertical);
    decay.setTextBoxStyle (juce::Slider::TextBoxAbove, false, 90, 30);
    decay.setPopupDisplayEnabled (false, false, this);
    decay.setTextValueSuffix (" sec");
    addAndMakeVisible (&decay);

    sustain.setSliderStyle (juce::Slider::LinearBarVertical);
    sustain.setTextBoxStyle (juce::Slider::TextBoxAbove, false, 90, 30);
    sustain.setPopupDisplayEnabled (false, false, this);
    sustain.setTextValueSuffix ("%");
    addAndMakeVisible (&sustain);

    release.setSliderStyle (juce::Slider::LinearBarVertical);
    release.setTextBoxStyle (juce::Slider::TextBoxAbove, false, 90, 30);
    release.setPopupDisplayEnabled (false, false, this);
    release.setTextValueSuffix (" sec");
    addAndMakeVisible (&release);
    
    attackValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getTree(), "AMP_ATTACK", attack);
    decayValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getTree(), "AMP_DECAY", decay);
    sustainValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getTree(), "AMP_SUSTAIN", sustain);
    releaseValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getTree(), "AMP_RELEASE", release);
    
//    attack.setSkewFactorFromMidPoint(0.35);
//    decay.setSkewFactorFromMidPoint(0.35);
//    release.setSkewFactorFromMidPoint(0.35);
}

SympleADSRComponent::~SympleADSRComponent()
{
}

void SympleADSRComponent::paint(juce::Graphics& g)
{
    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("A", 70, 55, 10, 15, juce::Justification::centred, 1);
    g.drawFittedText ("D", 145, 55, 10, 15, juce::Justification::centred, 1);
    g.drawFittedText ("S", 220, 55, 10, 15, juce::Justification::centred, 1);
    g.drawFittedText ("R", 295, 55, 10, 15, juce::Justification::centred, 1);
}

void SympleADSRComponent::resized()
{
    attack.setBounds (50, 0, 50, 50);
    decay.setBounds (125, 0, 50, 50);
    sustain.setBounds (200, 0, 50, 50);
    release.setBounds (275, 0, 50, 50);
}
