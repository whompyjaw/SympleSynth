/*
  ==============================================================================

    Filter.h
    Created: 29 Nov 2020 5:48:25pm
    Author:  woz

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class Filter : juce::dsp::LadderFilter<float>
{
public:
    Filter();
    ~Filter();
    void runSmoothers();
};
