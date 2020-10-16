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
struct SineWaveSound : public juce::SynthesiserSound
{
    SineWaveSound() {}

    // Not sure why these are overridden functions when they don't exist in the "SynthesizerSounds" class
    bool appliesToNote(int midiNoteNumber) override { return true; }
    bool appliesToChannel(int midiNoteNumber) override { return true; }
};


/* Represents a voice that a Synthesiser can use to play a SynthesizerSound
A voice plays a single sound at a time, and a synthesiser holds an array of voices so that it can
play polyphonically */
struct SineWaveVoice : public juce::SynthesiserVoice
{
    SineWaveVoice(juce::ADSR::Parameters& ampParameters) : ampParameters(ampParameters)
    {
        amplifier.setSampleRate(getSampleRate());
        amplifier.setParameters(ampParameters);
    }

    bool canPlaySound(juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<SineWaveSound*> (sound) != nullptr; // Not sure what this does
    }

    // Start the sine tone based on midi input
    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int /*currentPitchWheelPosition*/) override
    {
        amplifier.noteOn();
        currentAngle = 0.0;
        level = velocity * 0.15;

        auto cyclesPerSecond = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber); // convert midi note number to hertz
        juce::Logger::outputDebugString(std::to_string(midiNoteNumber));
        auto cyclesPerSample = cyclesPerSecond / getSampleRate();

        angleDelta = cyclesPerSample * 2.0 * juce::MathConstants<double>::pi; // Creates the sine tone. I 
    }

    /* Stops the voice by the owning synthesiser calling this function, which must be overriden*/
    void stopNote(float /*velocity*/, bool allowTailOff) override
    {
        amplifier.noteOff();
    }

    void pitchWheelMoved(int) override {}
    void controllerMoved(int, int) override {}

    /* Renders the next block of data for this voice. */
    void renderNextBlock(juce::AudioSampleBuffer& outputBuffer, int startSample, int numSamples) override
    {
        if (angleDelta != 0.0) // Not silent
        {
            while (--numSamples >= 0)
            {
                auto currentSample = (float)(std::sin(currentAngle) * level * amplifier.getNextSample());

                for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
                    outputBuffer.addSample(i, startSample, currentSample);

                currentAngle += angleDelta;
                ++startSample;

                if (!amplifier.isActive()) {
                    clearCurrentNote();
                    amplifier.reset();
                    angleDelta = 0.0;
                    break;
                }
            }
        }
    }
    
    void setAmpParameters(juce::ADSR::Parameters& params)
    {
        amplifier.setParameters(params);
    }

private:
    double currentAngle = 0.0, angleDelta = 0.0, level = 0.0, tailOff = 0.0;
    juce::ADSR amplifier;
    juce::ADSR::Parameters& ampParameters;
};
