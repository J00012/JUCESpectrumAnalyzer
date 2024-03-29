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


private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    FFTSpectrumAnalyzerAudioProcessor& audioProcessor;
    static bool isRunning;

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
    static bool isVisiblePlot1;
    static bool isVisiblePlot2;
 
    int xMinPrev = 0;
    int xMin = 0;
    int xMaxPrev = 100;
    int xMax = 100;
    int yMinPrev = -10;
    int yMin = -10;
    int yMaxPrev = 10;
    int yMax = 10;
    int plotIndexSelection = 0;

    // Rectangle bounds for background
    int FFTSpectrumAnalyzerAudioProcessorEditor::widgetHeight = 24;
    int rectanglesTopLeftMargin = 0;
    int leftMarginR2 = 950;
    int topMarginR4 = 1200;
    int widthR1 = 1200;
    int widthR2 = 600;
    int widthR3 = 1200;
    int widthR4 = 1200;
    int heightR1 = 50;
    int heightR2 = 950;
    int heightR3 = 50;
    int heightR4 = 400;

    // Bounds and sizes for widgets
    int widgetOffsetVertical = 10;
    int widgetOffsetHorizontal = 10;
    int widthLabel = 50;
    int widthPlotLabel = 50;
    int widthToggleButton = 30;
    int widthButton = 90;

    int sampleSize = 100;  // Adjust the number of samples being displayed as needed

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FFTSpectrumAnalyzerAudioProcessorEditor)
};
