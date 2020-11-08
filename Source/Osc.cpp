/*
  ==============================================================================

    Osc.cpp
    Created: 19 Oct 2020 10:13:17pm
    Author:  woz
    NOTES:  This code was adapted from Martin Finke's
            oscillator code available at:
            http://www.martin-finke.de/blog/articles/audio-plugins-008-synthesizing-waveforms
            The PolyBLEP (removing aliasing) feature was adapted from
            http://www.martin-finke.de/blog/articles/audio-plugins-018-polyblep-oscillator/

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

void Oscillator::generate(juce::dsp::AudioBlock<float>& buffer, int nFrames, juce::ADSR& amp)
{
    double value = 0.0;
    double t = mPhase / twoPI;
    
    if (mOscillatorMode == OSCILLATOR_MODE_SINE) {
        value = naiveWaveformForMode(OSCILLATOR_MODE_SINE);
    } else if (mOscillatorMode == OSCILLATOR_MODE_SAW) {
        value = naiveWaveformForMode(OSCILLATOR_MODE_SAW);
        value -= poly_blep(t);
    } else {
        value = naiveWaveformForMode(OSCILLATOR_MODE_SQUARE);
        value += poly_blep(t);
        value -= poly_blep(fmod(t + 0.5, 1.0));
        if (mOscillatorMode == OSCILLATOR_MODE_TRIANGLE) {
            // Leaky integrator: y[n] = A * x[n] + (1 - A) * y[n-1]
            value = mPhaseIncrement * value + (1 - mPhaseIncrement) * lastOutput;
            lastOutput = value;
        }
    }
    
    mPhase += mPhaseIncrement;
    while (mPhase >= twoPI) {
        mPhase -= twoPI;
    }
    return value;
    
//    switch (mOscillatorMode) {
//        case OSCILLATOR_MODE_SINE:
//            for (int i = 0; i < nFrames; i++) {
//                auto env = amp.getNextSample();
//                for (int j = 0; j < buffer.getNumChannels(); ++j) {
//                    buffer.addSample(j, i, (float) (sin(mPhase) * env));
//                }
//                mPhase += mPhaseIncrement;
//                while (mPhase >= twoPI) {
//                    mPhase -= twoPI;
//                }
//            }
//            break;
//        case OSCILLATOR_MODE_SAW:
//            for (int i = 0; i < nFrames; i++) {
//                auto env = amp.getNextSample();
//                for (int j = 0; j < buffer.getNumChannels(); ++j) {
//                    buffer.addSample(j, i,(float) (1.0 - (2.0 * mPhase / twoPI)) * env);
//                }
//                mPhase += mPhaseIncrement;
//                while (mPhase >= twoPI) {
//                    mPhase -= twoPI;
//                }
//            }
//            break;
//        case OSCILLATOR_MODE_SQUARE:
//            for (int i = 0; i < nFrames; i++) {
//                auto env = amp.getNextSample();
//                if (mPhase <= mPI) {
//                    for (int j = 0; j < buffer.getNumChannels(); ++j) {
//                        buffer.addSample(j, i, (float) 1.0 * env);
//                    }
//                } else {
//                    for (int j = 0; j < buffer.getNumChannels(); ++j) {
//                        buffer.addSample(j, i, (float) -1.0 * env);
//                    }
//                }
//                mPhase += mPhaseIncrement;
//                while (mPhase >= twoPI) {
//                    mPhase -= twoPI;
//                }
//            }
//            break;
//        case OSCILLATOR_MODE_TRIANGLE:
//            for (int i = 0; i < nFrames; i++) {
//                auto env = amp.getNextSample();
//                double value = -1.0 + (2.0 * mPhase / twoPI);
//                for (int j = 0; j < buffer.getNumChannels(); ++j) {
//                    buffer.addSample(j, i, (float)(2.0 * (fabs(value) - 0.5)) * env);
//                }
//                mPhase += mPhaseIncrement;
//                while (mPhase >= twoPI) {
//                    mPhase -= twoPI;
//                }
//            }
//            break;
//    }
}

double Oscillator::naiveWaveformForMode(OscillatorMode mode) {
    double value;
    switch (mode) {
        case OSCILLATOR_MODE_SINE:
            value = sin(mPhase);
            break;
        case OSCILLATOR_MODE_SAW:
            value = (2.0 * mPhase / twoPI) - 1.0;
            break;
        case OSCILLATOR_MODE_SQUARE:
            if (mPhase < mPI) {
                value = 1.0;
            } else {
                value = -1.0;
            }
            break;
        case OSCILLATOR_MODE_TRIANGLE:
            value = -1.0 + (2.0 * mPhase / twoPI);
            value = 2.0 * (fabs(value) - 0.5);
            break;
        default:
            break;
    }
    return value;
}

//double PolyBLEPOscillator::poly_blep(double t)
//{
//    double dt = mPhaseIncrement / twoPI;
//    if (t < dt) // this is at beginning of wave: 0 <= t < 1
//    {
//        t /= dt;
//        return t+t - t*t - 1.0;
//    } // at end of wave: -1 < t < 0
//    else if (t > 1.0 - dt)
//    {
//        t = (t - 1.0) / dt;
//        return t*t + t+t + 1.0;
//    }
//    else
//        return 0.0;
//}
//
//double PolyBLEPOscillator::nextSample()
//{
//    double value = 0.0;
//    double t = mPhase / twoPI;
//    
//    if (mOscillatorMode == OSCILLATOR_MODE_SINE) {
//        value = naiveWaveformForMode(OSCILLATOR_MODE_SINE);
//    } else if (mOscillatorMode == OSCILLATOR_MODE_SAW) {
//        value = naiveWaveformForMode(OSCILLATOR_MODE_SAW);
//        value -= poly_blep(t);
//    } else {
//        value = naiveWaveformForMode(OSCILLATOR_MODE_SQUARE);
//        value += poly_blep(t);
//        value -= poly_blep(fmod(t + 0.5, 1.0));
//        if (mOscillatorMode == OSCILLATOR_MODE_TRIANGLE) {
//            // Leaky integrator: y[n] = A * x[n] + (1 - A) * y[n-1]
//            value = mPhaseIncrement * value + (1 - mPhaseIncrement) * lastOutput;
//            lastOutput = value;
//        }
//    }
//    
//    mPhase += mPhaseIncrement;
//    while (mPhase >= twoPI) {
//        mPhase -= twoPI;
//    }
//    return value;
//}
