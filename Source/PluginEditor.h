/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
//==============================================================================
/**
*/
class SympleSynthAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    SympleSynthAudioProcessorEditor (SympleSynthAudioProcessor&);
    ~SympleSynthAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    /* The combination of this function, the AudioProcessorEditor inheriting Timer and the startTime
    function call in the SympleSynthAPE constructor, will apply keyboard focus so that you don't 
    have to click the plugin to make it work. */
    //void timerCallback() override
    //{
    //    keyboardComponent.grabKeyboardFocus();
    //    stopTimer();
    //}

    //juce::MidiKeyboardState keyboardState;
    //juce::MidiKeyboardComponent keyboardComponent;
    SympleSynthAudioProcessor& audioProcessor;


   /* juce::ComboBox midiInputList;
    juce::Label midiInputListLabel;
    int lastInputIndex = 0;*/

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SympleSynthAudioProcessorEditor)
};
