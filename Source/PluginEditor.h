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

    int const originPixel = 0;
    int const bufferXxSmall = 2;
    int const bufferYxSmall = 2; 
    int const bufferXSmall = 6; 
    int const bufferYSmall = 6; 
    int const bufferXMedium = 10;
    int const bufferYMedium = 10;
    int const bufferXLarge = 40;
    int const bufferYLarge = 40;
    int const bufferXxLarge = 55;
    int const bufferYxLarge = 55;
    int const bufferYPrimarySecondaryLabel = 8;
    int const bufferDrawingWindowBorder = bufferXLarge;
    int const bufferSurroundingBox = bufferXSmall; 

    int const widthWindowMin = 950;
    int const widthWindowMax = 2160;
    int const widthPrimaryCategoryLabel = 275;
    int const widthSecondaryLabel = 150;
    int const widthColumnGap = 120;
    int const widthSecondThirdColumnGap = widthColumnGap;
    int const widthxSmallWidget = 16;
    int const widthSmallWidget = 24;
    int const widthMediumWidget = 60;
    int const widthLargeWidget = 90;
    int const widthxLargeWidget = 180;
    int const widthBoundary = 243;
    int const widthSelectionBox = 263;
    int const widthSelectionBoundary = widthBoundary;
    int const widthZoomBoundary = widthBoundary;
    int const widthInputTextbox = widthMediumWidget;

    int const thicknessLine = 1;
    int const heightSmallWidget = 16;
    int const heightMediumWidget = 24;
    int const heightSmallWindow = 90;
    int const heightMediumWindow = 240; 
    int const heightBottomMenu = heightMediumWindow;
    int const heightImportAudioWindow = bufferYSmall + heightCategoryLabel + bufferYSmall + heightSelectionBox + bufferYSmall;
    //int heightZoomWindow = heightImportAudioWindow;
    int const heightZoomWindow = heightSmallWindow;
    int const heightExportWindow = heightBottomMenu;
    int const heightSelectionBox = heightSmallWindow;

    int const heightWindowMin = 550 + 2;
    int const heightWindowMax = 1080;
    int const heightCategoryLabel = heightMediumWidget;

    int const xMarginOrigin = originPixel;
    int const xMarginFirstLeftMenuWidget = bufferXMedium;
    int const xMarginRightMenu = widthPrimaryCategoryLabel;
    int const xMarginXYAxis = xMarginRightMenu + bufferDrawingWindowBorder;
    int const xMarginXBoundsColumn = xMarginFirstLeftMenuWidget + bufferXxLarge;
    int const xMarginYBoundsColumn = xMarginXBoundsColumn + widthColumnGap;
    int const xMarginSelectionBoundary = (widthSelectionBox - widthSelectionBoundary) - (widthSelectionBox - widthSelectionBoundary) / 4;
    int const xMarginZoomBoundary = xMarginSelectionBoundary;

    int const yMarginOrigin = originPixel;
    int const yMarginFirstLeftMenuWidget = bufferYMedium + heightCategoryLabel;
    int const yMarginLabelSelectTrace = heightCategoryLabel + bufferYSmall;
    int const yMarginLabelZoom = yMarginLabelSelectTrace + heightCategoryLabel + bufferYMedium + heightSelectionBox + bufferYSmall;
    int const yMarginZoomBoundary = yMarginLabelZoom + heightCategoryLabel + heightZoomWindow / 2;
    int const yMarginSelectionBoundary = yMarginLabelSelectTrace + heightCategoryLabel + bufferYSmall + heightSelectionBox / 2;
    int const yMarginSelectionBox = yMarginLabelSelectTrace + heightCategoryLabel + bufferYSmall;
    int const yMarginRowPlot1 = yMarginSelectionBox + heightSelectionBox / 4 - heightSmallWidget / 2;
    int const yMarginRowPlot2 = yMarginSelectionBoundary + heightSelectionBox / 4 - heightSmallWidget / 2;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FFTSpectrumAnalyzerAudioProcessorEditor)
};