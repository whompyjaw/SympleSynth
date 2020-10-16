/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SympleSynthAudioProcessorEditor::SympleSynthAudioProcessorEditor (SympleSynthAudioProcessor& p)
    : AudioProcessorEditor (&p),
      keyboardComponent (p.getKeyboardState(), juce::MidiKeyboardComponent::horizontalKeyboard),
      audioProcessor (p),
      amplifier(p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

    addAndMakeVisible(keyboardComponent);
    addAndMakeVisible(amplifier);

    masterGainSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    masterGainSlider.setRange(0.0f, 1.0f, 0.01f);
    masterGainSlider.setValue(0.5f);
    masterGainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 20);

    addAndMakeVisible(masterGainSlider);
    setSize (1200, 800);
    //startTimer(400);
}

SympleSynthAudioProcessorEditor::~SympleSynthAudioProcessorEditor()
{
}

//==============================================================================
void SympleSynthAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(juce::Colours::black);

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
    
    // draw amplifier label
    g.drawFittedText ("Amplifier", getWidth() - amplifier.getWidth() - 20,
                      0, amplifier.getWidth(), 20, juce::Justification::centred, 1);

    if (!keyboardComponent.hasKeyboardFocus (true) &&
        keyboardComponent.isVisible())
    {
        keyboardComponent.grabKeyboardFocus();
    }

}

void SympleSynthAudioProcessorEditor::resized()
{
    amplifier.setBounds(getWidth() - amplifier.getWidth() - 20, 30, amplifier.getWidth(), amplifier.getHeight());
    keyboardComponent.setBounds(0, 700, getWidth(), 100);
    masterGainSlider.setBounds(1100, 400, 40, 100);
}
