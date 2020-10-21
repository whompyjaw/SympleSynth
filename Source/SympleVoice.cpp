/*
  ==============================================================================

    SineWaveOsc.cpp
    Created: 18 Oct 2020 7:41:04pm
    Author:  whompyjaw

  ==============================================================================
*/

#include "SympleVoice.h"

SympleVoice::SympleVoice(juce::ADSR::Parameters& ampParameters) : ampParameters(ampParameters)
{
    amplifier.setSampleRate(getSampleRate());
    amplifier.setParameters(ampParameters);
}

bool SympleVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<SympleSound*> (sound) != nullptr; // Not sure what this does
}

/* Start the sine tone based on midi input converting the midi note number to a frequency */
void SympleVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int)
{
    amplifier.noteOn();
    currentAngle = 0.0;
    level = velocity * 0.15;

    auto cyclesPerSecond = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber); // convert midi note number to hertz
    auto cyclesPerSample = cyclesPerSecond / getSampleRate();

    angleDelta = cyclesPerSample * 2.0 * juce::MathConstants<double>::pi; // Creates the sone tone at that frequency
    // this is similar to 
    //mPhaseIncrement = mFrequency * 2 * mPI / mSampleRate;
    // angleDelta is the same as phase increment
}

/* Stops the voice by the owning synthesiser calling this function, which must be overriden*/
void SympleVoice::stopNote(float, bool allowTailOff)
{
    amplifier.noteOff();
}

/* Renders the next block of data for this voice, and while held down. */
void SympleVoice::renderNextBlock(juce::AudioSampleBuffer& outputBuffer, int startSample, int numSamples)
{
    if (angleDelta != 0.0) // Not silent
    {
        while (--numSamples >= 0) // note is being held down
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

void SympleVoice::setAmpParameters(juce::ADSR::Parameters& params)
{
    amplifier.setParameters(params);
}
