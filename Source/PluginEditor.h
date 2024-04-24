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
    int findPeak();
    float screenToGraph(float screenCoord);
    float graphToScreen(int graphCoord);
    float getYCoord(int plotNumber, bool log, int index);
    float calculateX(bool log, int index);
    float calculateY(int plotSelection, int index);
    bool inBounds(float x, float y);
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
    static int amountOfPlots;
    static int prevAmountOfPlots;
 struct plotItem {
        bool isVisible;
        juce::Colour color;
        juce::Path path;
        int checkBoxPos;
    };
    
    static plotItem plotInfo[7];

    static bool setToLog;
    static int initialAxisState;
    static bool newSelection;
    static bool displayError;
    static bool conCall;
    static bool blockProcessed;

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
    juce::Label labelPlot3{ "Plot 3" };
    juce::Label labelPlot4{ "Plot 4" };
    juce::Label labelPlot5{ "Plot 5" };
    juce::Label labelPlot6{ "Plot 6" };
    juce::Label labelPlot7{ "Plot 7" };
   
    juce::TextButton buttonPlot1{ "Selected" };
    juce::TextButton buttonPlot2{ "Select" };
    juce::TextButton buttonPlot3{ "Select" };
    juce::TextButton buttonPlot4{ "Select" };
    juce::TextButton buttonPlot5{ "Select" };
    juce::TextButton buttonPlot6{ "Select" };
    juce::TextButton buttonPlot7{ "Select" };
    
    juce::ToggleButton toggleButtonPlot1;
    juce::ToggleButton toggleButtonPlot2;
    juce::ToggleButton toggleButtonPlot3;
    juce::ToggleButton toggleButtonPlot4;
    juce::ToggleButton toggleButtonPlot5;
    juce::ToggleButton toggleButtonPlot6;
    juce::ToggleButton toggleButtonPlot7;
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
    // gui elements end

    const char* xAxisValueText = " Hz, ";
    const char* yAxisValueText = " dB)";

    int const logPower = 10;
    int const logScale = 40;

    int const cornerSizeSelectionBox = 3;
    int const cornersizeCheckbox = 4;
    int const tickWidth = 4;
    int const zeroTickWidth = 10;
    int const scaleTextOffsetX = 10;
    int const scaleTextOffsetY = 3;

    int const originPixel = 0;
    int const bufferExtraSmall = 2;
    int const bufferMediumSmall = 4;
    int const bufferSmall = 6;  //rename as padding
    int const bufferMedium = 10;
    int const bufferMediumLarge = 25;
    int const bufferLarge = 40;
    int const bufferExtraLarge = 55;

    int const widthWindowMin = 950;
    int const widthWindowMax = 2160;

    int const widthPrimaryCategoryLabel = 275;
    int const widthSecondaryLabel = 150;
    int const widthColumnGap = 120;
    int const widthSecondThirdColumnGap = widthColumnGap;
    int const widthxSmallWidget = 16;
    int const widthSmallWidget = 24;
    int const widthMediumSmallWidget = 30;
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
    int const heightImportAudioWindow = bufferSmall + heightMediumWidget + bufferSmall + heightSelectionBox + bufferSmall;
    int const heightZoomWindow = heightSmallWindow;
    int const heightExportWindow = heightBottomMenu;
    int const heightSelectionBox = heightSmallWindow;

    int const heightWindowMin = 550 + 2;
    int const heightWindowMax = 1080;

    int const xMarginOrigin = originPixel;
    int const xMarginFirstLeftMenuWidget = bufferMedium;
    int const xMarginRightMenu = widthPrimaryCategoryLabel;
    int const xMarginXYAxis = xMarginRightMenu + bufferLarge;
    int const xMarginXBoundsColumn = xMarginFirstLeftMenuWidget + bufferExtraLarge;
    int const xMarginYBoundsColumn = xMarginXBoundsColumn + widthColumnGap;
    int const xMarginSelectionBoundary = (widthSelectionBox - widthSelectionBoundary) - (widthSelectionBox - widthSelectionBoundary) / 4;
    int const xMarginZoomBoundary = xMarginSelectionBoundary;
    int const xMarginLabelPlots = xMarginFirstLeftMenuWidget + widthSmallWidget + bufferMedium;

    int const yMarginOrigin = originPixel;
    int const yMarginFirstLeftMenuWidget = bufferMedium + heightMediumWidget;
    int const yMarginLabelSelectTrace = heightMediumWidget + bufferSmall;
    int const yMarginLabelZoom = yMarginLabelSelectTrace + heightMediumWidget + bufferMedium + heightSelectionBox + bufferSmall;
    int const yMarginZoomBoundary = yMarginLabelZoom + heightMediumWidget + heightZoomWindow / 2;
    int const yMarginSelectionBox = yMarginLabelSelectTrace + heightMediumWidget + bufferSmall;
    int const yMarginSelectionBoundary = yMarginSelectionBox + heightSelectionBox / 2;
    int const yMarginRowPlot1 = yMarginSelectionBoundary - (bufferMedium + heightMediumWidget);
    int const yMarginRowPlot2 = yMarginSelectionBoundary + bufferMedium;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FFTSpectrumAnalyzerAudioProcessorEditor)
};