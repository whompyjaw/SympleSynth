/*
  ==============================================================================

    SineWaveOsc.cpp
    Created: 18 Oct 2020 7:41:04pm
    Author:  whompyjaw

  ==============================================================================
*/

#include "SineWaveOsc.h"

SineWaveVoice::SineWaveVoice(juce::ADSR::Parameters& ampParameters, juce::AudioProcessorValueTreeState& tree) : ampParameters(ampParameters)
{
    osc.setMode(OSCILLATOR_MODE_SQUARE);
    amplifier.setSampleRate(getSampleRate());
    amplifier.setParameters(ampParameters);
    osc.setSampleRate(getSampleRate());
    oscTree = &tree;
//    osc.setFrequency(oscTree.getParameter("Octave"));
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

    // TODO: calculate the frequency from the midi and the APVST
    int currentOctave = oscTree->getParameterAsValue("OSC_OCTAVE").getValue();
    midiNoteNumber += currentOctave;
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
    if (amplifier.isActive())
    {
        osc.generate(outputBuffer, numSamples, amplifier);
        if (!amplifier.isActive()) {
            amplifier.reset();
        }
    }
}

void SineWaveVoice::setAmpParameters(juce::ADSR::Parameters& params)
{
    amplifier.setParameters(params);
}
