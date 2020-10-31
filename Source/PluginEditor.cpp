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

    // osc wave type dial   
    addAndMakeVisible(&osc1WaveTypeDial);
    osc1WaveTypeDial.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    osc1WaveTypeDial.setTextBoxStyle(juce::Slider::TextBoxAbove, true, 30, 30);
    osc1WaveTypeDial.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::white);
    osc1WaveTypeDial.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::yellow);
    osc1WaveTypeValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getTree(), "OSC_1_WAVE_TYPE", osc1WaveTypeDial);
        
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
    int dialSpacing = 75;
    osc1WaveTypeDial.setBounds(0, 160, 100, 100);
    osc1OctaveDial.setBounds(osc1WaveTypeDial.getX() + dialSpacing, 160, 100, 100);
    osc1SemitoneDial.setBounds(osc1OctaveDial.getX() + dialSpacing, 160, 100, 100);
    osc1FineTuneDial.setBounds(osc1SemitoneDial.getX() + dialSpacing, 160, 100, 100);
}

void SympleSynthAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
}
