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

    setSize(1200, 800);
    addAndMakeVisible(filter);
    addAndMakeVisible(keyboardComponent);
    addAndMakeVisible(amplifier);
    addAndMakeVisible(filterAmplifier);

    



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
    g.fillAll(juce::Colours::black);
    

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
}

void SympleSynthAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    // If the passed in slider is masterGainSlider (if the addresses are equal
    if (slider == &masterGainSlider)
    {
        audioProcessor.masterGain = masterGainSlider.getValue();
    }
}
