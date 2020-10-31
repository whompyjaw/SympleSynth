/*
  ==============================================================================

    MasterAmp.cpp
    Created: 30 Oct 2020 6:18:34pm
    Author:  Nathan

  ==============================================================================
*/

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "MasterAmp.h"

//==============================================================================
MasterAmp::MasterAmp(SympleSynthAudioProcessor& p) 
    :   audioProcessor(p),
        amplifier(p)
{
    setSize(400, 200);

    // init amp parameter names struct
    SympleADSRParameterNames ampParameters;
    ampParameters.attack = "AMP_ATTACK";
    ampParameters.decay = "AMP_DECAY";
    ampParameters.sustain = "AMP_SUSTAIN";
    ampParameters.release = "AMP_RELEASE";
    amplifier.setParameters(ampParameters);

    addAndMakeVisible(amplifier);

    addAndMakeVisible(ampLabel);
    ampLabel.setText("Amplifier", juce::dontSendNotification);
    ampLabel.setJustificationType(juce::Justification::centred);
    ampLabel.attachToComponent(&amplifier, false);

    // Master Slider
    addAndMakeVisible(masterGainSlider);
    masterGainSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    masterGainSlider.setRange(-60.0f, 0.0f, 0.1f);
    //masterGainSlider.setValue(-20.0f);
    masterGainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 30);
    masterGainSlider.setTextValueSuffix(" db");

    //masterGainSlider.addListener(this); // Make sure this is set or the slider won't work.
}

MasterAmp::~MasterAmp()
{
    masterGainValue.reset();
}

void MasterAmp::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
}

void MasterAmp::resized()
{
    juce::Rectangle<int> area(0, 0, getWidth(), getHeight());
    auto margin = 5;
    auto labelMargin = 30;
    auto ampHeight = getHeight() / 2;

    auto ampArea = area.removeFromBottom(ampHeight).reduced(margin);

    masterGainSlider.setBounds(area.removeFromBottom(ampHeight).reduced(margin));
    amplifier.setBounds(ampArea.getX(), ampArea.getY() + labelMargin, ampArea.getWidth(), 100);
}

void MasterAmp::setParameters(MasterAmpParameterNames& params) 
{
    masterGainValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getTree(), params.gain, masterGainSlider);
}
