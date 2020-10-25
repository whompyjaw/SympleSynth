/*
  ==============================================================================

    Osc.cpp
    Created: 19 Oct 2020 10:13:17pm
    Author:  woz

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

void Oscillator::generate(juce::AudioSampleBuffer& buffer, int nFrames, juce::ADSR& amp) {
    const double twoPI = 2 * mPI;
    switch (mOscillatorMode) {
        case OSCILLATOR_MODE_SINE:
            for (int i = 0; i < nFrames; i++) {
                auto env = amp.getNextSample();
                for (int j = 0; j < buffer.getNumChannels(); ++j) {
                    buffer.addSample(j, i, (float) (sin(mPhase) * env));
                }
                mPhase += mPhaseIncrement;
                while (mPhase >= twoPI) {
                    mPhase -= twoPI;
                }
            }
            break;
        case OSCILLATOR_MODE_SAW:
            for (int i = 0; i < nFrames; i++) {
                auto env = amp.getNextSample();
                for (int j = 0; j < buffer.getNumChannels(); ++j) {
                    buffer.addSample(j, i,(float) (1.0 - (2.0 * mPhase / twoPI) * env));
                }
                mPhase += mPhaseIncrement;
                while (mPhase >= twoPI) {
                    mPhase -= twoPI;
                }
            }
            break;
        case OSCILLATOR_MODE_SQUARE:
            for (int i = 0; i < nFrames; i++) {
                auto env = amp.getNextSample();
                if (mPhase <= mPI) {
                    for (int j = 0; j < buffer.getNumChannels(); ++j) {
                        buffer.addSample(j, i, (float) 1.0 * env);
                    }
                } else {
                    for (int j = 0; j < buffer.getNumChannels(); ++j) {
                        buffer.addSample(j, i, (float) -1.0 * env);
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
                auto env = amp.getNextSample();
                double value = -1.0 + (2.0 * mPhase / twoPI);
                for (int j = 0; j < buffer.getNumChannels(); ++j) {
                    buffer.addSample(j, i, (float)(2.0 * (fabs(value) - 0.5)) * env);
                }
                mPhase += mPhaseIncrement;
                while (mPhase >= twoPI) {
                    mPhase -= twoPI;
                }
            }
            break;
    }
}
