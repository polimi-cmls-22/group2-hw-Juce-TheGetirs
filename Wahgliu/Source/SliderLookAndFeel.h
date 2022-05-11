#pragma once
#include <JuceHeader.h>

class SliderLookAndFeel : public juce::LookAndFeel_V4
{
public:
    SliderLookAndFeel();
    
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider) override;
    
private:
    juce::Image knobImage;
    double angle;
    int knobImgWidth, knobImgHeight;
    int frames, frameId;
    float radius, centerX, centerY, rx, ry, diameter;
};
