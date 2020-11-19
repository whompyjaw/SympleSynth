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

void Oscillator::generate(juce::dsp::AudioBlock<float>& buffer, int numSamples, double gain)
{
    double waveSegment = 0.0;
    
    for (int sample = 0; sample < numSamples; sample++)
    {
        polyBlepPhase = mPhase / twoPI;
        switch (mOscillatorMode)
        {
        case OSCILLATOR_MODE_SAW:
            waveSegment = (2.0 * mPhase / twoPI) - 1.0; // naive wave
            waveSegment -= polyBlep(polyBlepPhase);
            waveSegment *= -1;
            break;
        case OSCILLATOR_MODE_SINE:
            waveSegment = sin(mPhase);
            break;
        case OSCILLATOR_MODE_SQUARE:
            if (mPhase < mPI)
            {
                waveSegment = 1.0;
            }
            else
            {
                waveSegment = -1.0;
            }
            waveSegment += polyBlep(polyBlepPhase);
            waveSegment -= polyBlep(fmod(polyBlepPhase + 0.5, 1.0));
            break;
        case OSCILLATOR_MODE_TRIANGLE:
            if (mPhase < mPI)
            {
                waveSegment = 1.0;
            }
            else
            {
                waveSegment = -1.0;
            }
            waveSegment += polyBlep(polyBlepPhase);
            waveSegment -= polyBlep(fmod(polyBlepPhase + 0.5, 1.0));
            // Leaky integrator: y[n] = A * x[n] + (1 - A) * y[n-1]
            waveSegment = mPhaseIncrement * waveSegment + (1 - mPhaseIncrement) * lastOutput;
            lastOutput = waveSegment;
                break;
        case OSCILLATOR_MODE_NOISE:
            waveSegment = random.nextFloat() * 0.25 - 0.125;
        }

        // copy the same sample into all channels for mono sound
        for (int channel = 0; channel < buffer.getNumChannels(); channel++)
        {
            buffer.addSample(channel, sample, (float)waveSegment * juce::Decibels::decibelsToGain(gain));
        }
            
        mPhase += mPhaseIncrement;
        while (mPhase >= twoPI)
        {
            mPhase -= twoPI;
        }
    }
}

double Oscillator::polyBlep(double t)
{
    double dt = mPhaseIncrement / twoPI;
    // 0 <= t < 1
    if (t < dt)
    {
        t /= dt;
        return t+t - t*t - 1.0;
    }
    // -1 < t < 0
    else if (t > 1.0 - dt)
    {
        t = (t - 1.0) / dt;
        return t*t + t+t + 1.0;
    }
    // 0 otherwise
    else return 0.0;
}
