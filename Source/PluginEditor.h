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
    float calculateX(bool log, int index);

    //processBlock integration
    void processBuffer();
    void zeroBuffers();
    void zeroBinSelection();

    void mouseMove(const juce::MouseEvent& event) override;
    int findPeak();
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
    static bool setToLog;
    static bool newSelection;
    static bool displayError;

    static int fftSize;
    static int numBins;
    // static int sampleRate;
    static int maxFreq;
    static int numFreqBins;
    static int rowIndex;
    static int rowSize;
    static int fftCounter;
    static int stepSize;
    static int amountOfPlots;
    static int prevAmountOfPlots;
    static int initialAxisState;

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
    static bool isVisiblePlot1;
    static bool isVisiblePlot2;
    static float xMinPrev;
    static float xMin;
    static float xMaxPrev;
    static float xMax;
    static float yMinPrev;
    static float yMin;
    static float yMaxPrev;
    static float yMax;
    static int plotIndexSelection;
    static float cursorX;
    static int cursorPeak;
    static float xMaxFrequency;
    static float xMinFrequency;

    // gui elements start
    juce::Label labelCursor;
    juce::Label labelCursorValue;
    juce::Label labelPeak;
    juce::Label labelPeakValue;
    juce::Label labelImportAudio;
    juce::Label labelSelectTrace;
    juce::Label labelZoom;
    juce::Label labelUpperBounds;
    juce::Label labelLowerBounds;
    juce::Label labelExport;
    juce::Label labelDropdownAxis;
    juce::Label labelDropdownSize;
    juce::Label labelDropdownWindow;
    juce::Label labelPlot1{ "Plot 1" };
    juce::Label labelPlot2{ "Plot 2" };
    juce::Label labelUpperBoundsX;
    juce::Label labelUpperBoundsY;
    juce::Label labelLowerBoundsX;
    juce::Label labelLowerBoundsY; 
    juce::Label inputLowerBoundsX;
    juce::Label inputUpperBoundsX;
    juce::Label inputLowerBoundsY;
    juce::Label inputUpperBoundsY;
    juce::ComboBox comboboxAxisType;
    juce::ComboBox comboboxSizeSetting;
    juce::ComboBox comboboxWindowFunction;
    juce::TextButton buttonExport{ "Export .csv" };
    juce::TextButton buttonSelectPlot1{ "Selected" };
    juce::TextButton buttonSelectPlot2{ "Select" };
    juce::ToggleButton toggleButtonPlot1;
    juce::ToggleButton toggleButtonPlot2;
    // gui elements end

    int bufferXComponent = 6;
    int bufferYComponent = 6;
    int bufferXGraphBorder = 10;
    int bufferYPrimarySecondaryLabel = 8;
    int bufferYCheckbox = 46;
    int bufferX = xMarginRightMenu + 2;
    int bufferY = bufferYComponent + 12;
    int bufferDrawingWindowBorder = 20;

    int windowWidth = 950;
    int windowHeight = 550 + 2;
    int windowMaxWidth = 2160;
    int windowMaxHeight = 1080;
    int widthPrimaryCategoryLabel = 275;
    int widthSecondaryLabel = 150;

    int lineHeight = 1;
    int heightPrimaryCategoryLabel = 25;
    int heightSecondaryLabel = 25;

    int xMarginRightMenu = widthPrimaryCategoryLabel + bufferXGraphBorder;
    int xMarginOrigin = 0;
    int xMarginXYAxis = xMarginRightMenu + bufferDrawingWindowBorder;

    int yMarginOrigin = 0;
    int yMarginSelectionBox = heightPrimaryCategoryLabel + bufferYPrimarySecondaryLabel + heightSecondaryLabel + yMarginSelectionBox;




    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FFTSpectrumAnalyzerAudioProcessorEditor)
};