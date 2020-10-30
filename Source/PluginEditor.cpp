/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.
    
    Used code from https://github.com/TheAudioProgrammer/juceIIRFilter

  ==============================================================================
*/

//#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SympleSynthAudioProcessorEditor::SympleSynthAudioProcessorEditor (SympleSynthAudioProcessor& p)
    : AudioProcessorEditor (&p),
      keyboardComponent (p.getKeyboardState(), juce::MidiKeyboardComponent::horizontalKeyboard),
      audioProcessor(p),
      filter(p),
      amplifier(p),
      oscInterface(p)
     // filterAmplifier(p)
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
  
    
    /*
    // init filter amp parameter names struct
    SympleADSRParameterNames filterAmpParameters;
    filterAmpParameters.attack = "FILTER_ATTACK";
    filterAmpParameters.decay = "FILTER_DECAY";
    filterAmpParameters.sustain = "FILTER_SUSTAIN";
    filterAmpParameters.release = "FILTER_RELEASE";
    filterAmplifier.setParameters(filterAmpParameters);
*/

    addAndMakeVisible(filter);
    addAndMakeVisible(oscInterface);
    addAndMakeVisible(keyboardComponent);
    addAndMakeVisible(amplifier);
    //addAndMakeVisible(filterAmplifier);
    setSize (1200, 600);
    
}

SympleSynthAudioProcessorEditor::~SympleSynthAudioProcessorEditor()
{
}

//==============================================================================
void SympleSynthAudioProcessorEditor::paint (juce::Graphics& g)
{
    ///*
    juce::Rectangle<int> ampTitleArea(getWidth() - amplifier.getWidth() + 150, 120, 70, 20);
    //juce::Rectangle <float> filterArea(400, 150, 300, 200);
    juce::Rectangle <float> ampSection(700, 150, 375, 100);
    //*/
    
    g.setFont (15.0f);
    g.fillAll(juce::Colours::black);
    

    // draw amplifier label
    g.setColour(juce::Colours::white);
    g.drawFittedText ("Amplifier", ampTitleArea, juce::Justification::centred, 1);
    g.drawRect(ampTitleArea);


    g.setColour(juce::Colours::yellow);
    //g.drawRoundedRectangle(filterArea, 5.0f, 2.0f);
    g.drawRoundedRectangle(ampSection, 5.0f, 2.0f);
    
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
    amplifier.setBounds(area.removeFromLeft(componentWidth));
    filter.setBounds(area.removeFromLeft(componentWidth).reduced(margin));
    oscInterface.setBounds(0, 50, oscInterface.getWidth(), oscInterface.getHeight());
    masterGainSlider.setBounds(1000, 400, 40, 150);
}

void SympleSynthAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    // If the passed in slider is masterGainSlider (if the addresses are equal
    if (slider == &masterGainSlider)
    {
        audioProcessor.masterGain = masterGainSlider.getValue();
    }
}
