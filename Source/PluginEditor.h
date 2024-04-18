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
    void resized() override;
    void timerCallback() override;
    void getBounds();
    void setPlotIndex(int plotIndex);
    void updateToggleState(int plotId);
    void setVisibility(int plotId);
    void handleNewSelection(int numBins, int rowSize, int rowIndex);
    void setWindow(juce::dsp::WindowingFunction<float>::WindowingMethod type);

    //processBlock integration
    void processBuffer();
    void zeroBuffers();
    void zeroBinSelection();

    void mouseMove(const juce::MouseEvent& event) override;
    float findPeak();
    float screenToGraph(float screenCoord);
    float graphToScreen(int graphCoord);
    float getYCoord(int plotNumber, bool log, int index);
    void setFreqData(int fftData);
    void setWindowFunction();
    void setBlockSize();
    void setAxisType();
    std::string floatToStringPrecision(float f, int p);

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    FFTSpectrumAnalyzerAudioProcessor& audioProcessor;
    static bool isRunning;
    static bool isGraph;
    juce::Label cursorPlot;
    juce::Label peakPlot;

    // gui elements start
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
    juce::Label peakLabel;
    juce::Label windowLabel;
    juce::Label axisLabel;
    juce::Label sizeLabel;
    juce::ComboBox windowFunction;
    juce::ComboBox axis;
    juce::ComboBox size;
    // gui elements end

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
    static int xMinPrev;
    static int xMin;
    static int xMaxPrev;
    static int xMax;
    static int yMinPrev;
    static int yMin;
    static int yMaxPrev;
    static int yMax;
    static int plotIndexSelection;
    static float cursorX;
    static int cursorPeak;

    static int windowWidth;
    static int windowHeight;
    static int windowMaxWidth;
    static int windowMaxHeight;
    static int fftSize;
    static int numBins;
    // static int sampleRate;
    static int maxFreq;
    static int numFreqBins;
    static int rowIndex;
    static int rowSize;
    static int fftCounter;
    static int stepSize;

    static int count;
    static int countPrev;

    static bool setToLog;
    static bool newSelection;
    static bool initialWindow;

    //ProcessBlock 
    //static juce::dsp::FFT editFFT;
    static juce::dsp::WindowingFunction<float> editWindow;
    static std::vector<float> bufferRight;
    static std::vector<float> bufferLeft;
    static std::vector<float> windowBufferRight;
    static std::vector<float> windowBufferLeft;
    static std::vector<float> indexToFreqMap;
    static std::vector<std::vector<float>> binMag;
    static std::vector<std::vector<float>> sampleSelections;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FFTSpectrumAnalyzerAudioProcessorEditor)
};