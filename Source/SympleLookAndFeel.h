/*
  ==============================================================================

    SympleLookAndFeel.h
    Created: 18 Nov 2020 8:22:57pm
    Author:  Nathan

    Used info from https://docs.juce.com/master/tutorial_look_and_feel_customisation.html

  ==============================================================================
*/

#pragma once

class SympleLookAndFeel : public juce::LookAndFeel_V4
{
public:
    SympleLookAndFeel()
    {
        //setColour(juce::Slider::thumbColourId, juce::Colours::red);
    }

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
        const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider&) override
    {
        // Calculation variables
        auto radius = (float)juce::jmin(width / 3.5, height / 3.5) - 3.0f;
        auto centreX = (float)x + (float)width * 0.5f;
        auto centreY = (float)y + (float)height * 0.5f;
        auto rx = centreX - radius;
        auto ry = centreY - radius;
        auto rw = radius * 2.0f;
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        // Fill
        g.setColour(juce::Colours::lightgrey);
        g.fillEllipse(rx, ry, rw, rw);

        // Calculate Pointer
        juce::Path p;
        auto pointerLength = radius * 0.75f;
        auto pointerThickness = 2.0f;
        p.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
        p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));

        // Draw Pointer
        g.setColour(juce::Colours::black);
        g.fillPath(p);

    }
};

class LabelLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawLabel(juce::Graphics& g, juce::Label& l) override
    {
        auto labelArea = l.getLocalBounds();
        auto margin = labelArea.getWidth() / 4;
        labelArea.removeFromLeft(margin);
        labelArea.removeFromRight(margin);
        auto cornerSize = juce::jmin(l.getHeight(), l.getWidth()) / 2.5;

        juce::Path p;
        p.addRoundedRectangle(labelArea, cornerSize, cornerSize);
        g.setColour(juce::Colours::lightgrey);
        g.fillPath(p);

        g.setFont(l.getFont());
        g.setColour(juce::Colours::black);
        g.drawFittedText(l.getText(), labelArea.reduced(5), juce::Justification::centred, 2);
    }
};