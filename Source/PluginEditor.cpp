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

    // Add Labels
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

    addAndMakeVisible(keyboardComponent);
    addAndMakeVisible(filter);
    addAndMakeVisible(amplifier);

    
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
    
    
    // Master Slider
    addAndMakeVisible(masterGainSlider);
    masterGainSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    masterGainSlider.setRange(-60.0f, 0.0f, 0.1f);
    masterGainSlider.setValue(-20.0f);
    masterGainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 30);
    masterGainSlider.addListener(this); // Make sure this is set or the slider won't work.
    
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

    keyboardComponent.setBounds(area.removeFromBottom(70));

    auto oscArea = area.removeFromLeft(componentWidth);
    auto filterArea = area.removeFromLeft(componentWidth);
    auto ampArea = area.removeFromLeft(componentWidth);
    auto labelHeight = 75;
    auto componentHeight = oscArea.getHeight() / 2;

    titleLabel.setBounds(ampArea.removeFromTop(labelHeight).reduced(margin));
    amplifier.setBounds(ampArea.removeFromTop(componentHeight - labelHeight).reduced(margin));
    filter.setBounds(filterArea.removeFromTop(componentHeight).reduced(margin));

    masterGainSlider.setBounds(1000, 400, 40, 150);
    
    
    // Oscillator section
    int dialSpacing = 75;
    osc1WaveTypeDial.setBounds(0, 160, 100, 100);
    osc1OctaveDial.setBounds(osc1WaveTypeDial.getX() + dialSpacing, 160, 100, 100);
    osc1SemitoneDial.setBounds(osc1OctaveDial.getX() + dialSpacing, 160, 100, 100);
    osc1FineTuneDial.setBounds(osc1SemitoneDial.getX() + dialSpacing, 160, 100, 100);
}

void SympleSynthAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    // If the passed in slider is masterGainSlider (if the addresses are equal
    if (slider == &masterGainSlider)
    {
        audioProcessor.masterGain = masterGainSlider.getValue();
    }
}
