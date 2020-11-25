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
    noiseOsc.setSampleRate(getSampleRate());
    noiseOsc.setMode(OSCILLATOR_MODE_NOISE); // always set to noise

    // initialize amplifier envelope
    ampEnvelope.setSampleRate(getSampleRate());
    
    // intialize filter envelope
    filterEnvelope.setSampleRate(getSampleRate());
    filter2Envelope.setSampleRate(getSampleRate());
}

bool SynthVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<SynthSound*> (sound) != nullptr; // Not sure what this does
}

// Start the sine tone based on midi input

void SynthVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int)
{    
    // turn on envelopes
    ampEnvelope.noteOn();
    filterEnvelope.noteOn();
    filter2Envelope.noteOn();
    
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
    float fineTune1 = oscTree.getRawParameterValue("OSC_1_FINE_TUNE")->load();
    float fineTune2 = oscTree.getRawParameterValue("OSC_2_FINE_TUNE")->load();

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
    ampEnvelope.noteOff();
    filterEnvelope.noteOff();
    filter2Envelope.noteOff();
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
    
    if (ampEnvelope.isActive())
    {
        // clear voice block for processing
        voice1Block.clear();
        voice2Block.clear();
        
        // init counters
        size_t updateCounter = PARAM_UPDATE_RATE;
        size_t read = startSample;
        
        // add oscillator 1 sound
        osc1ModeInt = oscTree.getParameterAsValue("OSC_1_WAVE_TYPE").getValue();
        oscMode = static_cast<OscillatorMode> (osc1ModeInt);
        osc1.setMode(oscMode);

        // add oscillator 2 sound
        osc2ModeInt = oscTree.getParameterAsValue("OSC_2_WAVE_TYPE").getValue();
        oscMode = static_cast<OscillatorMode> (osc2ModeInt);
        osc2.setMode(oscMode);

        // prepare filter
        float nextFilterEnvSample;
        nextFilterEnvSample = filterEnvelope.getNextSample();
        float nextFilter2EnvSample;
        nextFilter2EnvSample = filter2Envelope.getNextSample();
        // get sample for second filter
        setFilter(read, nextFilterEnvSample, nextFilter2EnvSample);

        
        // process every sample
        while (read < (startSample + numSamples)) {
            
            auto max = juce::jmin((size_t) (startSample + numSamples) - read, updateCounter);
            auto subBlock1 = voice1Block.getSubBlock (read, max);
            auto subBlock2 = voice2Block.getSubBlock (read, max);
     
            float osc1Gain = oscTree.getRawParameterValue("OSC_1_GAIN")->load();
            osc1.generate(subBlock1, (int) subBlock1.getNumSamples(), osc1Gain);

            float osc2Gain = oscTree.getRawParameterValue("OSC_2_GAIN")->load();
            osc2.generate(subBlock2, (int) subBlock2.getNumSamples(), osc2Gain);
            
            // add noise osc sound
            float noiseGain1 = oscTree.getRawParameterValue("NOISE_1_GAIN")->load();
            float noiseGain2 = oscTree.getRawParameterValue("NOISE_2_GAIN")->load();
            noiseOsc.generate(subBlock1, (int) subBlock1.getNumSamples(), noiseGain1);
            noiseOsc.generate(subBlock2, (int) subBlock2.getNumSamples(), noiseGain2);

            // apply envelope
            applyAmpEnvelope(subBlock1, subBlock2);

            // filter sound
            filter1.process(juce::dsp::ProcessContextReplacing<float>(subBlock1));
            filter2.process(juce::dsp::ProcessContextReplacing<float>(subBlock2));

            // set counters
            read += max;
            updateCounter -= max;

            // advance the filter envelope for the amount of processed samples
            // and keep the most recent setting
            while (max-- > 1)
            {
                filterEnvelope.getNextSample();
                filter2Envelope.getNextSample();
            }
            
            nextFilterEnvSample = filterEnvelope.getNextSample();
            nextFilter2EnvSample = filter2Envelope.getNextSample();

            //juce::Logger::writeToLog("updateCounter: " + static_cast<juce::String> (updateCounter));
            if (updateCounter == 0)
            {
                // reset the amount of samples to process
                updateCounter = PARAM_UPDATE_RATE;

                // update filter
                setFilter(read, nextFilterEnvSample, nextFilter2EnvSample);
            }
            
        }
        //juce::Logger::writeToLog(readString + "(after while loop) " + static_cast<juce::String> (read));

        // add voice output to main buffer
        juce::dsp::AudioBlock<float> output(outputBuffer);
        output.add(voice1Block);
        output.add(voice2Block);
        
        // reset amp envelope if it's finished
        if (!ampEnvelope.isActive()) {
            ampEnvelope.reset();
            filterEnvelope.reset();
            filter2Envelope.reset();
            clearCurrentNote();
        }
    }
}

void SynthVoice::prepare(const juce::dsp::ProcessSpec& spec)
{
    voice1Block = juce::dsp::AudioBlock<float> (heap1Block, spec.numChannels, spec.maximumBlockSize);
    voice2Block = juce::dsp::AudioBlock<float> (heap2Block, spec.numChannels, spec.maximumBlockSize);
    filter1.prepare(spec);
    filter2.prepare(spec);
}

/*
 *  Reads the state of parameters in the value state tree and sets
 *  class variables that hold envelope, filter, and oscillator params
 */
void SynthVoice::readParameterState()
{
    ampEnvelopeParameters = {
        oscTree.getRawParameterValue("AMP_ATTACK")->load(),
        oscTree.getRawParameterValue("AMP_DECAY")->load(),
        oscTree.getRawParameterValue("AMP_SUSTAIN")->load() / 100,
        oscTree.getRawParameterValue("AMP_RELEASE")->load()
    };
    ampEnvelope.setParameters(ampEnvelopeParameters);

    filterEnvelopeParameters = {
        oscTree.getRawParameterValue("FILTER_1_ATTACK")->load(),
        oscTree.getRawParameterValue("FILTER_1_DECAY")->load(),
        oscTree.getRawParameterValue("FILTER_1_SUSTAIN")->load() / 100,
        oscTree.getRawParameterValue("FILTER_1_RELEASE")->load(),
    };
    filterEnvelope.setParameters(filterEnvelopeParameters);

    filter2EnvelopeParameters = {
        oscTree.getRawParameterValue("FILTER_2_ATTACK")->load(),
        oscTree.getRawParameterValue("FILTER_2_DECAY")->load(),
        oscTree.getRawParameterValue("FILTER_2_SUSTAIN")->load() / 100,
        oscTree.getRawParameterValue("FILTER_2_RELEASE")->load(),
    };
    filter2Envelope.setParameters(filter2EnvelopeParameters);
}

/*
 *  Applies the voice's envelope to a juce dsp audio block
 */
void SynthVoice::applyAmpEnvelope(juce::dsp::AudioBlock<float>& subBlock1, juce::dsp::AudioBlock<float>& subBlock2)
{
    float env;
    for (int sample = 0; sample < subBlock1.getNumSamples(); ++sample)
    {
        env = ampEnvelope.getNextSample();
        for (int channel = 0; channel < subBlock1.getNumChannels(); ++channel)
        {
            subBlock1.setSample(channel, sample, subBlock1.getSample(channel, sample) * env);
            subBlock2.setSample(channel, sample, subBlock2.getSample(channel, sample) * env);
        }
    }
}

void SynthVoice::setFilter(size_t read, float filterEnv, float filter2EnvSample)
{
    freq = oscTree.getRawParameterValue("FILTER_1_CUTOFF")->load();
    res = oscTree.getRawParameterValue("FILTER_1_RESONANCE")->load() / 100;
    amount = oscTree.getRawParameterValue("FILTER_1_AMOUNT")->load();
    lfoAmount = oscTree.getRawParameterValue("LFO_AMOUNT")->load();

    lfoSample = (int)juce::jmax((int)read - 1, (int)0);

    // calculate max cutoff from envelope
    // semitone calculations from
    // https://pages.mtu.edu/~suits/NoteFreqCalcs.html
    freqMax = juce::jmin((float)(freq * pow(twelfthRoot, amount)), 20000.0f);
    lfoFreqMax = juce::jmin((float)(freq * pow(twelfthRoot, lfoAmount)), 20000.0f);

    auto cutOffFreqHz = juce::jmap(filterEnv, 0.0f, 1.0f, freq, freqMax);
    lfoCutoffFreqHz = juce::jmap(lfoBuffer.getSample(0, lfoSample), -1.0f, 1.0f, freq, lfoFreqMax);
    // get filter params from state tree for filter 1


    // set the filter 1 values
    filterModeInt = oscTree.getParameterAsValue("FILTER_1_MODE").getValue();
    filterMode = static_cast<juce::dsp::LadderFilterMode> (filterModeInt);
    filter1.setMode(filterMode);
    filter1.setCutoffFrequencyHz(juce::jmax(cutOffFreqHz, lfoCutoffFreqHz));
    filter1.setResonance(res);

    // SET FILTER 2
    freq = oscTree.getRawParameterValue("FILTER_2_CUTOFF")->load();
    res = oscTree.getRawParameterValue("FILTER_2_RESONANCE")->load() / 100;
    amount = oscTree.getRawParameterValue("FILTER_2_AMOUNT")->load();
    
    freqMax = juce::jmin((float)(freq * pow(twelfthRoot, amount)), 20000.0f);
    lfoFreqMax = juce::jmin((float)(freq * pow(twelfthRoot, lfoAmount)), 20000.0f);
    cutOffFreqHz = juce::jmap(filter2EnvSample, 0.0f, 1.0f, freq, freqMax);
    lfoCutoffFreqHz = juce::jmap(lfoBuffer.getSample(0, lfoSample), -1.0f, 1.0f, freq, lfoFreqMax);
    
    // set filter 2 values
    filterModeInt = oscTree.getParameterAsValue("FILTER_2_MODE").getValue();
    filterMode = static_cast<juce::dsp::LadderFilterMode> (filterModeInt);
    filter2.setMode(filterMode);
    filter2.setCutoffFrequencyHz(juce::jmax(cutOffFreqHz, lfoCutoffFreqHz));
    filter2.setResonance(res);
}
