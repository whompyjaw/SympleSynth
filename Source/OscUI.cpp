/*
  ==============================================================================

    OscUI.cpp
    Created: 28 Oct 2020 8:52:13pm
    Author:  Whompy Jaw

  ==============================================================================
*/

#include "OscUI.h"
//==============================================================================
OscUI::OscUI (SympleSynthAudioProcessor& p)
    : audioProcessor(p)
{
    // octave dial
    addAndMakeVisible(&osc1OctaveDial);
    osc1OctaveDial.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    osc1OctaveDial.setTextBoxStyle(juce::Slider::TextBoxAbove, true, 30, 30);
    osc1OctaveDial.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::black);
    osc1OctaveValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getTree(), "OSC_1_OCTAVE", osc1OctaveDial);
    
    // semitone dial
    addAndMakeVisible(&osc1SemitoneDial);
    osc1SemitoneDial.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    osc1SemitoneDial.setTextBoxStyle(juce::Slider::TextBoxAbove, true, 30, 30);
    osc1SemitoneDial.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::black);
    osc1SemitoneValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getTree(), "OSC_1_SEMITONE", osc1SemitoneDial);
    
    // fine tune dial
    addAndMakeVisible(&osc1FineTuneDial);
    osc1FineTuneDial.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    osc1FineTuneDial.setTextBoxStyle(juce::Slider::TextBoxAbove, true, 30, 30);
    osc1FineTuneDial.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::black);
    osc1FineTuneValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getTree(), "OSC_1_FINE_TUNE", osc1FineTuneDial);

    // wave type dial
    addAndMakeVisible(&osc1WaveTypeDial);
    osc1WaveTypeDial.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    osc1WaveTypeDial.setTextBoxStyle(juce::Slider::TextBoxAbove, true, 30, 30);
    osc1WaveTypeDial.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::white);
    osc1WaveTypeDial.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::yellow);
    osc1WaveTypeValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getTree(), "OSC_1_WAVE_TYPE", osc1WaveTypeDial);
    
    setSize (400, 400);
    
}

OscUI::~OscUI()
{
}

//==============================================================================
void OscUI::paint (juce::Graphics& g)
{
    juce::Rectangle <int> oscTitleArea(50, 125, 70, 20);
    juce::Rectangle <float> oscSection(0, 0, 400, 400);
    
    g.setFont (15.0f);
    g.fillAll(juce::Colours::black);
    

    // draw amplifier label
    g.setColour(juce::Colours::white);
    g.drawFittedText ("Oscillator", oscTitleArea, juce::Justification::centred, 1);
    g.drawRect(oscTitleArea);


    g.setColour(juce::Colours::yellow);
    //g.drawRoundedRectangle(filterArea, 5.0f, 2.0f);
    g.drawRoundedRectangle(oscSection, 5.0f, 2.0f);

}

void OscUI::resized()
{
    auto area = getLocalBounds();

    auto componentWidth = getWidth() / 3;
    auto margin = 20;

//    keyboardComponent.setBounds(area.removeFromBottom(70));
//    amplifier.setBounds(area.removeFromLeft(componentWidth));
//    filter.setBounds(area.removeFromLeft(componentWidth).reduced(margin));
    
    
    // Oscillator section
    int dialSpacing = 75;
    osc1WaveTypeDial.setBounds(0, 50, 100, 100);
    osc1OctaveDial.setBounds(osc1WaveTypeDial.getX() + dialSpacing, 50, 100, 100);
    osc1SemitoneDial.setBounds(osc1OctaveDial.getX() + dialSpacing, 50, 100, 100);
    osc1FineTuneDial.setBounds(osc1SemitoneDial.getX() + dialSpacing, 50, 100, 100);
}

//void OscUI::setParameters() { 
//    // oscillator 1 defaults
//    juce::NormalisableRange<float> oscillatorOctaveParams (-2, 2, 1);
//    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("OSC_1_OCTAVE", "Octave 1", oscillatorOctaveParams, 0, "Octave"));
//    
//    juce::NormalisableRange<float> oscillatorSemitone (-12, 12, 1);
//    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("OSC_1_SEMITONE", "Semitone 1", oscillatorSemitone, 0, "Semitone"));
//    
//    juce::NormalisableRange<float> oscillatorFineTune (-100, 100, 1);
//    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("OSC_1_FINE_TUNE", "Fine Tune 1", oscillatorFineTune, 0, "Fine Tune"));
//    
//    juce::NormalisableRange<float> oscillatorWaveType (0, 3, 1);
//    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("OSC_1_WAVE_TYPE", "Wave Type 1", oscillatorWaveType, 1, "Wave Type"));
//    return 
//}

