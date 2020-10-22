/*
  ==============================================================================

    SineWaveOsc.h
    Created: 18 Oct 2020 7:41:04pm
    Author:  whompyjaw

  ==============================================================================
*/

#pragma once
/*
  ==============================================================================

    SynthAudioSource.h
    Created: 13 Oct 2020 11:31:47pm
    Author:  whompyjaw

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

// This code was taken from the Juce tutorial "SynthUsingMidiInputTutorial"

/*
Describes one of the sounds that a Synthesiser can play.
A synthesiser can contain one or more sounds, and a sound can choose which midi notes and
channels can trigger it.
SynthesiserSounds is a passive class that just describes what the sound is - the actual
audio rendering for a sound is done by a SynthesiserVoice.
KEY POINT: This allows more than one SynthesiserVoice to play the same sounds at the same time
creating polyphony.
*/
struct SympleSound : public juce::SynthesiserSound
{
    SympleSound() {}

    // Not sure why these are overridden functions when they don't exist in the "SynthesizerSounds" class
    bool appliesToNote(int midiNoteNumber) override { return true; }
    bool appliesToChannel(int midiNoteNumber) override { return true; }
};


/* Represents a voice that a Synthesiser can use to play a SynthesizerSound
A voice plays a single sound at a time, and a synthesiser holds an array of voices so that it can
play polyphonically */
struct SympleVoice : public juce::SynthesiserVoice
{
    // TODO: Add a method to choose the different oscillator types
    SympleVoice(juce::ADSR::Parameters& ampParameters);

    bool canPlaySound(juce::SynthesiserSound* sound) override;

    // Start the sine tone based on midi input
    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int /*currentPitchWheelPosition*/) override;

    /* Stops the voice by the owning synthesiser calling this function, which must be overriden*/
    void stopNote(float /*velocity*/, bool allowTailOff) override;

    void pitchWheelMoved(int) override {}
    void controllerMoved(int, int) override {}

    /* Renders the next block of data for this voice. */
    void renderNextBlock(juce::AudioSampleBuffer& outputBuffer, int startSample, int numSamples) override;
    
    void setAmpParameters(juce::ADSR::Parameters& params);

    void prepareOscillators(juce::dsp::ProcessSpec spec);


private:
    double currentAngle = 0.0, angleDelta = 0.0, level = 0.0;
    juce::ADSR amplifier;
    juce::ADSR::Parameters& ampParameters;
    /*enum
    {
        oscOneIndex,
        gainOneIndex
    };*/
    /* I would add oscTwoIndex and gainTwoIndex here... Maybe sawWaveIndex, triIndex, etc, but not 100% sure */
    juce::dsp::Oscillator<float> osc1;
    //juce::dsp::ProcessorChain<juce::dsp::Oscillator<float>, juce::dsp::Gain<float>> processorChain;
    // TODO: once i figure this out, then i can chain all four oscillators together.
};

class SympleOscillator
{
public:
    SympleOscillator() {}
    ~SympleOscillator() {}

    enum WaveType
    {
        sine = 0,
        saw = 1
    };
    void setWaveType(int type);
    void setLevel(float level);
    void setFrequency(float frequency);


private:
    WaveType waveTypes;

};
