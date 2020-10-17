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
    attack.setRange (0.0, 10.0, 0.001);
    attack.setValue (audioProcessor.getAmpParameters().attack);
    attack.setSkewFactorFromMidPoint(0.35);
    attack.setTextBoxStyle (juce::Slider::TextBoxAbove, false, 90, 30);
    attack.setPopupDisplayEnabled (true, false, this);
    attack.setTextValueSuffix (" sec");
    addAndMakeVisible (&attack);
    attack.addListener (this);
    
    decay.setSliderStyle (juce::Slider::LinearBarVertical);
    decay.setRange (0.0, 10.0, 0.001);
    decay.setValue (audioProcessor.getAmpParameters().decay);
    decay.setSkewFactorFromMidPoint(0.35);
    decay.setTextBoxStyle (juce::Slider::TextBoxAbove, false, 90, 30);
    decay.setPopupDisplayEnabled (false, false, this);
    decay.setTextValueSuffix (" sec");
    addAndMakeVisible (&decay);
    decay.addListener (this);

    sustain.setSliderStyle (juce::Slider::LinearBarVertical);
    sustain.setRange (0.0, 100.0, 0.01);
    sustain.setValue (audioProcessor.getAmpParameters().sustain * 100);
    sustain.setTextBoxStyle (juce::Slider::TextBoxAbove, false, 90, 30);
    sustain.setPopupDisplayEnabled (false, false, this);
    sustain.setTextValueSuffix ("%");
    addAndMakeVisible (&sustain);
    sustain.addListener (this);

    release.setSliderStyle (juce::Slider::LinearBarVertical);
    release.setRange (0.0, 10.0, 0.001);
    release.setSkewFactorFromMidPoint(0.35);
    release.setValue (audioProcessor.getAmpParameters().release);
    release.setTextBoxStyle (juce::Slider::TextBoxAbove, false, 90, 30);
    release.setPopupDisplayEnabled (false, false, this);
    release.setTextValueSuffix (" sec");
    addAndMakeVisible (&release);
    release.addListener (this);
}

SympleADSRComponent::~SympleADSRComponent()
{
}

void SympleADSRComponent::paint(juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

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

void SympleADSRComponent::sliderValueChanged(juce::Slider* slider)
{
    juce::ADSR::Parameters& params = audioProcessor.getAmpParameters();

    params.attack = attack.getValue();
    params.decay = decay.getValue();
    params.sustain = sustain.getValue() / 100.0;
    params.release = release.getValue();
    
    audioProcessor.setAmpParameters(params);
}

