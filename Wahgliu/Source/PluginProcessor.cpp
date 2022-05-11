#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
WahgliuAudioProcessor::WahgliuAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
: AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
                 .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
                 .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
                 ),
apvts(*this, nullptr, "Parameters", createParameters())
#endif
{
    
    // Initialization
    Q = 10.0;		 // Q-factor, computed runtime
    fb = 200.0;		 // Bandwidth, computed runtime
    fc = 1000.0;	 // Center frequency, computed runtime
    attack = 1;
    release = 0.01;
    minf = 100.0;    // Envelope sweeps from min freq to max freq
    maxf = 11000.0;
    minfbw = 80.0;
    maxfbw = 700.0;
}

WahgliuAudioProcessor::~WahgliuAudioProcessor()
{
}

//==============================================================================
const juce::String WahgliuAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool WahgliuAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool WahgliuAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool WahgliuAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double WahgliuAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int WahgliuAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int WahgliuAudioProcessor::getCurrentProgram()
{
    return 0;
}

void WahgliuAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String WahgliuAudioProcessor::getProgramName (int index)
{
    return {};
}

void WahgliuAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void WahgliuAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    
    lastSampleRate = sampleRate;
    
    // Initialize envelopes for auto-wah
    for (int i = 0; i < getTotalNumInputChannels(); ++i)
        envelopes.add(0.0f);
}

void WahgliuAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool WahgliuAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
    
    // This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif
    
    return true;
#endif
}
#endif

void WahgliuAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    
    juce::dsp::AudioBlock <float> block(buffer);
    
    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    
    if (*apvts.getRawParameterValue("BYPASS"))
        return; // no processing, output == input already on the buffer
    
    bool isAutoWah = *apvts.getRawParameterValue("WAHMODE");
    
    for (int channel = 0; channel < totalNumInputChannels; ++channel) {
        
        auto* channelData = buffer.getWritePointer(channel);
        
        for (int n = 0; n < buffer.getNumSamples(); ++n) {
            
            float x = channelData[n];
            float cleanSig = x;
            
            // Distortion
            float gain = *apvts.getRawParameterValue("GAIN");   // from 0 to 1
            if (gain != 0.0) {
                x *= gain * 1000;
                x = (2.f / juce::MathConstants<float>::pi) * atan(x);
                x /= 50;
            }
            
            if (isAutoWah) {
                // AUTO-WAH PROCESSING
                
                float sens = *apvts.getRawParameterValue("SENS") * 1.5; // from 0 to 1.5
                float rel = release / ( (sens/1.5) + 1 );               // actual release = 0.01/[1, 2]
                
                float speed = *apvts.getRawParameterValue("SPEED");     // from 0 to 1
                float atck = attack / ( (speed * 1.8) + 0.2);           // actual attack = 1/[0.2, 2]
                
                // Envelope
                float envelope;
                alpha_A = exp(-1.0f / (atck * getSampleRate()));
                alpha_R = exp(-1.0f / (rel * getSampleRate()));
                
                // Level detector
                if (fabs(x) > envelopes[channel]) {
                    envelope = alpha_A * envelopes[channel] + (1.0f - alpha_A) * fabs(x);   // Attack coeff
                }
                else {
                    envelope = alpha_R * envelopes[channel] + (1.0f - alpha_R) * fabs(x);   // Release coeff
                }
                
                envelopes.set(channel, envelope);
                
                // Center freq and Q factor
                fc = minf + (maxf - minf) * (envelopes[channel] / 0.03) * sens;
                
                fb = minfbw + (maxfbw - minfbw) * (envelopes[channel] / 0.03);
                Q = fc / fb;
                
                if (fc >= 16500) { fc = 16500; }    // to avoid errors with low sample rates
                if (fc <= 20) { fc = 20; }
                
                filterCoefs = juce::dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(), fc, Q, 7);
            } else {
                // WAH-WAH PROCESSING
                
                if (fc >= 16500) { fc = 16500; }    // to avoid errors with low sample rates
                if (fc <= 20) { fc = 20; }
                
                float centerFreq = *apvts.getRawParameterValue("FREQ");
                float quality = *apvts.getRawParameterValue("Q");
                
                filterCoefs = juce::dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(), centerFreq, quality, 7);
            }
            
            // Filter and Mix
            float mix = *apvts.getRawParameterValue("MIX");     // from 0 to 1
            float vol = *apvts.getRawParameterValue("VOL") * 2; // from 0 to 2
            
            switch (channel)
            {
                case 0:
                    filterLeft.coefficients = filterCoefs;
                    x = filterLeft.processSample(x);
                    break;
                    
                case 1:
                    filterRight.coefficients = filterCoefs;
                    x = filterRight.processSample(x);
                    break;
                    
                default:
                    break;
            }
            
            channelData[n] = ((x * mix) + (cleanSig * (1.f - mix))) * vol;  // mix and volume setting stage
            
        }
    }
}

//==============================================================================
bool WahgliuAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* WahgliuAudioProcessor::createEditor()
{
    return new WahgliuAudioProcessorEditor (*this);
}

//==============================================================================
void WahgliuAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void WahgliuAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new WahgliuAudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout WahgliuAudioProcessor::createParameters()
{
    // parameters correspond to a vector of unique pointers to objects of classes RangedAudioParameters
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> parameters;
    
    // we push in the vector the objects using make_unique template (which actually allocate the memory for the objects) on the class AudioParameter
    // AudioParameterFloat (String parameterID, String parameterName, float minValue, float maxValue, float defaultValue)ù
    // int similarly
    // AudioParameterBool (String parameterID, String parameterName, bool defaultValue)
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("FREQ", "Center freq", 400.0f, 8000.0f, 1000.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("Q", "Q Factor", 0.5f, 3.0f, 1.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("SPEED", "Speed", 0.0f, 1.0f, 0.5f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("SENS", "Sensitivity", 0.0f, 1.0f, 0.5f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("GAIN", "Distortion gain", 0.0f, 1.0f, 0.5f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("MIX", "Mix", 0.0f, 1.0f, 0.5f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("VOL", "Volume", 0.0f, 1.0f, 0.5f));
    parameters.push_back(std::make_unique<juce::AudioParameterBool>("BYPASS", "Bypass" , false));
    parameters.push_back(std::make_unique<juce::AudioParameterBool>("WAHMODE", "Wah or Auto-Wah" , false));
    
    return { parameters.begin(), parameters.end() };
}
