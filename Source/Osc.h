/*
  ==============================================================================

    Osc.h
    Created: 19 Oct 2020 10:13:11pm
    Author:  woz
    NOTES:  This code was adapted from Martin Finke's
            oscillator code available at:
            http://www.martin-finke.de/blog/articles/audio-plugins-008-synthesizing-waveforms

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include <math.h>

enum OscillatorMode {
    OSCILLATOR_MODE_SINE,
    OSCILLATOR_MODE_SAW,
    OSCILLATOR_MODE_SQUARE,
    OSCILLATOR_MODE_TRIANGLE,
    OSCILLATOR_MODE_NOISE,
};

class Oscillator {
private:
    double polyBlep(double);
    double lastOutput;
    double polyBlepPhase;
    juce::Random random;

public:
    void setMode(OscillatorMode mode);
    void setFrequency(double frequency);
    void setSampleRate(double sampleRate);
    void startNote();
    void generate(juce::dsp::AudioBlock<float>&, int nFrames, double gain);
    Oscillator() :
    mOscillatorMode(OSCILLATOR_MODE_SAW),
    mPI(2*acos(0.0)),
    twoPI(2 * mPI),
    mFrequency(440.0),
    lastOutput(0.0),
    mPhase(0.0) { updateIncrement(); }; // TODO: This could be an issue if the user or DAW is set to a different sample rate.
    
//    ~Oscillator();
protected:
    OscillatorMode mOscillatorMode;
//    double naiveWaveformForMode(OscillatorMode mode);
    const double mPI;
    const double twoPI;
    double mFrequency;
    double mPhase;
    double mSampleRate;
    double mPhaseIncrement;
    
    void updateIncrement();
};
