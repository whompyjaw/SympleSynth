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
    setSize(400, 400);
    addAndMakeVisible(&osc1OctaveDial);
    osc1OctaveDial.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    osc1OctaveDial.setTextBoxStyle(juce::Slider::TextBoxAbove, true, 30, 30);
    osc1OctaveDial.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::black);
    osc1OctaveValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getTree(), "OSC_1_OCTAVE", osc1OctaveDial);
    
//    setLayoutParams();
    
}

OscInterface::~OscInterface()
{
    
}

void OscInterface::paint(juce::Graphics &g)
{
    juce::Rectangle <int> oscTitleArea(50, 125, 70, 20);
    juce::Rectangle <float> oscSection(25, 150, 300, 400);
    
    g.setColour(juce::Colours::white);
    g.drawFittedText ("Oscillator", oscTitleArea, juce::Justification::centred, 1);
    g.drawRect(oscTitleArea);
    
    g.setColour(juce::Colours::yellow);
    g.drawRoundedRectangle(oscSection, 5.0f, 2.0f);
    
}

void OscInterface::resized()
{
    int dialSpacing = 75;
    osc1OctaveDial.setBounds(0 + dialSpacing, 160, 100, 100);
    
}

//void OscInterface::setLayoutParams()
//{
//    juce::NormalisableRange<float> oscillatorOctaveParams (-2, 2, 1);
//    audioProcessor.parameters.push_back(std::make_unique<juce::AudioParameterFloat>("OSC_1_OCTAVE", "Octave 1", oscillatorOctaveParams, 0, "Octave"));
//}

