#include "SliderLookAndFeel.h"

SliderLookAndFeel::SliderLookAndFeel()
{
    // load binary image
    knobImage = juce::ImageCache::getFromMemory(BinaryData::knobImg_png, BinaryData::knobImg_pngSize);
    // set other colors
    setColour(juce::Slider::textBoxTextColourId, juce::Colours::black);
    
    if(!knobImage.isNull()) {
        // get number of frames and img measures
        knobImgWidth = knobImage.getWidth();
        knobImgHeight = knobImage.getHeight();
        frames = knobImgHeight / knobImgWidth;
    } else
        jassert(!knobImage.isNull());
}

void SliderLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                                          const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider)
{
    // angle rotation and corresponding frame in the KnobMan image
    angle = (slider.getValue() - slider.getMinimum()) / (slider.getMaximum() - slider.getMinimum());
    frameId = (int) ceil(angle * ((double) frames - 1.0));
    
    // calculate drawing position and diameter
    radius = juce::jmin(width / 2.0f, height / 2.0f);
    centerX = x + width * 0.5f;
    centerY = y + height * 0.5f;
    rx = centerX - radius;
    ry = centerY - radius;
    diameter = 2.0 * radius;
    
    // draw knob @ frameId
    g.setOpacity(1.0f);
    g.drawImage(knobImage, rx, ry, diameter, diameter, 0, frameId * knobImgWidth, knobImgWidth, knobImgWidth);
}
