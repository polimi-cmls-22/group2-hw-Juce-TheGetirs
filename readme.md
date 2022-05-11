![wagliu](Wahgliu/Resources/waghliu.png)
## Wah-Wah and Auto-Wah with distortion mixing
#### CMLS 2022 - HW 2

## Folder structure
```bash
.
├── Wahgliu
│   ├── Resources
│   │   ├── autowah.png
│   │   ├── background.png
│   │   ├── knobImg.png
│   │   ├── realegetir.png
│   │   ├── waghliu.png
│   │   └── wahwah.png
│   ├── Source
│   │   ├── PluginEditor.cpp
│   │   ├── PluginEditor.h
│   │   ├── PluginProcessor.cpp
│   │   ├── PluginProcessor.h
│   │   ├── SliderLookAndFeel.cpp
│   │   └── SliderLookAndFeel.h
│   └── Wahgliu.jucer
├── readme.md
└── report TheGetirs.pdf
```

# Introduction
"Wah-gliù" is a VST Plug-In that emulates the function of a Wah-Wah pedal. It has two main function: AUTO-WAH and WAH-WAH. You can switch between the two functions with a button in the lower right section of the GUI.  

# Plug-in Usage
Compile the .vst3 file and launch it into your preferred DAW. 
To use the wah-wah mode, you should map the central frequency parameter to an external midi controller (expression pedal, sliders, knobs) to get the best exprerience. We suggest to use Ableton Live to get the best results. 

# Implementation 
"Wah-gliù" is a VST implemented in C++, through the applicative framework JUCE.

## Auto-Wah
With the Auto-Wah function ON, the effect follows the volume envelope of the input signal. Looking at the GUI, the knobs that you can use to customize the effect are:
- Speed: regulates how quick the response of the envelope follower is.
- Sensitivity: modifies the filter sweep, i.e. how much the center frequency of the peaking filter follows the envelope of the signal in auto-wah mode.
- Mix: regulates the ratio between the dry and the wet signal
at the output stage.
- Distortion: distortion gain applied to the effected signal before filtering.
- Volume: master volume of the effect.
- Bypass: grants possibility to bypass the effect, hearing the input as-is.

## Wah-Wah
With the Wah-Wah function ON, the effect works like a typical wah-wah pedal. Looking at the GUI, the knobs that you can use to customize the effect are:
- Frequency (controlled through MIDI devices)
- Q factor: acts on the quality factor of the peaking filter applied to the wet
signal.
- Mix: regulates the ratio between the dry and the wet signal
at the output stage.
- Distortion: distortion gain applied to the effected signal before filtering.
- Volume: master volume of the effect.
- Bypass: grants possibility to bypass the effect, hearing the input as-is.

## Distortion
The "Wah-gliù" has an integrated distortion effect, applied before the filtering phase, that can be used to create some unique effects. 

## GUI
<div>
<img src="Wahgliu/Resources/wahwah.png" alt="drawing" width="200"/>
<img src="Wahgliu/Resources/autowah.png" alt="drawing" width="200"/>
</div>

## Modules to add
* juce_cryptography
* juce_dsp
* juce_opengl
* juce_video

## Authors
![getirs](Wahgliu/Resources/realegetir.png) 

Di Palma - Gargiulo - Orsatti - Morena - Perego