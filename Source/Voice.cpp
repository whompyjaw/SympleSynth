/*
  ==============================================================================

    SineWaveOsc.cpp
    Created: 18 Oct 2020 7:41:04pm
    Author:  whompyjaw

  ==============================================================================
*/

#include "Voice.h"

SineWaveVoice::SineWaveVoice(juce::AudioProcessorValueTreeState& tree)
    : oscTree(tree)
{
    // initialize all ADSR parameters
    readParameterState();

    // initialize oscillator
    osc.setMode(OSCILLATOR_MODE_SQUARE);
    osc.setSampleRate(getSampleRate());

    // initialize amplifier envelope
    envelope.setSampleRate(getSampleRate());
    envelope.setParameters(envelopeParameters);
    
    // intialize filter envelope
    filterEnvelope.setSampleRate(getSampleRate());
    filterEnvelope.setParameters(filterEnvelopeParameters);
    
    // initialize filter
//    juce::dsp::ProcessSpec spec;
//    spec.sampleRate = sampleRate;
//    spec.maximumBlockSize = samplesPerBlock;
//    spec.numChannels = getTotalNumOutputChannels();
//
//    lowPassFilter.prepare(spec);
//    lowPassFilter.reset();
}

bool SineWaveVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<SineWaveSound*> (sound) != nullptr; // Not sure what this does
}

// Start the sine tone based on midi input

void SineWaveVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int)
{
    readParameterState();
    envelope.noteOn();
    filterEnvelope.noteOn();
    osc.startNote();
    level = velocity * 0.15;

    // calculate the frequency from the midi and the APVST
    int currentOctave = oscTree.getParameterAsValue("OSC_1_OCTAVE").getValue();
    midiNoteNumber += currentOctave * 12; //(if currentOctave = -2, -2 * 12 = -24
    int currentSemitone = oscTree.getParameterAsValue("OSC_1_SEMITONE").getValue();
    midiNoteNumber += currentSemitone;
    
    // turn midi note number into hertz
    auto cyclesPerSecond = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    
    // adjust the frequency with value from the fine tune knob
    float fineTune = oscTree.getParameterAsValue("OSC_1_FINE_TUNE").getValue();
    // cents formula adapted from http://hyperphysics.phy-astr.gsu.edu/hbase/Music/cents.html
    auto adjustedFreq = 2 * pow(2, fineTune/1200) * cyclesPerSecond;
    osc.setFrequency(adjustedFreq);
}

/* Stops the voice by the owning synthesiser calling this function, which must be overriden*/

void SineWaveVoice::stopNote(float, bool allowTailOff)
{
    envelope.noteOff();
    filterEnvelope.noteOff();
}

/* Renders the next block of data for this voice. */

void SineWaveVoice::renderNextBlock(juce::AudioSampleBuffer& outputBuffer, int startSample, int numSamples)
{
    if (envelope.isActive())
    {
        osc.generate(outputBuffer, numSamples, envelope);
        if (!envelope.isActive()) {
            envelope.reset();
        }
    }
}

/*
 *  Reads the state of parameters in the value state tree and sets
 *  class variables that hold envelope, filter, and oscillator params
 */
void SineWaveVoice::readParameterState()
{
    envelopeParameters = {
        oscTree.getRawParameterValue("AMP_ATTACK")->load(),
        oscTree.getRawParameterValue("AMP_DECAY")->load(),
        oscTree.getRawParameterValue("AMP_SUSTAIN")->load() / 100,
        oscTree.getRawParameterValue("AMP_RELEASE")->load()
    };
    
    filterEnvelopeParameters = {
        oscTree.getRawParameterValue("FILTER_ATTACK")->load(),
        oscTree.getRawParameterValue("FILTER_DECAY")->load(),
        oscTree.getRawParameterValue("FILTER_SUSTAIN")->load() / 100,
        oscTree.getRawParameterValue("FILTER_RELEASE")->load(),
    };
}

/*
 *  Code for this block is adapted from the JUCE DSP tutorial for LFO filter
 *  cutoff triggering. The specific code is available under the heading
 *  "Modulating the signal with an LFO" numbers 5, 6, 7 at:
 *  https://docs.juce.com/master/tutorial_dsp_introduction.html
 *
 *  This function processes an audio block with the filter sections
*/
void SineWaveVoice::filterNextBlock(juce::AudioBuffer<float>& buffer)
{
//    juce::dsp::AudioBlock<float> block(buffer);
//    size_t numSamples = block.getNumSamples();
//
//    size_t updateRate = FILTER_UPDATE_RATE;
//    size_t updateCounter = updateRate;
//    size_t read = 0;
//    while (read < numSamples) {
//        auto max = juce::jmin((size_t) numSamples - read, updateCounter);
//        auto subBlock = block.getSubBlock (read, max);
//
//        lowPassFilter.process(juce::dsp::ProcessContextReplacing<float>(subBlock));
//
//        read += max;
//        updateCounter -= max;
//        float nextAmpSample = filterAmp.getNextSample();
//
//        if (updateCounter == 0)
//        {
//            updateCounter = updateRate;
//            float freq = tree.getRawParameterValue("CUTOFF")->load();
//            float res = tree.getRawParameterValue("RESONANCE")->load() / 10;
//            float amount = tree.getRawParameterValue("AMOUNT")->load() / 100;
//            float freqMax = freq + ((20000.0f - freq) * amount);
//            auto cutOffFreqHz = juce::jmap (nextAmpSample, 0.0f, 1.0f, freq, freqMax);
//            *lowPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(lastSampleRate, cutOffFreqHz, res);
//        }
//    }
}
