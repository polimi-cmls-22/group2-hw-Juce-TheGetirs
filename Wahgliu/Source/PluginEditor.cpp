#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "SliderLookAndFeel.h"

//==============================================================================
WahgliuAudioProcessorEditor::WahgliuAudioProcessorEditor (WahgliuAudioProcessor& p)
: AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (350, 550);
    
    auto backGrndImage = juce::ImageCache::getFromMemory(BinaryData::background_png, BinaryData::background_pngSize);
    if(!backGrndImage.isNull())
        backgroundImageComp.setImage(backGrndImage);
    else
        jassert(!backGrndImage.isNull());
    
    addAndMakeVisible(backgroundImageComp);
    
    juce::LookAndFeel::setDefaultLookAndFeel(&sliderStyle);
    
    // first row wah-wah
    qualitySlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    qualitySlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 70, 20);
    qualitySlider.setRange(0.5, 3.0, 0.0);
    addAndMakeVisible(qualitySlider);
    
    qualityLabel.setText("Q factor", juce::dontSendNotification);
    qualityLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    qualityLabel.setFont(18.0f);
    qualityLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(qualityLabel);
    
    frequencySlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    frequencySlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 70, 20);
    frequencySlider.setRange(400.0, 8000.0, 1.0);
    addAndMakeVisible(frequencySlider);
    
    frequencyLabel.setText("Center Freq.", juce::dontSendNotification);
    frequencyLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    frequencyLabel.setFont(18.0f);
    frequencyLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(frequencyLabel);
    
    
    // first row auto-wah
    speedSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    speedSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 70, 20);
    speedSlider.setRange(0.0, 1.0, 0.0);
    addChildComponent(speedSlider);
    
    speedLabel.setText("Speed", juce::dontSendNotification);
    speedLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    speedLabel.setFont(18.0f);
    speedLabel.setJustificationType(juce::Justification::centred);
    addChildComponent(speedLabel);
    
    sensitivitySlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    sensitivitySlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 70, 20);
    sensitivitySlider.setRange(0.0, 1.0, 0.0);
    addChildComponent(sensitivitySlider);
    
    sensitivityLabel.setText("Sensitivity", juce::dontSendNotification);
    sensitivityLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    sensitivityLabel.setFont(18.0f);
    sensitivityLabel.setJustificationType(juce::Justification::centred);
    addChildComponent(sensitivityLabel);
    
    
    // second row
    distortionGainSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    distortionGainSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 70, 20);
    distortionGainSlider.setRange(0.0, 1.0, 0.0);
    addAndMakeVisible(distortionGainSlider);
    
    distortionGainLabel.setText("Distortion", juce::dontSendNotification);
    distortionGainLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    distortionGainLabel.setFont(18.0f);
    distortionGainLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(distortionGainLabel);
    
    mixSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    mixSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 70, 20);
    mixSlider.setRange(0.0, 1.0, 0.0);
    addAndMakeVisible(mixSlider);
    
    mixLabel.setText("Mix", juce::dontSendNotification);
    mixLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    mixLabel.setFont(18.0f);
    mixLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(mixLabel);
    
    
    // third row
    volumeSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    volumeSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 70, 20);
    volumeSlider.setRange(0.0, 1.0, 0.0);
    addAndMakeVisible(volumeSlider);
    
    volumeLabel.setText("Volume", juce::dontSendNotification);
    volumeLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    volumeLabel.setFont(18.0f);
    volumeLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(volumeLabel);
    
    bypassButton.setButtonText("BYPASS OFF");
    bypassButton.onClick = [this]() {
        auto new_state = bypassButton.getToggleState();
        if(!new_state){
            bypassButton.setButtonText("BYPASS OFF");
        } else{
            bypassButton.setButtonText("BYPASS ON");
        }
    };
    bypassButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
    bypassButton.setColour(juce::TextButton::buttonOnColourId, juce::Colours::green);
    bypassButton.setClickingTogglesState(true);
    addAndMakeVisible(bypassButton);
    
    wahModeSelectionButton.setButtonText("WAH-WAH");
    wahModeSelectionButton.setClickingTogglesState(true);
    wahModeSelectionButton.onClick = [this]() {
        auto new_state = wahModeSelectionButton.getToggleState();
        if(!new_state){
            wahModeSelectionButton.setButtonText("WAH-WAH");
        } else{
            wahModeSelectionButton.setButtonText("AUTO-WAH");
        }
        WahgliuAudioProcessorEditor::toggleWahMode(new_state);
    };
    addAndMakeVisible(wahModeSelectionButton);
    
    // attach apvts parameters to GUI elements
    freqSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "FREQ", frequencySlider);
    qSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "Q", qualitySlider);
    speedSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "SPEED", speedSlider);
    sensSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "SENS", sensitivitySlider);
    gainSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "GAIN", distortionGainSlider);
    mixSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "MIX", mixSlider);
    volumeSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "VOL", volumeSlider);
    bypassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "BYPASS", bypassButton);
    wahModeSelectionAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "WAHMODE", wahModeSelectionButton);
}

WahgliuAudioProcessorEditor::~WahgliuAudioProcessorEditor()
{
}

//==============================================================================
void WahgliuAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.setFont (15.0f);
}

void WahgliuAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    backgroundImageComp.setBounds(0,0,350,550);
    
    // first row wah-wah
    frequencyLabel.setBounds(55, 110, 100, 20);
    frequencySlider.setBounds(55, 130, 100, 100);
    
    qualityLabel.setBounds(195, 110, 100, 20);
    qualitySlider.setBounds(195, 130, 100, 100);
    
    // first row auto-wah
    speedLabel.setBounds(55, 110, 100, 20);
    speedSlider.setBounds(55, 130, 100, 100);
    
    sensitivityLabel.setBounds(195, 110, 100, 20);
    sensitivitySlider.setBounds(195, 130, 100, 100);
    
    // second row
    distortionGainLabel.setBounds(55, 240, 100, 20);
    distortionGainSlider.setBounds(55, 260, 100, 100);
    
    mixLabel.setBounds(195, 240, 100, 20);
    mixSlider.setBounds(195, 260, 100, 100);
    
    // third row
    volumeLabel.setBounds(55, 370, 100, 20);
    volumeSlider.setBounds(55, 390, 100, 100);
    bypassButton.setBounds(195, 400, 100, 30);
    wahModeSelectionButton.setBounds(195, 440, 100, 30);
}

void WahgliuAudioProcessorEditor::toggleWahMode(bool isWahWah) {
    // false == wah-wah
    frequencyLabel.setVisible(!isWahWah);
    frequencySlider.setVisible(!isWahWah);
    qualityLabel.setVisible(!isWahWah);
    qualitySlider.setVisible(!isWahWah);
    sensitivityLabel.setVisible(isWahWah);
    sensitivitySlider.setVisible(isWahWah);
    speedLabel.setVisible(isWahWah);
    speedSlider.setVisible(isWahWah);
}
