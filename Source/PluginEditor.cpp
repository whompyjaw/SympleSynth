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

    // Add Labels
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
        
    addAndMakeVisible(oscUI);
    
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
    auto margin = 20;
    auto labelHeight = 75;

    // Set Keyboard & Title Bounds
    keyboardComponent.setBounds(area.removeFromBottom(70));
    titleLabel.setBounds(area.removeFromTop(labelHeight).reduced(margin));

    // Seperate component area into 3 columns
    auto oscArea = area.removeFromLeft(componentWidth);
    auto filterArea = area.removeFromLeft(componentWidth);
    auto ampArea = area.removeFromLeft(componentWidth);
    auto componentHeight = oscArea.getHeight() / 2;

    // Set Component Bounds
    amplifier.setBounds(ampArea.removeFromTop(componentHeight).reduced(margin));
    filter.setBounds(filterArea.removeFromTop(componentHeight).reduced(margin));    
    
    // Oscillator section
    oscUI.setBounds(0, 0, oscUI.getWidth(), oscUI.getHeight());
}

void SympleSynthAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
}
