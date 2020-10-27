/*
  ==============================================================================

    Amplifier.h
    Created: 26 Oct 2020 8:23:58pm
    Author:  Whompy Jaw

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class Amplifier  : public juce::Component
{
public:
    Amplifier();
    ~Amplifier() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Amplifier)
};
