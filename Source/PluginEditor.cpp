/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.
    
    Used code from https://github.com/TheAudioProgrammer/juceIIRFilter

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SympleSynthAudioProcessorEditor::SympleSynthAudioProcessorEditor (SympleSynthAudioProcessor& p)
    : AudioProcessorEditor (&p),
      keyboardComponent (p.getKeyboardState(), juce::MidiKeyboardComponent::horizontalKeyboard),
      audioProcessor(p),
      amplifier(p),
      filterAmplifier(p)
{
    // init amp parameter names struct
    SympleADSRParameterNames ampParameters;
    ampParameters.attack = "AMP_ATTACK";
    ampParameters.decay = "AMP_DECAY";
    ampParameters.sustain = "AMP_SUSTAIN";
    ampParameters.release = "AMP_RELEASE";
    amplifier.setParameters(ampParameters);
    
    // init filter amp parameter names struct
    SympleADSRParameterNames filterAmpParameters;
    filterAmpParameters.attack = "FILTER_ATTACK";
    filterAmpParameters.decay = "FILTER_DECAY";
    filterAmpParameters.sustain = "FILTER_SUSTAIN";
    filterAmpParameters.release = "FILTER_RELEASE";
    filterAmplifier.setParameters(filterAmpParameters);

    setSize(1200, 800);
    addAndMakeVisible(keyboardComponent);
    addAndMakeVisible(amplifier);
    addAndMakeVisible(filterAmplifier);

    //  Add Filter Dials
    filterCutoffDial.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    filterCutoffDial.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    filterCutoffDial.setPopupDisplayEnabled(true, true, this);
    addAndMakeVisible(&filterCutoffDial);

    filterResDial.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    filterResDial.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    filterResDial.setPopupDisplayEnabled(true, true, this);
    addAndMakeVisible(&filterResDial);

    filterCutoffValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getTree(), "CUTOFF", filterCutoffDial);
    filterResValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getTree(), "RESONANCE", filterResDial);
    filterCutoffDial.setSkewFactorFromMidPoint(1000.0f);


    masterGainSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    masterGainSlider.setRange(-60.0f, 0.0f, 0.01f);
    masterGainSlider.setValue(-20.0f);
    masterGainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 20);
    masterGainSlider.addListener(this); // Make sure this is set or the slider won't work.
    addAndMakeVisible(masterGainSlider);
}

SympleSynthAudioProcessorEditor::~SympleSynthAudioProcessorEditor()
{
}

//==============================================================================
void SympleSynthAudioProcessorEditor::paint (juce::Graphics& g)
{
    juce::Rectangle<int> titleArea(0, 10, getWidth(), 20);
    juce::Rectangle <float> filterArea(25, 25, 150, 150);
    
    g.setFont (15.0f);
    g.fillAll(juce::Colours::black);
    g.setColour(juce::Colours::white);

    // Draw Filter Dials
    g.drawText("Filter", titleArea, juce::Justification::centredTop);
    g.drawText("Cutoff", 46, 70, 50, 25, juce::Justification::centredLeft);
    g.drawText("Resonance", 107, 70, 70, 25, juce::Justification::centredLeft);

    // draw amplifier label
    g.drawFittedText ("Amplifier", getWidth() - amplifier.getWidth() - 20,
                      20, amplifier.getWidth(), 40, juce::Justification::centred, 1);

    if (!keyboardComponent.hasKeyboardFocus (true) &&
        keyboardComponent.isVisible())
    {
        keyboardComponent.grabKeyboardFocus();
    }
}

void SympleSynthAudioProcessorEditor::resized()
{
    amplifier.setBounds(getWidth() - amplifier.getWidth() - 20, 70, amplifier.getWidth(), amplifier.getHeight());
    filterAmplifier.setBounds(-20, 160, filterAmplifier.getWidth(), filterAmplifier.getHeight());
    keyboardComponent.setBounds(0, 700, getWidth(), 100);
    masterGainSlider.setBounds(1100, 400, 40, 100);
    filterCutoffDial.setBounds(30, 90, 70, 70);
    filterResDial.setBounds(100, 90, 70, 70);
}

void SympleSynthAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    // If the passed in slider is masterGainSlider (if the addresses are equal
    if (slider == &masterGainSlider)
    {
        audioProcessor.masterGain = masterGainSlider.getValue();
    }
}
