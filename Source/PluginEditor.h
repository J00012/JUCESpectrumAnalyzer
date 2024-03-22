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
    void getBounds();
    void setPlotIndex(int plotIndex);
    void updateToggleState(int plotId);
    void setVisibility(int plotId);
    void mouseDown(const juce::MouseEvent &event) override;


private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    FFTSpectrumAnalyzerAudioProcessor& audioProcessor;
    static bool isRunning;

    juce::Label cursor; //mouse
    juce::Label inputXmin;
    juce::Label inputXmax;
    juce::Label inputYmin;
    juce::Label inputYmax;
    juce::Label cursorLabel; //mouse
    juce::Label labelPlot1{ "Plot 1" };
    juce::Label labelPlot2{ "Plot 2" };
    juce::TextButton buttonPlot1{ "Selected" };
    juce::TextButton buttonPlot2{ "Select" };   
    juce::ToggleButton toggleButtonPlot1;
    juce::ToggleButton toggleButtonPlot2;
    static bool isVisiblePlot1;
    static bool isVisiblePlot2;
    static int cursorX; //mouse
    static int cursorY; //mouse
    static int xMinPrev;
    static int xMin;
    static int xMaxPrev;
    static int xMax;
    static int yMinPrev;
    static int yMin;
    static int yMaxPrev;
    static int yMax;
    static int plotIndexSelection;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FFTSpectrumAnalyzerAudioProcessorEditor)
};
