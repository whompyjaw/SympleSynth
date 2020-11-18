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
      osc1(p),
      osc2(p),
      filter(p),
      filter2(p),
      amplifier(p),
      lfo(p)
{
    // Add Title Label
    addAndMakeVisible(titleLabel);
    titleLabel.setText("SYMPLESYNTH 1.0", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(36.0, juce::Font::bold));
    titleLabel.setJustificationType(juce::Justification::centred);

    // init osc1 parameter names struct
    SympleOscParameterNames osc1Parameters;
    osc1Parameters.octave = "OSC_1_OCTAVE";
    osc1Parameters.semitone = "OSC_1_SEMITONE";
    osc1Parameters.finetune = "OSC_1_FINE_TUNE";
    osc1Parameters.wavetype = "OSC_1_WAVE_TYPE";
    osc1Parameters.gain = "OSC_1_GAIN";
    osc1.setParameters(osc1Parameters);

    // init osc2 parameter names struct
    SympleOscParameterNames osc2Parameters;
    osc2Parameters.octave = "OSC_2_OCTAVE";
    osc2Parameters.semitone = "OSC_2_SEMITONE";
    osc2Parameters.finetune = "OSC_2_FINE_TUNE";
    osc2Parameters.wavetype = "OSC_2_WAVE_TYPE";
    osc2Parameters.gain = "OSC_2_GAIN";
    osc2.setParameters(osc2Parameters);

    filterParameters.cutoff = "FILTER_1_CUTOFF";
    filterParameters.resonance = "FILTER_1_RESONANCE";
    filterParameters.amount = "FILTER_1_AMOUNT";
    filterParameters.mode = "FILTER_1_MODE";
    filterEnvNames.attack = "FILTER_1_ATTACK";
    filterEnvNames.decay = "FILTER_1_DECAY";
    filterEnvNames.sustain = "FILTER_1_SUSTAIN";
    filterEnvNames.release = "FILTER_1_RELEASE";
    filter.setParameters(filterParameters, filterEnvNames);

    //filterParameters.cutoff = "FILTER_2_CUTOFF";
    //filterParameters.resonance = "FILTER_2_RESONANCE";
    //filterParameters.amount = "FILTER_2_AMOUNT";
    //filterParameters.mode = "FILTER_2_MODE";
    //filterEnvNames.attack = "FILTER_2_ATTACK";
    //filterEnvNames.decay = "FILTER_2_DECAY";
    //filterEnvNames.sustain = "FILTER_2_SUSTAIN";
    //filterEnvNames.release = "FILTER_2_RELEASE";
    //filter2.setParameters(filterParameters, filterEnvNames);

    // init gain parameter names struct
    MasterAmpParameterNames ampParameters;
    ampParameters.gain = "MASTER_GAIN";
    amplifier.setParameters(ampParameters);

    // Add Components
    addAndMakeVisible(osc1);
    addAndMakeVisible(osc2);
    addAndMakeVisible(keyboardComponent);
    addAndMakeVisible(filter);
    addAndMakeVisible(filter2);
    addAndMakeVisible(amplifier);
    addAndMakeVisible(lfo);

    // Add Labels
    addAndMakeVisible(osc1Label);
    osc1Label.setText("OSCILLATOR 1", juce::dontSendNotification);
    osc1Label.setFont(juce::Font(18.0, juce::Font::bold));
    osc1Label.setJustificationType(juce::Justification::centred);
    osc1Label.attachToComponent(&osc1, false);

    addAndMakeVisible(osc2Label);
    osc2Label.setText("OSCILLATOR 2", juce::dontSendNotification);
    osc2Label.setFont(juce::Font(18.0, juce::Font::bold));
    osc2Label.setJustificationType(juce::Justification::centred);
    osc2Label.attachToComponent(&osc2, false);

    addAndMakeVisible(filter1Label);
    filter1Label.setText("FILTER 1", juce::dontSendNotification);
    filter1Label.setFont(juce::Font(18.0, juce::Font::bold));
    filter1Label.setJustificationType(juce::Justification::centred);
    filter1Label.attachToComponent(&filter, false);
    
    addAndMakeVisible(filter2Label);
    filter2Label.setText("FILTER 2", juce::dontSendNotification);
    filter2Label.setFont(juce::Font(18.0, juce::Font::bold));
    filter2Label.setJustificationType(juce::Justification::centred);
    filter2Label.attachToComponent(&filter2, false);

    addAndMakeVisible(ampLabel);
    ampLabel.setText("AMPLIFIER", juce::dontSendNotification);
    ampLabel.setFont(juce::Font(18.0, juce::Font::bold));
    ampLabel.setJustificationType(juce::Justification::centred);
    ampLabel.attachToComponent(&amplifier, false);
    
    addAndMakeVisible(lfoLabel);
    lfoLabel.setText("LFO", juce::dontSendNotification);
    lfoLabel.setFont(juce::Font(18.0, juce::Font::bold));
    lfoLabel.setJustificationType(juce::Justification::centred);
    lfoLabel.attachToComponent(&lfo, false);

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
    osc1.setBounds(oscArea.removeFromTop(componentHeight).reduced(margin));
    osc2.setBounds(oscArea.removeFromTop(componentHeight).reduced(margin));

    filter.setBounds(filterArea.removeFromTop(componentHeight).reduced(margin));
    filter2.setBounds(filterArea.removeFromTop(componentHeight * 2).reduced(margin));
    amplifier.setBounds(ampArea.removeFromTop(componentHeight).reduced(margin));
    lfo.setBounds(ampArea.removeFromTop(componentHeight).reduced(margin));
 }

void SympleSynthAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
}
