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
    juce::Label labelPeakPlot;
    juce::Label labelImportAudio;
    juce::Label labelSelectTrace;
    juce::Label labelZoom;
    juce::Label labelUpperBounds;
    juce::Label labelLowerBounds;
    juce::Label labelExport;
    juce::Label labelPeak;
    juce::Label labelDropdownWindow;
    juce::Label labelDropdownAxis;
    juce::Label labelDropdownSize;
    juce::Label labelPlot1{ "Plot 1" };
    juce::Label labelPlot2{ "Plot 2" };
    juce::Label labelUpperBoundsX;
    juce::Label labelUpperBoundsY;
    juce::Label labelLowerBoundsX;
    juce::Label labelLowerBoundsY; 
    juce::Label inputXmin;
    juce::Label inputXmax;
    juce::Label inputYmin;
    juce::Label inputYmax;
    juce::ComboBox comboboxWindowFunction;
    juce::ComboBox comboboxAxisType;
    juce::ComboBox comboboxSizeSetting;
    juce::TextButton buttonExport{ "Export .csv" };
    juce::TextButton buttonPlot1{ "Selected" };
    juce::TextButton buttonPlot2{ "Select" };
    juce::ToggleButton toggleButtonPlot1;
    juce::ToggleButton toggleButtonPlot2;
    // gui elements end

    // height and width for primary category labels (Import Audio, Zoom, Export, etc.) 
    int widthPrimaryCategoryLabel = 275;
    int heightPrimaryCategoryLabel = 25;
    // height and width for secondary labels ("Selected Traces", Upper/Lower, etc.)
    int width_secondaryLabel = 150;
    int heightSecondaryLabel = 25;
    // space between primary labels and secondary labels
    int yOffsetPrimarySecondaryLabel = 8;
    // space between secondary components (e.g. white box for plot selection) and physical boundaries
    int xOffsetComponent = 6;
    int y_componentOffset = 6;
    // dimensions of white box for plot selection
    int yOffsetSelectionBox = 2;
    int widthSelectionBox = 263;
    int heightSelectionBox = 90;

    // GUI/Window Sizing and values
    int windowWidth = 950;
    int windowHeight = 550 + 2;
    int windowMaxWidth = 2160;
    int windowMaxHeight = 1080;
    int lineHeight = 1;
    //** white area of plot selection in IMPORT AUDIO**//
    int magic_num9 = 23;
    int verticalBufferCheckbox = 46;
    int xMarginCheckboxFill = 16;
    int yMarginCheckbox1Fill = 74;
    int xMarginZoomBoundary = 2.5 * xOffsetComponent;
    int xMarginSelectionBoundary = 2.5 * xOffsetComponent;

    int widthCheckbox = 16;
    int widthSelectionBoundary = 243;
    int widthZoomBoundary = 245;

    int heightSelectionBoundary = 1;
    int heightCheckbox = 16;
    int heightZoomBoundary = heightSelectionBoundary;

    int cornerSizeSelectionBox = 3;
    int cornersizeCheckbox = 4;

    int yMarginSelectionBoundary = heightPrimaryCategoryLabel + yOffsetPrimarySecondaryLabel + heightSecondaryLabel + (magic_num9 * yOffsetSelectionBox);
    int yMarginSelectionBox = heightPrimaryCategoryLabel + yOffsetPrimarySecondaryLabel + heightSecondaryLabel + yOffsetSelectionBox;
    int yMarginCheckbox2Fill = yMarginCheckbox1Fill + verticalBufferCheckbox;
    int yMarginZoomBoundary = (119.5 * yOffsetSelectionBox);

    int tickWidth = 5;
    int zeroTickWidth = 15;



    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FFTSpectrumAnalyzerAudioProcessorEditor)
};