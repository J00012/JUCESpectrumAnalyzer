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
    bool inBounds(float x, float y);
    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;
    void getBounds();
    void setPlotIndex(int plotIndex);
    void setPlotVisibility(int plotId);
    void handleNewSelection(int numBins, int rowSize, int rowIndex);
    void initializeBinMag();
    void setWindow(juce::dsp::WindowingFunction<float>::WindowingMethod type); 
    void mouseMove(const juce::MouseEvent& event) override;
    void setWindowFunction(); 
    void setBlockSize();
    void setAxisType();
    void writeAxisLabels(juce::Graphics& g, juce::Path axisMarkers, juce::String text, float x, float y, int textOffset, char axis);
    void setAppearance();
   
    //processBlock integration
    void processBuffer();
    void zeroBuffers();
    void setFreqData(int fftData); 
    bool selectionSizeValid();
    
    float screenToGraph(float screenCoord);
    float graphToScreen(int graphCoord);
    float getYCoord(int index);
    float calculateX(int index);
    float calculateY(int index);
    int calculateIndex(float cursor);

 

    int findPeak(int samples);
    int getAxisLength(char axis);

    std::string floatToStringPrecision(float f, int p);

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    FFTSpectrumAnalyzerAudioProcessor& audioProcessor;

    //ProcessBlock 
    //static juce::dsp::FFT editFFT;
    static juce::dsp::WindowingFunction<float> windowData;
    static std::vector<float> bufferRight;
    static std::vector<float> bufferLeft;
    static std::vector<float> windowBufferRight;
    static std::vector<float> windowBufferLeft;
    static std::vector<float> indexToFreqMap;
    static std::vector<std::vector<float>> binMag;
    static std::vector<std::vector<float>> sampleSelections;
    static std::vector<std::string> numPlots; //can later use to allow user to add mulitple plots
    static std::vector<std::string> windowFunctionTypes;
    static std::vector<std::string> axisTypes;
    static std::vector<std::string> sizeOptions;
    static bool isRunning;
    static bool setToLog;
    static bool newSelection;
    static bool displayError;
    static bool conCall;
    static bool blockProcessed;
    static bool isVisiblePlot1;
    static bool isVisiblePlot2;
    static bool darkMode;
    static bool gridOff;
    static bool initialLambda;
    static bool selectionSizeError;
    static float xMinPrev;
    static float xMin;
    static float xMaxPrev;
    static float xMax;
    static float yMinPrev;
    static float yMin;
    static float yMaxPrev;
    static float yMax;
    static float peakRange;
    static float xMaxFrequency;
    static float xMinFrequency;

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
    static int windowVar;
    static int cursorIndex;
    static int initialAxisState;
    static char axis;

     struct plotItem {
            bool isVisible;
            juce::Colour color;
            juce::Path path;
        };
    
    static plotItem plotInfo[4];

    //std::vector<std::string>plotToggleButtons = { 0 };
    //std::vector<std::string>plotSelectButtons = { 0 };
    //std::vector<std::string>plotLabels = { 0 };


    juce::Label labelPlot1{ "Trace 1" };
    juce::Label labelPlot2{ "Trace 2" };
    juce::Label labelPlot3{ "Trace 3" };
    juce::Label labelPlot4{ "Trace 4" };

    juce::Label labelUpperBoundsX;
    juce::Label labelUpperBoundsY;
    juce::Label labelLowerBoundsX;
    juce::Label labelLowerBoundsY;
    juce::Label inputLowerBoundsX;
    juce::Label inputUpperBoundsX;
    juce::Label inputLowerBoundsY;
    juce::Label inputUpperBoundsY;
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
    juce::Label labelAppearanceMode;
    juce::Label labelGrid;

    const char* textSelected = "Selected";
    const char* textNotSelected = "Select";
   
    juce::TextButton buttonSelectPlot1{ textSelected };
    juce::TextButton buttonSelectPlot2{ textNotSelected };
    juce::TextButton buttonSelectPlot3{ textNotSelected };
    juce::TextButton buttonSelectPlot4{ textNotSelected };
 
    juce::TextButton buttonExport{ "Export .csv" };
    
    juce::ToggleButton toggleButtonPlot1;
    juce::ToggleButton toggleButtonPlot2;
    juce::ToggleButton toggleButtonPlot3;
    juce::ToggleButton toggleButtonPlot4;

    juce::ToggleButton togglePluginAppearance;
    juce::ToggleButton toggleGrid;

    juce::ComboBox comboboxAxisType;
    juce::ComboBox comboboxSizeSetting;
    juce::ComboBox comboboxWindowFunction;
    // gui elements end

    const char* xAxisValueText = " Hz, ";
    const char* yAxisValueText = " dB";

    int const logPower = 10;
    int const logScale = 40;
    int const yMinVal = -200;

    int const precisionValue1 = 1;
    int const precisionValue2 = 2;

    int const cornerSizeSelectionBox = 3;
    int const borderSizeSelectionBox = 1;
    int const cornersizeCheckbox = 4;
    int const tickWidth = 4;
    int const zeroTickWidth = 10;
    int const scaleTextOffsetX = 10;
    int const scaleTextOffsetY = 3;

    int const originPixel = 0;

    int const paddingExtraSmall = 2;
    int const paddingMediumSmall = 4;
    int const paddingSmall = 6;  //rename as padding
    int const paddingMedium = 10;
    int const paddingMediumLarge = 25;
    int const paddingLarge = 40;
    int const paddingExtraLarge = 55;

    int const widthWindowMin = 950;
    int const widthWindowMax = 2160;

    int const widthPrimaryCategoryLabel = 275;
    int const widthSecondaryLabel = 150;
    int const widthColumnGap = 120;
    int const widthSecondThirdColumnGap = widthColumnGap;
    int const widthExtraSmallWidget = 16;
    int const widthSmallWidget = 24;
    int const widthMediumSmallWidget = 30;
    int const widthMediumWidget = 60;
    int const widthLargeWidget = 90;
    int const widthExtraLargeWidget = 180;
    int const widthBoundary = 243;
    int const widthSelectionBox = 263;
    int const widthSelectionBoundary = widthBoundary;
    int const widthZoomBoundary = widthBoundary;
    int const widthInputTextbox = widthMediumWidget;

    int const thicknessLine = 1;
    int const heightSmallWidget = 16;
    int const heightMediumWidget = 24;
    int const heightLargeWidget = 30;
    int const heightSmallWindow = 90;
    int const heightMediumWindow = 180;
    int const heightLargeWindow = 240; 
    int const heightBottomMenu = heightLargeWindow;
    int const heightImportAudioWindow = paddingSmall + heightMediumWidget + paddingSmall + heightSelectionBox + paddingSmall;
    int const heightZoomWindow = heightSmallWindow;
    int const heightExportWindow = heightBottomMenu;
    int const heightSelectionBox = heightMediumWindow;

    int const heightWindowMin = 550 + 2;
    int const heightWindowMax = 1080;

    int const xMarginOrigin = originPixel;
    int const xMarginFirstLeftMenuWidget = paddingMedium;
    int const xMarginRightMenu = widthPrimaryCategoryLabel;
    int const xMarginXYAxis = xMarginRightMenu + paddingLarge;
    int const xMarginXBoundsColumn = xMarginFirstLeftMenuWidget + paddingExtraLarge;
    int const xMarginYBoundsColumn = xMarginXBoundsColumn + widthColumnGap;
    int const xMarginSelectionBoundary = (widthSelectionBox - widthSelectionBoundary) - (widthSelectionBox - widthSelectionBoundary) / 4;
    int const xMarginZoomBoundary = xMarginSelectionBoundary;
    int const xMarginLabelPlots = xMarginFirstLeftMenuWidget + widthSmallWidget + paddingMedium;
     
    int const yMarginOrigin = originPixel;
    int const yMarginFirstLeftMenuWidget = paddingMedium + heightMediumWidget;
    int const yMarginLabelSelectTrace = heightMediumWidget + paddingSmall;
    int const yMarginLabelZoom = yMarginLabelSelectTrace + heightMediumWidget + paddingMedium + heightSelectionBox + paddingSmall;
    int const yMarginZoomBoundary = yMarginLabelZoom + heightMediumWidget + heightZoomWindow / 2;
    int const yMarginSelectionBox = yMarginLabelSelectTrace + heightMediumWidget + paddingSmall;
    int const yMarginSelectionBoundary1 = yMarginSelectionBox + heightSelectionBox * 1/4;
    int const yMarginSelectionBoundary2 = yMarginSelectionBox + heightSelectionBox * 2/4;
    int const yMarginSelectionBoundary3 = yMarginSelectionBox + heightSelectionBox * 3/4;
    int const yMarginRowPlot1 = yMarginSelectionBoundary1 - (paddingMedium + heightMediumWidget);
    int const yMarginRowPlot2 = yMarginSelectionBoundary1 + paddingMedium;
    int const yMarginRowPlot3 = yMarginSelectionBoundary3 - (paddingMedium + heightMediumWidget);
    int const yMarginRowPlot4 = yMarginSelectionBoundary3 + paddingMedium;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FFTSpectrumAnalyzerAudioProcessorEditor)
};