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
    void mouseMove(const juce::MouseEvent& event) override;
    std::string floatToStringPrecision(float f, int p);


private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    FFTSpectrumAnalyzerAudioProcessor& audioProcessor;
    static bool isRunning;
    static bool isVisiblePlot1;
    static bool isVisiblePlot2;

    juce::Label inputXmin;
    juce::Label inputXmax;
    juce::Label inputYmin;
    juce::Label inputYmax;
    juce::Label labelPlot1{ "Plot 1" };
    juce::Label labelPlot2{ "Plot 2" };
    juce::TextButton buttonPlot1{ "Selected" };
    juce::TextButton buttonPlot2{ "Select" };   
    juce::ToggleButton toggleButtonPlot1;
    juce::ToggleButton toggleButtonPlot2;
    juce::Label cursorPlot1; //mouse
    juce::Label cursorPlot2; //mouse
    juce::Label cursorLabel0; //mouse
    juce::Label cursorLabel1; //mouse
    juce::Label cursorLabel2; //mouse

    int cursorX1; //mouse
    float cursorY1; //mouse
    int cursorX2; //mouse
    float cursorY2; //mouse
 
    static int xMinPrev;
    static int xMin;
    static int xMaxPrev;
    static int xMax;
    static int yMinPrev;
    static int yMin;
    static int yMaxPrev;
    static int yMax;
    static int plotIndexSelection;

    // Rectangle bounds for background
    int origin = 0;

    // Bounds and sizes for widgets
    int widgetOffsetVertical = 10;
    int widgetOffsetHorizontal = 10;
    int widthLabel = 50;
    int widthPlotLabel = 50;
    int widthToggleButton = 30;
    int widthButton = 80;
    int widgetHeight = 24;

    int sampleSize = 100;  // Adjust the number of samples being displayed as needed

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FFTSpectrumAnalyzerAudioProcessorEditor)
};
