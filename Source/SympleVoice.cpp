/*
  ==============================================================================

    SineWaveOsc.cpp
    Created: 18 Oct 2020 7:41:04pm
    Author:  whompyjaw

  ==============================================================================
*/

#include "SympleVoice.h"

SympleVoice::SympleVoice(juce::ADSR::Parameters& ampParameters, juce::ADSR& filterAmp)
    : filterAmp(filterAmp), ampParameters(ampParameters)
{
    amplifier.setSampleRate(getSampleRate());
    amplifier.setParameters(ampParameters);
}

bool SympleVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<SympleSound*> (sound) != nullptr; // Not sure what this does
}

// Start the sine tone based on midi input

void SympleVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int)
{
    amplifier.noteOn();
//    filterAmp.noteOn();

    auto freqHz = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    processorChain.get<osc1Index>().setFrequency (freqHz, true);
    processorChain.get<osc1Index>().setLevel (velocity*0.15);
 
}

/* Stops the voice by the owning synthesiser calling this function, which must be overriden*/

void SympleVoice::stopNote(float, bool allowTailOff)
{
    amplifier.noteOff();
//    filterAmp.noteOff();
}

/* Renders the next block of data for this voice. */

void SympleVoice::renderNextBlock(juce::AudioSampleBuffer& outputBuffer, int startSample, int numSamples)
{
    if (amplifier.isActive())
    {
        juce::dsp::AudioBlock<float> output (outputBuffer);

        juce::dsp::ProcessContextReplacing<float> context (output);
        processorChain.process (context);

        // set amplifier adsr to buffer
//        amplifier.applyEnvelopeToBuffer(outputBuffer, startSample, numSamples);
        
//        // clear synth voice and adsr if note is over
        if (!amplifier.isActive())
        {
            clearCurrentNote();
            amplifier.reset();
        }
        //Note: Later this will be used for the LFO will need to take small chunks of the samples and apply lfo stuff to them.
    }
}

void SympleVoice::setAmpParameters(juce::ADSR::Parameters& params)
{
    amplifier.setParameters(params);
}

void SympleVoice::prepare(const juce::dsp::ProcessSpec& spec)
{
    tempBlock = juce::dsp::AudioBlock<float> (heapBlock, spec.numChannels, spec.maximumBlockSize);
    processorChain.prepare (spec);
}



