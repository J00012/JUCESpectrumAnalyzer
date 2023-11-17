# Waveform Plugin for Audacity

We are developing a plugin for AudacityⓇ that allows the user to map two frequency waveforms on top of each other, in the form of spectral graphs. Additionally, we will include functionality for the user to set the window function of the graph, the frequency/amplitude bounds, and the FFT (Fast Fourier Transform) size of the plot sample. 
The project will be scalable so that we can build upon it. 

Our goal is to develop a GUI to make audio analysis more user-friendly. 

## Minimum System Requirements

 - __C++ Standard__: 17+
 - __Windows__: Windows 8.1 and Visual Studio 2017

## Getting Started

Start by using any git client to download a copy of the [JUCE Repository](https://github.com/juce-framework/JUCE) to your computer ( for this example, we will be using Fork ). 

You will also need to download [Audacity](https://www.audacityteam.org/download/).

### For Fork users
 - Download [Fork](https://git-fork.com/) for Windows
   - In Fork, go to [File->Preferences] and choose a Default Clone Folder Path
 - Copy the git URL from the [JUCE Repository](https://github.com/juce-framework/JUCE), go back to Fork and click on [File->Clone], then paste the git URL into the Repository URL

   ![image](https://github.com/RobertMahin/csusm/assets/113948918/7ce5b0bb-8503-487e-b289-bfe6274c54d8)
 - In Fork, check out the 'develop' branch by scrolling up and double-clicking on the [origin/develop] branch. It will then prompt you to track this remote branch by clicking [Track]

### ProJucer

 - Now that you have a copy of JUCE on your computer, navigate to the JUCE folder and go to \extras\Projucer\Builds\VisualStudio20~\Projucer.sln
   - In Visual Studio, go to [Build->Build Solution]. You can close Visual Studio when the build is completed
 - Navigate to \extras\Projucer\Builds\VisualStudio20~\x64\Debug\App\Projucer.exe
 - If this is your first time using Projucer:
   - Go to [File->Sign in->Enable GPL Mode] (this requires you to make your project open-source)
   - Go to [File->Global Paths] and connect the 'Path to JUCE' path to ~\JUCE and the 'User Modules' path to ~\JUCE\modules

After cloning our repository onto your Visual Studio and clicking [Build Solution], navigate to our repository's file location and go to \Builds\VisualStudio20~\x64\Debug\VST3

Then copy the .vst3 file and paste it to your [Common Files] folder commonly found in C:\Program Files\Common Files

In Audacity, go to [Effects->Plugin Manager->Rescan] and enable our plugin. You may need to restart Audacity but our plugin is now available for you to use!


Make sure when in the projucer menu to enable plugin copy step

https://docs.juce.com/master/tutorial_choosing_projucer_template.html

When a Plugin is made in Projucer it makes 4 source files



SOLUTION EXPLORER
Once indside of Visual Studio Code there will be four projects in the Solution Explorer


    graphicsSampleTest_SharedCode: This project contains the shared code between the standalone and plugin projects. It includes the PluginProcessor and PluginEditor classes, which are used to create the audio plugin. The shared code is compiled into a static library and linked to the other projects.

    graphicsSampleTest_StandalonePlugin: This project is used to create a standalone version of the audio plugin. The standalone version can be run as a standalone application outside of a DAW.

    graphicsSampleTest_VST3: This project is used to create a VST3 version of the audio plugin. VST3 is a plugin format used by many DAWs, including Cubase, Nuendo, and Ableton Live.

    graphicsSampleTest_VST3ManifestHelper: This project is used to create a manifest file for the VST3 version of the audio plugin. The manifest file contains information about the plugin, such as its name, version, and author.


If you only want to display new UI elements like squares, you can modify the PluginEditor class in the graphicsSampleTest_SharedCode project. The PluginEditor class is responsible for creating and managing the user interface of the audio plugin. You can add new UI elements such as squares by modifying the PluginEditor class.


INITIAL PROJECT INFORMATION

The Audio Plugin project has an all around different project structure compared to other template projects offered by the Projucer and creates an AudioProcessor and an AudioProcessorEditor.

The AUDIOPROCESSOR CLASS is an abstract base class that handles the audio processing of your audio plugin. It represents an instance of a loaded plugin and is wrapped by the different plugin formats such as VST, AU, RTAS and AAX.

The AUDIOPROCESSOREDITOR CLASS is a base class derived from the COMPONENT class and holds the GUI functionalities of your audio plugin.


When an Audio Plugin project is created you will find the following files in the source folder:

    PluginProcessor.h: Header file for the PluginProcessor derived from the AudioProcessor class.
    PluginProcessor.cpp: Implementation file for the PluginProcessor derived from the AudioProcessor class.
    PluginEditor.h: Header file for the PluginEditor derived from the AudioProcessorEditor class.
    PluginEditor.cpp: Implementation file for the PluginEditor derived from the AudioProcessorEditor class.

"Use this project type when you want to create a plugin that can be hosted in a DAW or as a standalone application with a GUI."

KEY CLASS REFERENCES

AudioProcessor Class Reference - https://docs.juce.com/master/classAudioProcessor.html
AudioProcessorEditor Class Reference - https://docs.juce.com/master/classAudioProcessorEditor.html
MouseListener Class Reference - https://docs.juce.com/master/classMouseListener.html
Component Class Reference - https://docs.juce.com/master/classComponent.html



AUDIO PLUGIN INITIAL FILES

PluginProcessor.cpp:

This file typically contains the implementation of your audio processing logic. The PluginProcessor class is where you handle the actual audio signal processing, such as filtering, modulation, or any other audio-related functionality. You'll find methods like processBlock() in this file, where you write the code for manipulating audio samples.

PluginProcessor.h:

This header file is the corresponding declaration file for PluginProcessor.cpp. It usually contains the class declaration for PluginProcessor, including member variables, method prototypes, and any necessary includes or forward declarations. This file provides the interface for other parts of your program or external systems to interact with the audio processing logic.

PluginEditor.cpp:

The PluginEditor class is responsible for creating and managing the graphical user interface (GUI) of your audio plug-in. This file often includes code for constructing sliders, buttons, and other UI elements, as well as handling user interactions. Methods like resized() and paint() are common in this file, dealing with the layout and appearance of your plug-in's GUI.

PluginEditor.h:

Similar to PluginProcessor.h, this header file is the declaration file for PluginEditor.cpp. It contains the class declaration for PluginEditor, including member variables, method prototypes, and any necessary includes or forward declarations. This file defines the interface for interacting with the GUI from other parts of your program.





SAMPLE TUTORIAL:

CREATING A BASIC AUDIO/MIDI PLUGIN PART 1 (setting up envioroment) - https://docs.juce.com/master/tutorial_create_projucer_basic_plugin.html
CREATING A BASIC AUDIO/MIDI PLUGIN PART 2 (coding) - https://docs.juce.com/master/tutorial_code_basic_plugin.html


Orientation

A newly-created audio plug-in project contains two main classes: PluginProcessor handles the audio and MIDI IO and processing logic, and PluginEditor handles any on screen GUI controls or visualisations.

https://docs.juce.com/master/tutorial_graphics_class.html



dsp code (state based programming)
audio parameter



