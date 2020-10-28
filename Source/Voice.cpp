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
    
    // intialize filter envelope
    filterEnvelope.setSampleRate(getSampleRate());
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

/*
 *  Code for this block is adapted from the JUCE DSP tutorial for LFO filter
 *  cutoff triggering. The specific code is available under the heading
 *  "Modulating the signal with an LFO" numbers 5, 6, 7 at:
 *  https://docs.juce.com/master/tutorial_dsp_introduction.html
 *
 *  This function generates sound and applies audio filters
*/
void SineWaveVoice::renderNextBlock(juce::AudioSampleBuffer& outputBuffer, int startSample, int numSamples)
{
    if (envelope.isActive())
    {
        // clear voice block for processing
        voiceBlock.clear();
        
        size_t updateCounter = FILTER_UPDATE_RATE;
        size_t read = 0;
        while (read < numSamples) {
            auto max = juce::jmin((size_t) numSamples - read, updateCounter);
            auto subBlock = voiceBlock.getSubBlock (read, max);

            // add oscillator 1 sound
            osc.generate(subBlock, (int) subBlock.getNumSamples(), envelope);

            // filter sound
            filter.process(juce::dsp::ProcessContextReplacing<float>(subBlock));

            // set counters
            read += max;
            updateCounter -= max;
            float nextFilterEnvSample = filterEnvelope.getNextSample();

            if (updateCounter == 0)
            {
                updateCounter = FILTER_UPDATE_RATE;
                float freq = oscTree.getRawParameterValue("CUTOFF")->load();
                float res = oscTree.getRawParameterValue("RESONANCE")->load() / 100;
                float amount = oscTree.getRawParameterValue("AMOUNT")->load() / 100;
                float freqMax = freq + ((20000.0f - freq) * amount);
                auto cutOffFreqHz = juce::jmap (nextFilterEnvSample, 0.0f, 1.0f, freq, freqMax);
                filter.setCutoffFrequencyHz(cutOffFreqHz);
                filter.setResonance(res);
            }
        }

        // add voice output to main buffer
        juce::dsp::AudioBlock<float> output(outputBuffer);
        output.add(voiceBlock);
        
        // reset amp envelope if it's finished
        if (!envelope.isActive()) {
            envelope.reset();
        }
    }
}

void SineWaveVoice::prepare(const juce::dsp::ProcessSpec& spec)
{
    voiceBlock = juce::dsp::AudioBlock<float> (heapBlock, spec.numChannels, spec.maximumBlockSize);
    filter.prepare(spec);
    
    // initialize filter
    float freq = oscTree.getRawParameterValue("CUTOFF")->load();
    float res = oscTree.getRawParameterValue("RESONANCE")->load() / 10;
    filter.setCutoffFrequencyHz(freq);
    filter.setResonance(res);
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
        oscTree.getRawParameterValue("FILTER_ATTACK")->load() / 100,
        oscTree.getRawParameterValue("FILTER_DECAY")->load() / 100,
        oscTree.getRawParameterValue("FILTER_SUSTAIN")->load() / 100,
        oscTree.getRawParameterValue("FILTER_RELEASE")->load() / 100,
    };
    envelope.setParameters(envelopeParameters);
    filterEnvelope.setParameters(filterEnvelopeParameters);
}
