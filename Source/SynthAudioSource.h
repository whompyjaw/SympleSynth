/*
  ==============================================================================

    SynthAudioSource.h
    Created: 13 Oct 2020 11:31:47pm
    Author:  whompyjaw

  ==============================================================================
*/
#include <JuceHeader.h>
#pragma once


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
    bool appliesToNote(int) override { return true; }
    bool appliesToChannel(int) override { return true; }
};


/* Represents a voice that a Synthesiser can use to play a SynthesizerSound
A voice plays a single sound at a time, and a synthesiser holds an array of voices so that it can
play polyphonically */
struct SineWaveVoice : public juce::SynthesiserVoice
{
    SineWaveVoice() {}

    bool canPlaySound(juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<SineWaveSound*> (sound) != nullptr; // Not sure what this does
    }

    // Start the sine tone based on midi input
    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int /*currentPitchWheelPosition*/) override
    {
        currentAngle = 0.0;
        level = velocity * 0.15;
        tailOff = 0.0; // Q: not sure what this is for

        auto cyclesPerSecond = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber); // convert midi note number to hertz
        juce::Logger::outputDebugString(std::to_string(midiNoteNumber));
        auto cyclesPerSample = cyclesPerSecond / getSampleRate();

        angleDelta = cyclesPerSample * 2.0 * juce::MathConstants<double>::pi; // Creates the sine tone. I 
    }

    /* Stops the voice by the owning synthesiser calling this function, which must be overriden*/
    void stopNote(float /*velocity*/, bool allowTailOff) override
    {

        // You could just have clearCurrentNote();, but the below code adds extra bits I think
        if (allowTailOff)
        {
            if (tailOff == 0.0)
                tailOff = 1.0;
        }
        else
        {
            clearCurrentNote();
            angleDelta = 0.0;
        }
    }

    void pitchWheelMoved(int) override {}
    void controllerMoved(int, int) override {}

    /* Renders the next block of data for this voice. */
    // TODO: Try to add a slower attack so they don't play abruptly.
    void renderNextBlock(juce::AudioSampleBuffer& outputBuffer, int startSample, int numSamples) override
    {
        if (angleDelta != 0.0) // Not silent
        {
            /* When the key has been released the tailOff value will be greater than 0*/
            if (tailOff > 0.0)
            {
                while (--numSamples >= 0)
                {
                    auto currentSample = (float)(std::sin(currentAngle) * level * tailOff);

                    for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
                        outputBuffer.addSample(i, startSample, currentSample);

                    currentAngle += angleDelta;
                    ++startSample;

                    // TODO: This is maybe where we would add our own decay value?
                    tailOff *= 0.99; // Simple exponential decay envelope shape

                    // If note has decayed enough, kill the sound
                    // Note: This could be placed in the stopNote method as well
                    if (tailOff <= 0.005)
                    {
                        clearCurrentNote();
                        angleDelta = 0.0;
                        break;
                    }
                }
            }
            else
            {
                /* Used for the normal state of the voice, while the key is being held down
                Notice that we use the AudioSampleBuffer::addSample() function, which mixes
                the currentSample value with the value alread at index startSample. This is
                because the synthesiser will be iterating over all of the voices.
                It is the responsibility of each voice to mix its output with the current
                contents of the buffer.*/
                while (--numSamples >= 0)
                {
                    auto currentSample = (float)(std::sin(currentAngle) * level);

                    for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
                        outputBuffer.addSample(i, startSample, currentSample);

                    currentAngle += angleDelta;
                    ++startSample;
                }
            }
        }
    }

private:
    double currentAngle = 0.0, angleDelta = 0.0, level = 0.0, tailOff = 0.0;
};

class SynthAudioSource : public juce::AudioSource
{
public:
    SynthAudioSource(juce::MidiKeyboardState& keyState) : keyboardState(keyState)
    {
        // Add number of synth voices
        for (auto i = 0; i < 4; ++i)
            synth.addVoice(new SineWaveVoice());

        synth.addSound(new SineWaveSound());
    }

    void setUsingSineWaveSound()
    {
        synth.clearSounds();
    }

    /* In order to process the timestamps of the MIDI data the collector class needs to know
    the audio sample rate. That is set here.*/
    void prepareToPlay(int /*samplesPerBlockExpected*/, double sampleRate) override
    {
        synth.setCurrentPlaybackSampleRate(sampleRate); // Needs to know the sample rate of audio output
        midiCollector.reset(sampleRate);
    }

    void releaseResources() override {}


    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override
    {
        bufferToFill.clearActiveBufferRegion(); // otherwise i think you would have garbage data

        juce::MidiBuffer incomingMidi;
        midiCollector.removeNextBlockOfMessages(incomingMidi, bufferToFill.numSamples); // pull any MIDI messages for each block of audio

        // Pull midi buffers from keyboardState object
        keyboardState.processNextMidiBuffer(incomingMidi, bufferToFill.startSample, bufferToFill.numSamples, true);

        // These buffers of midi are passed to the synth which whill be used to render the voices using the timestamps
        // of the note-on and note-off messages (and other MIDI channel voice messages)
        synth.renderNextBlock(*bufferToFill.buffer, incomingMidi, bufferToFill.startSample, bufferToFill.numSamples);
    }

    // Accessor function 
    juce::MidiMessageCollector* getMidiCollector()
    {
        return &midiCollector;
    }

private:
    juce::MidiKeyboardState& keyboardState; // Note this is second time calling keyboardState, so not sure what this is for again
    juce::MidiMessageCollector midiCollector;
    juce::Synthesiser synth; // Only one synth. It manages the lifetime of the voices.


};