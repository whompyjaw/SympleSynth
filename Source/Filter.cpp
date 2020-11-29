/*
  ==============================================================================

    Filter.cpp
    Created: 29 Nov 2020 5:48:33pm
    Author:  woz
    Description: class override to call a protected function in
                 juce::dsp::ladderfilter

  ==============================================================================
*/

#include "Filter.h"

Filter::Filter()
{
}

Filter::~Filter()
{
}

void Filter::runSmoothers()
{
    this->updateSmoothers();
}
