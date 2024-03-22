/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
class FFTSpectrumAnalyzerAudioProcessorEditor  : public juce::AudioProcessorEditor, juce::Timer
{
public:
    FFTSpectrumAnalyzerAudioProcessorEditor (FFTSpectrumAnalyzerAudioProcessor&);
    ~FFTSpectrumAnalyzerAudioProcessorEditor() override;

    //==============================================================================
    
    void paint (juce::Graphics&) override;
    void drawFrame(juce::Graphics& g); 
    void resized() override;
    void timerCallback() override;
    void getXmin();
    void getXmax();
    void getYmin();
    void getYmax();
    void updateToggleStatePlot1(juce::Button* button, juce::String name);
    void setVisibilityPlot1();
    void updateToggleStatePlot2(juce::Button* button, juce::String name);
    void setVisibilityPlot2();

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    FFTSpectrumAnalyzerAudioProcessor& audioProcessor;
    static bool isRunning;

    juce::Label inputXmin;
    juce::Label inputXmax;
    juce::Label inputYmin;
    juce::Label inputYmax;

    juce::ToggleButton graphVisibility_p1{ "Plot 1" };
    juce::ToggleButton graphVisibility_p2{ "Plot 2" };
    static bool isVisible_p1;
    static bool isVisible_p2;

    static int xMinPrev;
    static int x_min;
    static int xMaxPrev;
    static int x_max;
    static int yMinPrev;
    static int y_min;
    static int yMaxPrev;
    static int y_max;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FFTSpectrumAnalyzerAudioProcessorEditor)
};
