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
        juce::Logger::writeToLog("Rendering Next Block");
        // clear voice block for processing
        voiceBlock.clear();
        
        // init counters
        size_t updateCounter = PARAM_UPDATE_RATE;
        size_t read = startSample;
        static_cast<size_t> (startSample);
        static_cast<size_t> (numSamples);
        
        // prepare filter
        float nextFilterEnvSample;
        nextFilterEnvSample = filterEnvelope.getNextSample();
        setFilter(read, nextFilterEnvSample);

        
        // process every sample
        while (read < (startSample + numSamples)) {
            
            auto max = juce::jmin((size_t) (startSample + numSamples) - read, updateCounter);
            auto subBlock = voiceBlock.getSubBlock (read, max);
            juce::Logger::writeToLog(readString + static_cast<juce::String> (read));
            juce::Logger::writeToLog(maxString + static_cast<juce::String> (max));

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
            filter1.process(juce::dsp::ProcessContextReplacing<float>(subBlock));
            filter2.process(juce::dsp::ProcessContextReplacing<float>(subBlock));

            // set counters
            read += max;
            updateCounter -= max;

            // advance the filter envelope for the amount of processed samples
            // and keep the most recent setting
            while (max-- > 1)
                 filterEnvelope.getNextSample();
            nextFilterEnvSample = filterEnvelope.getNextSample();

            juce::Logger::writeToLog("updateCounter: " + static_cast<juce::String> (updateCounter));
            if (updateCounter == 0)
            {
                // reset the amount of samples to process
                updateCounter = PARAM_UPDATE_RATE;

                // update filter
                setFilter(read, nextFilterEnvSample);
            }
        }
        juce::Logger::writeToLog(readString + "(after while loop) " + static_cast<juce::String> (read));

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
    filter1.prepare(spec);
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
        oscTree.getRawParameterValue("FILTER_1_ATTACK")->load(),
        oscTree.getRawParameterValue("FILTER_1_DECAY")->load(),
        oscTree.getRawParameterValue("FILTER_1_SUSTAIN")->load() / 100,
        oscTree.getRawParameterValue("FILTER_1_RELEASE")->load(),
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

    //lfoSample = (int)juce::jmax((int)read - 1, (int)0);

    //// calculate max cutoff from envelope
    //// semitone calculations from
    //// https://pages.mtu.edu/~suits/NoteFreqCalcs.html
    //freqMax = juce::jmin((float)(freq * pow(twelfthRoot, amount)), 20000.0f);
    //lfoFreqMax = juce::jmin((float)(freq * pow(twelfthRoot, lfoAmount)), 20000.0f);

    //auto cutOffFreqHz = juce::jmap(filterEnv, 0.0f, 1.0f, freq, freqMax);
    //lfoCutoffFreqHz = juce::jmap(lfoBuffer.getSample(0, lfoSample), -1.0f, 1.0f, freq, lfoFreqMax);
    //// get filter params from state tree for filter 1
    //freq = oscTree.getRawParameterValue("FILTER_1_CUTOFF")->load();
    //res = oscTree.getRawParameterValue("FILTER_1_RESONANCE")->load() / 100;
    //amount = oscTree.getParameterAsValue("FILTER_1_AMOUNT").getValue();
    //lfoAmount = oscTree.getParameterAsValue("LFO_AMOUNT").getValue();

    //// set the filter 1 values
    //filterModeInt = oscTree.getParameterAsValue("FILTER_1_MODE").getValue();
    //filterMode = static_cast<juce::dsp::LadderFilterMode> (filterModeInt);
    //filter1.setMode(filterMode);
    //filter1.setCutoffFrequencyHz(juce::jmax(cutOffFreqHz, lfoCutoffFreqHz));
    //filter1.setResonance(res);

    // SET FILTER 2
    //freq = oscTree.getRawParameterValue("FILTER_2_CUTOFF")->load();
    //res = oscTree.getRawParameterValue("FILTER_2_RESONANCE")->load() / 100;
    //amount = oscTree.getParameterAsValue("FILTER_2_AMOUNT").getValue();
    //cutOffFreqHz = juce::jmap(filter2Env, 0.0f, 1.0f, freq, freqMax);
    //filterModeInt = oscTree.getParameterAsValue("FILTER_2_MODE").getValue();
    //filterMode = static_cast<juce::dsp::LadderFilterMode> (filterModeInt);
    //filter2.setMode(filterMode);
    //filter2.setCutoffFrequencyHz(juce::jmax(cutOffFreqHz, lfoCutoffFreqHz));
    //filter2.setResonance(res);
}
