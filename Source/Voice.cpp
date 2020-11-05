/*
  ==============================================================================

    SineWaveOsc.cpp
    Created: 18 Oct 2020 7:41:04pm
    Author:  whompyjaw

  ==============================================================================
*/

#include "Voice.h"

SynthVoice::SynthVoice(juce::AudioProcessorValueTreeState& tree)
    : oscTree(tree)
{
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
    
    // reset oscillator phase
    osc1.startNote();
    osc2.startNote();
    
    // reduce note amplitude
    level = velocity * 0.15;

    // calculate the frequency from the midi and the APVST
    int currentOctave1 = oscTree.getParameterAsValue("OSC_1_OCTAVE").getValue();
    int currentOctave2 = oscTree.getParameterAsValue("OSC_2_OCTAVE").getValue();

    auto twelthRoot = pow(2, (1 / 12));

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
    OscillatorMode oscMode;
    if (envelope.isActive())
    {
        // clear voice block for processing
        voiceBlock.clear();
        
        // init counters
        size_t updateCounter = PARAM_UPDATE_RATE;
        size_t read = startSample;
        
        // process every sample
        while (read < (startSample + numSamples)) {
            auto max = juce::jmin((size_t) (startSample + numSamples) - read, updateCounter);
            auto subBlock = voiceBlock.getSubBlock (read, max);

            // add oscillator 1 sound
            osc1ModeInt = oscTree.getParameterAsValue("OSC_1_WAVE_TYPE").getValue();
            oscMode = static_cast<OscillatorMode> (osc1ModeInt);
            osc1.setMode(oscMode);
            osc1.generate(subBlock, (int) subBlock.getNumSamples(), envelope);

            // add oscillator 2 sound
            osc2ModeInt = oscTree.getParameterAsValue("OSC_2_WAVE_TYPE").getValue();
            oscMode = static_cast<OscillatorMode> (osc2ModeInt);
            osc2.setMode(oscMode);
            osc2.generate(subBlock, (int)subBlock.getNumSamples(), envelope);

            // filter sound
            filter.process(juce::dsp::ProcessContextReplacing<float>(subBlock));

            // set counters
            read += max;
            updateCounter -= max;
            float nextFilterEnvSample = filterEnvelope.getNextSample();

            if (updateCounter == 0)
            {
                // reset the amount of samples to process
                updateCounter = PARAM_UPDATE_RATE;

                // get filter params from state tree
                float freq = oscTree.getRawParameterValue("CUTOFF")->load();
                float res = oscTree.getRawParameterValue("RESONANCE")->load() / 100;
                float amount = oscTree.getRawParameterValue("AMOUNT")->load() / 100;

                // calculate max cutoff from envelope
                float freqMax = freq + ((20000.0f - freq) * amount);
                auto cutOffFreqHz = juce::jmap (nextFilterEnvSample, 0.0f, 1.0f, freq, freqMax);

                // reset filter values
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

void SynthVoice::prepare(const juce::dsp::ProcessSpec& spec)
{
    voiceBlock = juce::dsp::AudioBlock<float> (heapBlock, spec.numChannels, spec.maximumBlockSize);
    filter.prepare(spec);
    
    // initialize filter
    float freq = oscTree.getRawParameterValue("CUTOFF")->load();
    float res = oscTree.getRawParameterValue("RESONANCE")->load() / 10;
    filter.setCutoffFrequencyHz(freq);
    filter.setResonance(res);
}
