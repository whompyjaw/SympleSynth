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
    osc.setMode(OSCILLATOR_MODE_SQUARE);
    amplifier.setSampleRate(getSampleRate());
    amplifier.setParameters(ampParameters);
    osc.setSampleRate(getSampleRate());
}

bool SineWaveVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<SineWaveSound*> (sound) != nullptr; // Not sure what this does
}

// Start the sine tone based on midi input

void SineWaveVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int)
{
    amplifier.noteOn();
    osc.startNote();
    level = velocity * 0.15;

    auto cyclesPerSecond = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber); // convert midi note number to hertz
    osc.setFrequency(cyclesPerSecond);
}

/* Stops the voice by the owning synthesiser calling this function, which must be overriden*/

void SineWaveVoice::stopNote(float, bool allowTailOff)
{
    amplifier.noteOff();
}

/* Renders the next block of data for this voice. */

void SineWaveVoice::renderNextBlock(juce::AudioSampleBuffer& outputBuffer, int startSample, int numSamples)
{
    double buff[numSamples];
    osc.generate(buff, numSamples);

    if (amplifier.isActive()) // Not silent
    {
        while (--numSamples >= 0)
        {
            auto currentSample = (float)(buff[startSample] * level * amplifier.getNextSample());

            for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
                outputBuffer.addSample(i, startSample, currentSample);

            ++startSample;

            if (!amplifier.isActive()) {
                clearCurrentNote();
                amplifier.reset();
                break;
            }
        }
    }
}

void SineWaveVoice::setAmpParameters(juce::ADSR::Parameters& params)
{
    amplifier.setParameters(params);
}
