/*
  ==============================================================================

    WaveSelectSlider.h
    Created: 14 Nov 2020 11:04:17am
    Author:  woz

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class WaveSelectSlider : public juce::Slider
{
public:
    WaveSelectSlider() {};
    ~WaveSelectSlider() {};
    juce::String getTextFromValue(double value)
    {
        int mode = (int) value;
        switch (mode)
        {
            case 0:
                return juce::String("Sine");
            case 1:
                return juce::String("Saw");
            case 2:
                return juce::String("Square");
            case 3:
                return juce::String("Triangle");
            default:
                return juce::String("Wave");
        }
    };
};
