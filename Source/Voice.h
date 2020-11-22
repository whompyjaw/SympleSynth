/*
  ==============================================================================

    SineWaveOsc.h
    Created: 18 Oct 2020 7:41:04pm
    Author:  whompyjaw
    Notes: The foundation of this code was adapted from the Juce tutorial "SynthUsingMidiInputTutorial"

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "Osc.h"

/*
Describes one of the sounds that a Synthesiser can play.
A synthesiser can contain one or more sounds, and a sound can choose which midi notes and
channels can trigger it.
SynthesiserSounds is a passive class that just describes what the sound is - the actual
audio rendering for a sound is done by a SynthesiserVoice.
KEY POINT: This allows more than one SynthesiserVoice to play the same sounds at the same time
creating polyphony.
*/
struct SynthSound : public juce::SynthesiserSound
{
    SynthSound() {}

    bool appliesToNote(int midiNoteNumber) override { return true; }
    bool appliesToChannel(int midiNoteNumber) override { return true; }
};


/* Represents a voice that a Synthesiser can use to play a SynthesizerSound
A voice plays a single sound at a time, and a synthesiser holds an array of voices so that it can play polyphonically. The Synthesiser controls the voices */
struct SynthVoice : public juce::SynthesiserVoice
{
    SynthVoice(juce::AudioProcessorValueTreeState&, juce::dsp::AudioBlock<float>&);

    bool canPlaySound(juce::SynthesiserSound* sound) override;

    // Start the sine tone based on midi input
    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int /*currentPitchWheelPosition*/) override;

    /* Stops the voice by the owning synthesiser calling this function, which must be overriden*/
    void stopNote(float /*velocity*/, bool allowTailOff) override;

    void pitchWheelMoved(int) override {}
    void controllerMoved(int, int) override {}

    /* Renders the next block of data for this voice. */
    void renderNextBlock(juce::AudioSampleBuffer& outputBuffer, int startSample, int numSamples) override;
    
    /* sets buffer and sample rate for juce dsp */
    void prepare(const juce::dsp::ProcessSpec& spec);

private:
    float freq;
    float res;
    float amount;
    float lfoAmount;
    int lfoSample;
    float freqMax;
    float lfoFreqMax;
    float lfoCutoffFreqHz;
    int osc1ModeInt = 0;
    int osc2ModeInt = 0;
    double twelfthRoot = pow(2.0, 1.0 / 12.0);
    juce::String maxString = "max: ";
    juce::String readString = "read: ";
    const int PARAM_UPDATE_RATE = 100; // the number of samples each parameter setting will process

    // memory for voice processing
    juce::HeapBlock<char> heap1Block;
    juce::HeapBlock<char> heap2Block;
    juce::dsp::AudioBlock<float> voice1Block;
    juce::dsp::AudioBlock<float> voice2Block;
    juce::dsp::AudioBlock<float>& lfoBuffer;

    juce::ADSR ampEnvelope;
    juce::dsp::LadderFilterMode filterMode;
    int filterModeInt;
    juce::ADSR filterEnvelope;
    juce::ADSR filter2Envelope;
    juce::ADSR::Parameters ampEnvelopeParameters;
    juce::ADSR::Parameters filterEnvelopeParameters;
    juce::ADSR::Parameters filter2EnvelopeParameters;
    juce::AudioProcessorValueTreeState& oscTree;
    Oscillator osc1;
    Oscillator osc2;
    Oscillator noiseOsc;

    juce::dsp::LadderFilter<float> filter1;
    juce::dsp::LadderFilter<float> filter2;
    OscillatorMode oscMode;

    void readParameterState();
    void applyAmpEnvelope(juce::dsp::AudioBlock<float>&, juce::dsp::AudioBlock<float>&);
    void setFilter(size_t, float, float);

};
