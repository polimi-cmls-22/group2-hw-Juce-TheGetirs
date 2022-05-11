#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "SliderLookAndFeel.h"

//==============================================================================

class WahgliuAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    WahgliuAudioProcessorEditor (WahgliuAudioProcessor&);
    ~WahgliuAudioProcessorEditor() override;
    
    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
private:
    juce::ImageComponent backgroundImageComp;
    
    SliderLookAndFeel sliderStyle;
    
    // first row wah-wah
    juce::Slider frequencySlider;
    juce::Label frequencyLabel;
    juce::Slider qualitySlider;
    juce::Label qualityLabel;
    
    // first row wah-wah
    juce::Slider speedSlider;
    juce::Label speedLabel;
    juce::Slider sensitivitySlider;
    juce::Label sensitivityLabel;
    
    // second row
    juce::Slider distortionGainSlider;
    juce::Label distortionGainLabel;
    juce::Slider mixSlider;
    juce::Label mixLabel;
    
    // third row
    juce::Slider volumeSlider;
    juce::Label volumeLabel;
    juce::TextButton wahModeSelectionButton;
    juce::TextButton bypassButton;
    
    // apvts attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> freqSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> qSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> speedSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sensSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> volumeSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> bypassAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> wahModeSelectionAttachment;
    
    WahgliuAudioProcessor& audioProcessor;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WahgliuAudioProcessorEditor)
    
    //==============================================================================
    void toggleWahMode(bool isWahWah);
};
