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
      amplifier(p),
      filterAmplifier(p)
{
    // init filter parameter names struct
    SympleFilterParameterNames filterParameters;
    filterParameters.cutoff = "CUTOFF";
    filterParameters.resonance = "RESONANCE";
    filterParameters.amount = "AMOUNT";
    filter.setParameters(filterParameters);

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

    addAndMakeVisible(filter);
    addAndMakeVisible(keyboardComponent);
    addAndMakeVisible(amplifier);
    addAndMakeVisible(filterAmplifier);

    
    // Oscillator
    addAndMakeVisible(&osc1OctaveDial);
    osc1OctaveDial.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    osc1OctaveDial.setTextBoxStyle(juce::Slider::TextBoxAbove, true, 70, 30);
    osc1OctaveDial.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::black);
    osc1OctaveValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getTree(), "OSC_1_OCTAVE", osc1OctaveDial);
    
    // Semitone dial
    addAndMakeVisible(&osc1SemitoneDial);
    osc1SemitoneDial.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    osc1SemitoneDial.setTextBoxStyle(juce::Slider::TextBoxAbove, true, 100, 30);
    osc1SemitoneDial.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::black);
    osc1SemitoneValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getTree(), "OSC_1_SEMITONE", osc1SemitoneDial);

    // Master Slider
    addAndMakeVisible(masterGainSlider);
    masterGainSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    masterGainSlider.setRange(-60.0f, 0.0f, 0.1f);
    masterGainSlider.setValue(-20.0f);
    masterGainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 30);
    masterGainSlider.addListener(this); // Make sure this is set or the slider won't work.
    
    setSize (1100, 700);
    
}

SympleSynthAudioProcessorEditor::~SympleSynthAudioProcessorEditor()
{
}

//==============================================================================
void SympleSynthAudioProcessorEditor::paint (juce::Graphics& g)
{
    juce::Rectangle<int> ampTitleArea(getWidth() - amplifier.getWidth() + 150, 120, 70, 20);
    juce::Rectangle <int> oscTitleArea(50, 125, 70, 20);
    juce::Rectangle <float> filterArea(400, 150, 300, 200);
    juce::Rectangle <float> oscSection(25, 150, 300, 400);
    juce::Rectangle <float> ampSection(700, 150, 375, 100);
    
    g.setFont (15.0f);
    g.fillAll(juce::Colours::black);
    

    // draw amplifier label
    g.setColour(juce::Colours::white);
    g.drawFittedText ("Amplifier", ampTitleArea, juce::Justification::centred, 1);
    g.drawRect(ampTitleArea);
    g.drawFittedText ("Oscillator", oscTitleArea, juce::Justification::centred, 1);
    g.drawRect(oscTitleArea);


    g.setColour(juce::Colours::yellow);
    g.drawRoundedRectangle(filterArea, 5.0f, 2.0f);
    g.drawRoundedRectangle(oscSection, 5.0f, 2.0f);
    g.drawRoundedRectangle(ampSection, 5.0f, 2.0f);
    
    if (!keyboardComponent.hasKeyboardFocus (true) &&
        keyboardComponent.isVisible())
    {
        keyboardComponent.grabKeyboardFocus();
    }
}

void SympleSynthAudioProcessorEditor::resized()
{
    amplifier.setBounds(getWidth() - amplifier.getWidth(), 150, amplifier.getWidth(), amplifier.getHeight());
    
    keyboardComponent.setBounds(0, 630, getWidth(), 70);
    
    masterGainSlider.setBounds(1000, 400, 40, 150);
    
    filterAmplifier.setBounds(350, 275, filterAmplifier.getWidth(), filterAmplifier.getHeight());
    
    // Oscillator section
    osc1OctaveDial.setBounds(40, 310, 100, 100);
    osc1SemitoneDial.setBounds(130, 310, 100, 100);
    
}

void SympleSynthAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    // If the passed in slider is masterGainSlider (if the addresses are equal
    if (slider == &masterGainSlider)
    {
        audioProcessor.masterGain = masterGainSlider.getValue();
    }
}
