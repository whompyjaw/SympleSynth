/*
  ==============================================================================

    SineWaveOsc.cpp
    Created: 18 Oct 2020 7:41:04pm
    Author:  whompyjaw

  ==============================================================================
*/

#include "SineWaveOsc.h"

SineWaveVoice::SineWaveVoice(juce::ADSR::Parameters& ampParameters) : ampParameters(ampParameters)
{
    amplifier.setSampleRate(getSampleRate());
    amplifier.setParameters(ampParameters);
}

bool SineWaveVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<SineWaveSound*> (sound) != nullptr; // Not sure what this does
}

// Start the sine tone based on midi input

void SineWaveVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int)
{
    amplifier.noteOn();
    currentAngle = 0.0;
    level = velocity * 0.15;

    auto cyclesPerSecond = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber); // convert midi note number to hertz
    auto cyclesPerSample = cyclesPerSecond / getSampleRate();

    angleDelta = cyclesPerSample * 2.0 * juce::MathConstants<double>::pi; // Creates the sine tone. I 
}

/* Stops the voice by the owning synthesiser calling this function, which must be overriden*/

void SineWaveVoice::stopNote(float, bool allowTailOff)
{
    amplifier.noteOff();
}

/* Renders the next block of data for this voice. */

void SineWaveVoice::renderNextBlock(juce::AudioSampleBuffer& outputBuffer, int startSample, int numSamples)
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

void SineWaveVoice::setAmpParameters(juce::ADSR::Parameters& params)
{
    amplifier.setParameters(params);
}
