/*
  ==============================================================================

    SineWaveOsc.cpp
    Created: 18 Oct 2020 7:41:04pm
    Author:  whompyjaw

  ==============================================================================
*/

#include "Voice.h"

SynthVoice::SynthVoice(juce::AudioProcessorValueTreeState& tree, juce::dsp::AudioBlock<float>& lfoBuffer)
    : oscTree(tree), lfoBuffer(lfoBuffer)
{
    readParameterState();

    osc1.setSampleRate(getSampleRate());
    osc2.setSampleRate(getSampleRate());

    // initialize amplifier envelope
    envelope.setSampleRate(getSampleRate());
    
    // intialize filter envelope
    filterEnvelope.setSampleRate(getSampleRate());
}

bool SynthVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<SynthSound*> (sound) != nullptr; // Not sure what this does
}

// Start the sine tone based on midi input

void SynthVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int)
{    
    // turn on envelopes
    envelope.noteOn();
    filterEnvelope.noteOn();
    
    readParameterState();

    // reset oscillator phase
    osc1.startNote();
    osc2.startNote();

    // calculate the frequency from the midi and the APVST
    int currentOctave1 = oscTree.getParameterAsValue("OSC_1_OCTAVE").getValue();
    int currentOctave2 = oscTree.getParameterAsValue("OSC_2_OCTAVE").getValue();

    int currentSemitone1 = oscTree.getParameterAsValue("OSC_1_SEMITONE").getValue();
    int currentSemitone2 = oscTree.getParameterAsValue("OSC_2_SEMITONE").getValue();

    // adjust the frequency with value from the fine tune knob
    float fineTune1 = oscTree.getParameterAsValue("OSC_1_FINE_TUNE").getValue();
    float fineTune2 = oscTree.getParameterAsValue("OSC_2_FINE_TUNE").getValue();

    auto hertz1 = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber + currentSemitone1);
    auto hertz2 = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber + currentSemitone2);

    // cents formula adapted from http://hyperphysics.phy-astr.gsu.edu/hbase/Music/cents.html
    auto pow1 = currentOctave1 + (fineTune1 / 1200);
    auto pow2 = currentOctave2 + (fineTune2 / 1200);

    auto adjustedFreq1 = 2 * pow(2, pow1) * hertz1;
    auto adjustedFreq2 = 2 * pow(2, pow2) * hertz2;

    osc1.setFrequency(adjustedFreq1);
    osc2.setFrequency(adjustedFreq2);
}

/* Stops the voice by the owning synthesiser calling this function, which must be overriden*/

void SynthVoice::stopNote(float, bool allowTailOff)
{
    // set envelopes to release stage
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
void SynthVoice::renderNextBlock(juce::AudioSampleBuffer& outputBuffer, int startSample, int numSamples)
{
    if (envelope.isActive())
    {
        // clear voice block for processing
        voiceBlock.clear();
        
        // init counters
        size_t updateCounter = PARAM_UPDATE_RATE;
        size_t read = startSample;
        
        // prepare filter
        float nextFilterEnvSample;
        nextFilterEnvSample = filterEnvelope.getNextSample();
        setFilter(read, nextFilterEnvSample);

        // TODO: This is where I think we could put all our processing so we can DRY our code. remove the loops in Oscillator::generate
//        for(int sample = startSample; sample < numSamples; sample++)
//        {
//            for(int channel = 0; channel < outputBuffer.getNumChannels(); channel++)
//            {
//
//            }
//
//        }
        
        // process every sample
        while (read < (startSample + numSamples)) {
            auto max = juce::jmin((size_t) (startSample + numSamples) - read, updateCounter);
            auto subBlock = voiceBlock.getSubBlock (read, max);

            // add oscillator 1 sound
            osc1ModeInt = oscTree.getParameterAsValue("OSC_1_WAVE_TYPE").getValue();
            double osc1Gain = oscTree.getParameterAsValue("OSC_1_GAIN").getValue();
            oscMode = static_cast<OscillatorMode> (osc1ModeInt);
            osc1.setMode(oscMode);
            osc1.generate(subBlock, (int) subBlock.getNumSamples(), osc1Gain);

            // add oscillator 2 sound
            osc2ModeInt = oscTree.getParameterAsValue("OSC_2_WAVE_TYPE").getValue();
            double osc2Gain = oscTree.getParameterAsValue("OSC_2_GAIN").getValue();
            oscMode = static_cast<OscillatorMode> (osc2ModeInt);
            osc2.setMode(oscMode);
            osc2.generate(subBlock, (int) subBlock.getNumSamples(), osc2Gain);

            // apply envelope
            applyEnvelope(subBlock);

            // filter sound
            filter.process(juce::dsp::ProcessContextReplacing<float>(subBlock));

            // set counters
            read += max;
            updateCounter -= max;

            // advance the filter envelope for the amount of processed samples
            // and keep the most recent setting
            while (max-- > 1)
                 filterEnvelope.getNextSample();
            nextFilterEnvSample = filterEnvelope.getNextSample();

            if (updateCounter == 0)
            {
                // reset the amount of samples to process
                updateCounter = PARAM_UPDATE_RATE;

                // update filter
                setFilter(read, nextFilterEnvSample);
            }
        }

        // add voice output to main buffer
        juce::dsp::AudioBlock<float> output(outputBuffer);
        output.add(voiceBlock);
        
        // reset amp envelope if it's finished
        if (!envelope.isActive()) {
            envelope.reset();
            filterEnvelope.reset();
            clearCurrentNote();
        }
    }
}

void SynthVoice::prepare(const juce::dsp::ProcessSpec& spec)
{
    voiceBlock = juce::dsp::AudioBlock<float> (heapBlock, spec.numChannels, spec.maximumBlockSize);
    filter.prepare(spec);
}

/*
 *  Reads the state of parameters in the value state tree and sets
 *  class variables that hold envelope, filter, and oscillator params
 */
void SynthVoice::readParameterState()
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
    envelope.setParameters(envelopeParameters);
    filterEnvelope.setParameters(filterEnvelopeParameters);
}

/*
 *  Applies the voice's envelope to a juce dsp audio block
 */
void SynthVoice::applyEnvelope(juce::dsp::AudioBlock<float>& subBlock)
{
    float env;
    for (int sample = 0; sample < subBlock.getNumSamples(); ++sample)
    {
        env = envelope.getNextSample();
        for (int channel = 0; channel < subBlock.getNumChannels(); ++channel)
        {
            subBlock.setSample(channel, sample, subBlock.getSample(channel, sample) * env);
        }
    }
}

void SynthVoice::setFilter(size_t read, float filterEnv)
{
    // get filter params from state tree
    float freq = oscTree.getRawParameterValue("CUTOFF")->load();
    float res = oscTree.getRawParameterValue("RESONANCE")->load() / 100;
    float amount = oscTree.getParameterAsValue("AMOUNT").getValue();
    float lfoAmount = oscTree.getParameterAsValue("LFO_AMOUNT").getValue();
    int lfoSample = (int) juce::jmax((int) read - 1, (int) 0);

    // calculate max cutoff from envelope
    // semitone calculations from
    // https://pages.mtu.edu/~suits/NoteFreqCalcs.html
    float freqMax = juce::jmin((float) (freq * pow(twelfthRoot, amount)), 20000.0f);
    float lfoFreqMax = juce::jmin((float) (freq * pow(twelfthRoot, lfoAmount)), 20000.0f);

    auto cutOffFreqHz = juce::jmap (filterEnv, 0.0f, 1.0f, freq, freqMax);
    float lfoCutoffFreqHz = juce::jmap (lfoBuffer.getSample(0, lfoSample), -1.0f, 1.0f, freq, lfoFreqMax);

    // reset filter values
    filterModeInt = oscTree.getParameterAsValue("FILTER_1_MODE").getValue();
    filterMode = static_cast<juce::dsp::LadderFilterMode> (filterModeInt);
    filter.setMode(filterMode);
    filter.setCutoffFrequencyHz(juce::jmax(cutOffFreqHz, lfoCutoffFreqHz));
    filter.setResonance(res);
}
