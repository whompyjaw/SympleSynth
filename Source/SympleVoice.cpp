/*
  ==============================================================================

    SineWaveOsc.cpp
    Created: 18 Oct 2020 7:41:04pm
    Author:  whompyjaw

  ==============================================================================
*/

#include "SympleVoice.h"

SympleVoice::SympleVoice(juce::ADSR::Parameters& ampParameters) : ampParameters(ampParameters)
{
    amplifier.setSampleRate(getSampleRate());
    amplifier.setParameters(ampParameters);
}

bool SympleVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<SympleSound*> (sound) != nullptr; // Not sure what this does
}

/* Start the sine tone based on midi input converting the midi note number to a frequency */
void SympleVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int)
{
    amplifier.noteOn();
    currentAngle = 0.0;
    level = velocity * 0.15;

    auto cyclesPerSecond = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber); // convert midi note number to hertz
    auto cyclesPerSample = cyclesPerSecond / getSampleRate();

    angleDelta = cyclesPerSample * 2.0 * juce::MathConstants<double>::pi; // Creates the sone tone at that frequency
    // this is similar to 
    //mPhaseIncrement = mFrequency * 2 * mPI / mSampleRate;
    // angleDelta is the same as phase increment
}

/* Stops the voice by the owning synthesiser calling this function, which must be overriden*/
void SympleVoice::stopNote(float, bool allowTailOff)
{
    amplifier.noteOff();
}

/* Renders the next block of data for this voice, and while held down. */
void SympleVoice::renderNextBlock(juce::AudioSampleBuffer& outputBuffer, int startSample, int numSamples)
{
    if (angleDelta != 0.0) // Not silent
    {
        while (--numSamples >= 0) // note is being held down
        {
            auto currentSample = (float)(std::sin(currentAngle) * level * amplifier.getNextSample());

            for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
                outputBuffer.addSample(i, startSample, currentSample);

            currentAngle += angleDelta;
            ++startSample;

            if (!amplifier.isActive()) {
                clearCurrentNote();
                amplifier.reset();
                angleDelta = 0.0;
                break;
            }
        }
    }
}

void SympleVoice::setAmpParameters(juce::ADSR::Parameters& params)
{
    amplifier.setParameters(params);
}

SympleOscillator::SympleOscillator()
{
    auto& osc = processorChain.template get<oscIndex>();
    osc.initialise([](Type x)
        {
            return juce::jmap(x,
                Type(-juce::MathConstants<double>::pi),
                Type(juce::MathConstants<double>::pi),
                Type(-1),
                Type(1));
        }, 2);
}

/* prepares each process in the chain sequentially (calls reset for oscillator, for gain, etc */
void SympleOscillator::prepare(const juce::dsp::ProcessSpec& spec)
{
    processorChain.prepare(spec);   
}

/* Calls reset for each process in the chain sequentially */
void SympleOscillator::reset() noexcept
{
    processorChain.reset();
}

void SympleOscillator::setLevel(Type newValue)
{
    //example uses "processorChain.template get<oscIndex>(); but i feel like that is probably old
    auto& gain = processorChain.get<gainIndex>();
    
    gain.setGainDecibels(newValue);
}

void SympleOscillator::setFrequency(float frequency, bool force = false)
{
    auto& osc = processorChain.get<oscIndex>();
    osc.setFrequency(frequency, force);

}
