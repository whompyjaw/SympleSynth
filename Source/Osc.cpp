/*
  ==============================================================================

    Osc.cpp
    Created: 19 Oct 2020 10:13:17pm
    Author:  woz
    NOTES:  This code was adapted from Martin Finke's
            oscillator code available at:
            http://www.martin-finke.de/blog/articles/audio-plugins-008-synthesizing-waveforms

  ==============================================================================
*/

#include <JuceHeader.h>
#include <math.h>
#include "Osc.h"


void Oscillator::setMode(OscillatorMode mode) {
    mOscillatorMode = mode;
}

void Oscillator::setFrequency(double frequency) {
    mFrequency = frequency;
    updateIncrement();
}

void Oscillator::setSampleRate(double sampleRate) {
    mSampleRate = sampleRate;
    updateIncrement();
}

void Oscillator::updateIncrement() {
    mPhaseIncrement = mFrequency * 2 * mPI / mSampleRate;
}

void Oscillator::startNote() {
    mPhase = 0.0;
}

void Oscillator::generate(juce::dsp::AudioBlock<float>& buffer, int nFrames)
{
    const double twoPI = 2 * mPI;
    switch (mOscillatorMode) {
        case OSCILLATOR_MODE_SINE:
            for (int i = 0; i < nFrames; i++) {
                for (int j = 0; j < buffer.getNumChannels(); ++j) {
                    buffer.addSample(j, i, (float) (sin(mPhase)));
                }
                mPhase += mPhaseIncrement;
                while (mPhase >= twoPI) {
                    mPhase -= twoPI;
                }
            }
            break;
        case OSCILLATOR_MODE_SAW:
            for (int i = 0; i < nFrames; i++) {
                for (int j = 0; j < buffer.getNumChannels(); ++j) {
                    buffer.addSample(j, i,(float) (1.0 - (2.0 * mPhase / twoPI)));
                }
                mPhase += mPhaseIncrement;
                while (mPhase >= twoPI) {
                    mPhase -= twoPI;
                }
            }
            break;
        case OSCILLATOR_MODE_SQUARE:
            for (int i = 0; i < nFrames; i++) {
                if (mPhase <= mPI) {
                    for (int j = 0; j < buffer.getNumChannels(); ++j) {
                        buffer.addSample(j, i, (float) 1.0);
                    }
                } else {
                    for (int j = 0; j < buffer.getNumChannels(); ++j) {
                        buffer.addSample(j, i, (float) -1.0);
                    }
                }
                mPhase += mPhaseIncrement;
                while (mPhase >= twoPI) {
                    mPhase -= twoPI;
                }
            }
            break;
        case OSCILLATOR_MODE_TRIANGLE:
            for (int i = 0; i < nFrames; i++) {
                double value = -1.0 + (2.0 * mPhase / twoPI);
                for (int j = 0; j < buffer.getNumChannels(); ++j) {
                    buffer.addSample(j, i, (float) (2.0 * (fabs(value) - 0.5)));
                }
                mPhase += mPhaseIncrement;
                while (mPhase >= twoPI) {
                    mPhase -= twoPI;
                }
            }
            break;
    }
}
