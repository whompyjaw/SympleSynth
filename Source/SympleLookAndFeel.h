/*
  ==============================================================================

    SympleLookAndFeel.h
    Created: 18 Nov 2020 8:22:57pm
    Author:  Nathan

  ==============================================================================
*/

#pragma once

class SympleLookAndFeel : public juce::LookAndFeel_V4
{
public:
    SympleLookAndFeel()
    {
        setColour(juce::Slider::thumbColourId, juce::Colours::red);
    }
};