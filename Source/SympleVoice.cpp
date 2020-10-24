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
    filterAmp.noteOn();

    auto freqHz = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    processorChain.get<osc1Index>().setFrequency (freqHz, true);
    processorChain.get<osc1Index>().setLevel (velocity);
 
}

/* Stops the voice by the owning synthesiser calling this function, which must be overriden*/

void SympleVoice::stopNote(float, bool allowTailOff)
{
    amplifier.noteOff();
    filterAmp.noteOff();
}

/* Renders the next block of data for this voice. */

void SympleVoice::renderNextBlock(juce::AudioSampleBuffer& outputBuffer, int startSample, int numSamples)
{
    auto output = tempBlock.getSubBlock (0, (size_t) numSamples);
    output.clear();

//    for (size_t pos = 0; pos < (size_t) numSamples;)
//    {
//        auto max = juce::jmin ((size_t) numSamples - pos, lfoUpdateCounter);
//        auto block = output.getSubBlock (pos, max);

        juce::dsp::ProcessContextReplacing<float> context (output);
        processorChain.process (context);

        
//        lfoUpdateCounter -= max;

//        if (lfoUpdateCounter == 0)
//        {
//            lfoUpdateCounter = lfoUpdateRate;
//            auto lfoOut = lfo.processSample (0.0f);                                 // [5]
//            auto curoffFreqHz = juce::jmap (lfoOut, -1.0f, 1.0f, 100.0f, 2000.0f);  // [6]
//            processorChain.get<filterIndex>().setCutoffFrequencyHz (curoffFreqHz);  // [7]
//        }
//    }
//
    juce::dsp::AudioBlock<float> (outputBuffer)
        .getSubBlock ((size_t) startSample, (size_t) numSamples)
        .add (tempBlock);

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



