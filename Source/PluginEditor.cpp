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
      oscUI(p),
      filter(p),
      amplifier(p)
{
    // Add Title Label
    addAndMakeVisible(titleLabel);
    titleLabel.setText("SYMPLESYNTH 1.0", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(36.0, juce::Font::bold));
    titleLabel.setJustificationType(juce::Justification::centred);

    // init filter parameter names struct
    SympleFilterParameterNames filterParameters;
    filterParameters.cutoff = "CUTOFF";
    filterParameters.resonance = "RESONANCE";
    filterParameters.amount = "AMOUNT";
    filter.setParameters(filterParameters);

    // init gain parameter names struct
    MasterAmpParameterNames ampParameters;
    ampParameters.gain = "MASTER_GAIN";
    amplifier.setParameters(ampParameters);

    // Add Components
    addAndMakeVisible(keyboardComponent);
    addAndMakeVisible(filter);
    addAndMakeVisible(amplifier);
    addAndMakeVisible(oscUI);

    // Add Labels
    addAndMakeVisible(osc1Label);
    osc1Label.setText("OSCILLATOR 1", juce::dontSendNotification);
    osc1Label.setFont(juce::Font(18.0, juce::Font::bold));
    osc1Label.setJustificationType(juce::Justification::centred);
    osc1Label.attachToComponent(&oscUI, false);

    addAndMakeVisible(filter1Label);
    filter1Label.setText("FILTER 1", juce::dontSendNotification);
    filter1Label.setFont(juce::Font(18.0, juce::Font::bold));
    filter1Label.setJustificationType(juce::Justification::centred);
    filter1Label.attachToComponent(&filter, false);

    addAndMakeVisible(ampLabel);
    ampLabel.setText("AMPLIFIER", juce::dontSendNotification);
    ampLabel.setFont(juce::Font(18.0, juce::Font::bold));
    ampLabel.setJustificationType(juce::Justification::centred);
    ampLabel.attachToComponent(&amplifier, false);

    setSize (1200, 800); 
    
}

SympleSynthAudioProcessorEditor::~SympleSynthAudioProcessorEditor()
{
}

//==============================================================================
void SympleSynthAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
  
    if (!keyboardComponent.hasKeyboardFocus (true) &&
        keyboardComponent.isVisible())
    {
        keyboardComponent.grabKeyboardFocus();
    }
}

void SympleSynthAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();
    auto componentWidth = getWidth() / 3;
    auto margin = 30;
    auto labelHeight = titleLabel.getHeight() + 2 * margin;

    // Set Keyboard & Title Bounds
    keyboardComponent.setBounds(area.removeFromBottom(70));
    titleLabel.setBounds(area.removeFromTop(labelHeight));

    // Seperate component area into 3 columns
    auto oscArea = area.removeFromLeft(componentWidth);
    auto filterArea = area.removeFromLeft(componentWidth);
    auto ampArea = area.removeFromLeft(componentWidth);
    auto componentHeight = oscArea.getHeight() / 2;

    // Set Component Bounds
    oscUI.setBounds(oscArea.removeFromTop(componentHeight).reduced(margin));
    filter.setBounds(filterArea.removeFromTop(componentHeight).reduced(margin));
    amplifier.setBounds(ampArea.removeFromTop(componentHeight).reduced(margin));
 }

void SympleSynthAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
}
