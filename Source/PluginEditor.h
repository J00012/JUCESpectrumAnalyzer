/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
class FFTSpectrumAnalyzerAudioProcessorEditor : public juce::AudioProcessorEditor, juce::Timer
{
public:
    FFTSpectrumAnalyzerAudioProcessorEditor(FFTSpectrumAnalyzerAudioProcessor&);
    ~FFTSpectrumAnalyzerAudioProcessorEditor() override;

    //==============================================================================

    void paint(juce::Graphics&) override;
    //void drawFrame(juce::Graphics& g); 
    void setFreqData(int fftData, int sampleR);
    void resized() override;
    void timerCallback() override;
    void getBounds();
    void setPlotIndex(int plotIndex);
    void updateToggleState(int plotId);
    void setVisibility(int plotId);
    void mouseMove(const juce::MouseEvent& event) override; 
    //int findPeak(int index); 
    int screenToGraph(int screenCoord); 
    int graphToScreen(int graphCoord); 
    void setWindowFunction(); 
    void setSampleSize(); 
    void setAxisScale(); 
    std::string floatToStringPrecision(float f, int p);


private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    FFTSpectrumAnalyzerAudioProcessor& audioProcessor;
    static bool isRunning;
    static bool isVisiblePlot1;
    static bool isVisiblePlot2;
    static bool isGraph;

    // gui elements
    juce::Label gui_importAudio; 
    juce::Label gui_selectTrace; 
    juce::Label gui_zoom;
    juce::Label gui_upper;
    juce::Label gui_lower;
    juce::Label gui_xUpper;
    juce::Label gui_yUpper;
    juce::Label gui_xLower;
    juce::Label gui_yLower;
    juce::Label gui_export;
    juce::TextButton gui_exportButton{ "Export .csv" };
    juce::Label cursorLabel;
    juce::Label cursorFunction;
    juce::Label peakLabel;
    juce::Label peakFunction;
    juce::Label windowLabel;
    juce::Label axisLabel;
    juce::Label sizeLabel;
    juce::ComboBox windowFunction;
    juce::ComboBox axis;
    juce::ComboBox size;

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

    static int graphWest;
    static int graphEast;
    static int graphNorth;
    static int graphSouth;

    // mouse cursor elements
    juce::Label cursorPlot1; //mouse
    juce::Label cursorPlot2; //mouse
    juce::Label cursorLabel0; //mouse
    juce::Label cursorLabel1; //mouse
    juce::Label cursorLabel2; //mouse

    static int cursorX;
    static int cursorY;
    static float cursorYPeak;

    // row elements
    static int rowSize;
    static int rowIndex;

    // processor statics
    static int fftSize;
    static int fftS;
    static int numBins;
    static int sampleRate;
    static int maxFreq;
    static int numFreqBins;
    static int row;

    // static vectors
    static std::vector<float> indexToFreqMap;
    static std::vector<std::vector<float>> binMag;

    static int xMinPrev;
    static int xMin;
    static int xMaxPrev;
    static int xMax;
    static int yMinPrev;
    static int yMin;
    static int yMaxPrev;
    static int yMax;
    static int plotIndexSelection;

    // rectangle bounds for background
    int origin = 0;

    // bounds and sizes for widgets
    const int width_primaryCategoryLabel = 275;
    const int height_primaryCategoryLabel = 25;
    const int width_secondaryLabel = 150;
    const int height_secondaryLabel = 25;
    const int yOffsetPrimary_secondaryLabel = 8;
    const int x_componentOffset = 6;
    const int y_componentOffset = 6;
    const int yOffset_selectionBox = 2;
    const int width_selectionBox = 263;
    const int height_selectionBox = 90;

    // window dimensions 
    static int FFTSpectrumAnalyzerAudioProcessorEditor::windowWidth;
    static int FFTSpectrumAnalyzerAudioProcessorEditor::windowHeight;
    static int FFTSpectrumAnalyzerAudioProcessorEditor::windowMaxWidth;
    static int FFTSpectrumAnalyzerAudioProcessorEditor::windowMaxHeight;

    int sampleSize = 100;  // Adjust the number of samples being displayed as needed

    /*
    int widgetOffsetVertical = 10;
    int widgetOffsetHorizontal = 10;
    int widthLabel = 50;
    int widthPlotLabel = 50;
    int widthToggleButton = 30;
    int widthButton = 80;
    int widgetHeight = 24;
    */

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FFTSpectrumAnalyzerAudioProcessorEditor)
};