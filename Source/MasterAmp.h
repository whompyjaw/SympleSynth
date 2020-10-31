/*
  ==============================================================================

    MasterAmp.h
    Created: 30 Oct 2020 6:18:34pm
    Author:  Nathan

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Envelope.h"

//==============================================================================
/*
*/
class MasterAmp  : public juce::Component
{
public:
    MasterAmp(SympleSynthAudioProcessor&);
    ~MasterAmp();

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    SympleADSRComponent amplifier;
    juce::Label ampLabel;

    SympleSynthAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MasterAmp)
};
