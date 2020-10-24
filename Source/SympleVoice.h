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

class SympleSynth : juce::Synthesiser
{
public:
    SympleSynth(juce::ADSR::Parameters, juce::ADSR);
    void prepare(const juce::dsp::ProcessSpec& spec);
    void renderNextBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&, int, int);
private:
    const int VOICE_COUNT = 64;
    
 
};

class SympleOscillator
{
public:
    SympleOscillator()
    {
        auto& osc = processorChain.get<oscIndex>();
        osc.initialise ([] (float x)
        {
            return juce::jmap (x,
                               float (-juce::MathConstants<double>::pi),
                               float (juce::MathConstants<double>::pi),
                               float (-1),
                               float (1));
        }, 2);

    }
    void setFrequency (float newValue, bool force = false)
    {
        auto& osc = processorChain.get<oscIndex>();
        osc.setFrequency (newValue, force);
    }

    //==============================================================================
    void setLevel (float newValue)
    {
        auto& gain = processorChain.get<gainIndex>();
        gain.setGainLinear (newValue);
    }

    //==============================================================================
    void reset() noexcept
    {
         processorChain.reset();
    }

    //==============================================================================
    /* Context will be an block of audio, essentially*/
    void process (const juce::dsp::ProcessContextReplacing<float>& context) noexcept
    {
        processorChain.process (context);
    }

    //==============================================================================
    void prepare (const juce::dsp::ProcessSpec& spec)
    {
        processorChain.prepare (spec);
    }

private:
    enum
    {
        oscIndex,
        gainIndex
    };
    
    using Osc = juce::dsp::Oscillator<float>;
    using Gain = juce::dsp::Gain<float>;
    juce::dsp::ProcessorChain<Osc,Gain>processorChain;
};






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
    SympleVoice(juce::ADSR::Parameters&, juce::ADSR&);

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
    
    //============ ProcessorChain =========================
    void prepare(const juce::dsp::ProcessSpec&);

private:
    double currentAngle = 0.0, angleDelta = 0.0, level = 0.0;
    juce::ADSR amplifier;
    juce::ADSR& filterAmp;
    juce::ADSR::Parameters& ampParameters;
    
    juce::HeapBlock<char> heapBlock;
    juce::dsp::AudioBlock<float> tempBlock;

    enum
    {
        osc1Index
    };
    juce::dsp::ProcessorChain<SympleOscillator> processorChain;
};

