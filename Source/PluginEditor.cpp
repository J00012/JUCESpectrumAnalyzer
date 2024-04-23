/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <string>
#include <algorithm>


float FFTSpectrumAnalyzerAudioProcessorEditor::cursorX;
int FFTSpectrumAnalyzerAudioProcessorEditor::cursorPeak = 0;
bool FFTSpectrumAnalyzerAudioProcessorEditor::isRunning = false;
bool FFTSpectrumAnalyzerAudioProcessorEditor::newSelection = false;
bool FFTSpectrumAnalyzerAudioProcessorEditor::isVisiblePlot1 = true;
bool FFTSpectrumAnalyzerAudioProcessorEditor::isVisiblePlot2 = true;
float FFTSpectrumAnalyzerAudioProcessorEditor::xMinPrev = 1;
float FFTSpectrumAnalyzerAudioProcessorEditor::xMin = 1;
float FFTSpectrumAnalyzerAudioProcessorEditor::xMinFrequency = 1;
float FFTSpectrumAnalyzerAudioProcessorEditor::xMaxPrev = 100;
float FFTSpectrumAnalyzerAudioProcessorEditor::xMax = 8000;
float FFTSpectrumAnalyzerAudioProcessorEditor::xMaxFrequency = 8000;
float FFTSpectrumAnalyzerAudioProcessorEditor::yMinPrev = -90;
float FFTSpectrumAnalyzerAudioProcessorEditor::yMin = -90;
float FFTSpectrumAnalyzerAudioProcessorEditor::yMaxPrev = 0;
float FFTSpectrumAnalyzerAudioProcessorEditor::yMax = 0;
int FFTSpectrumAnalyzerAudioProcessorEditor::plotIndexSelection = 0;

//ROW INDEX STUFF!!!
int FFTSpectrumAnalyzerAudioProcessorEditor::rowSize = 2;
int FFTSpectrumAnalyzerAudioProcessorEditor::rowIndex = 0;
int FFTSpectrumAnalyzerAudioProcessorEditor::amountOfPlots = 0;
int FFTSpectrumAnalyzerAudioProcessorEditor::prevAmountOfPlots = 0;

//Processor statics
int FFTSpectrumAnalyzerAudioProcessorEditor::fftSize = 1024;
int FFTSpectrumAnalyzerAudioProcessorEditor::numBins = 513;
int FFTSpectrumAnalyzerAudioProcessorEditor::maxFreq = 8000;
int FFTSpectrumAnalyzerAudioProcessorEditor::stepSize = 512;
int FFTSpectrumAnalyzerAudioProcessorEditor::numFreqBins = 0;
int FFTSpectrumAnalyzerAudioProcessorEditor::fftCounter = 0;

bool FFTSpectrumAnalyzerAudioProcessorEditor::setToLog;
int FFTSpectrumAnalyzerAudioProcessorEditor::initialAxisState = 1;

//Processor vectors
std::vector<float> FFTSpectrumAnalyzerAudioProcessorEditor::indexToFreqMap = { 0 };
std::vector< std::vector<float>> FFTSpectrumAnalyzerAudioProcessorEditor::binMag;
std::vector< std::vector<float>> FFTSpectrumAnalyzerAudioProcessorEditor::sampleSelections;
std::vector<float> FFTSpectrumAnalyzerAudioProcessorEditor::bufferRight = { 0 };
std::vector<float> FFTSpectrumAnalyzerAudioProcessorEditor::bufferLeft = { 0 };
std::vector<float> FFTSpectrumAnalyzerAudioProcessorEditor::windowBufferRight = { 0 };
std::vector<float> FFTSpectrumAnalyzerAudioProcessorEditor::windowBufferLeft = { 0 };
//juce::dsp::FFT FFTSpectrumAnalyzerAudioProcessorEditor::editFFT(0);

juce::dsp::WindowingFunction<float> FFTSpectrumAnalyzerAudioProcessorEditor::editWindow(0, juce::dsp::WindowingFunction<float>::blackman);

//==============================================================================
FFTSpectrumAnalyzerAudioProcessorEditor::FFTSpectrumAnalyzerAudioProcessorEditor(FFTSpectrumAnalyzerAudioProcessor& p)
	: AudioProcessorEditor(&p), audioProcessor(p)
{
	setOpaque(true);
	startTimer(500);

	setSize(windowWidth, windowHeight);
	setResizable(true, true);
	setResizeLimits(windowWidth, windowHeight, windowMaxWidth, windowMaxHeight);

	sampleSelections.resize(2);
	setPlotIndex(plotIndexSelection);
	setFreqData(1024);
	zeroBuffers();

	// Make visible GUI Elements
	addAndMakeVisible(labelImportAudio);
	addAndMakeVisible(labelSelectTrace);
	addAndMakeVisible(labelZoom);
	addAndMakeVisible(labelUpperBounds);
	addAndMakeVisible(labelLowerBounds);
	addAndMakeVisible(labelUpperBoundsX);
	addAndMakeVisible(labelUpperBoundsY);
	addAndMakeVisible(labelLowerBoundsX);
	addAndMakeVisible(labelLowerBoundsY);
	addAndMakeVisible(labelExport);
	addAndMakeVisible(labelPlot1);
	addAndMakeVisible(labelPlot2);
	addAndMakeVisible(labelCursor);
	addAndMakeVisible(labelCursorValue);
	addAndMakeVisible(labelPeak);
	addAndMakeVisible(labelPeakValue);
	addAndMakeVisible(labelDropdownWindow);
	addAndMakeVisible(labelDropdownAxis);
	addAndMakeVisible(labelDropdownSize);
	addAndMakeVisible(comboboxWindowFunction);
	addAndMakeVisible(comboboxAxisType);
	addAndMakeVisible(comboboxSizeSetting);
	addAndMakeVisible(buttonExport);
	addAndMakeVisible(buttonSelectPlot1);
	addAndMakeVisible(buttonSelectPlot2);
	addAndMakeVisible(toggleButtonPlot1);
	addAndMakeVisible(toggleButtonPlot2);
	addAndMakeVisible(inputLowerBoundsX);
	addAndMakeVisible(inputUpperBoundsX);
	addAndMakeVisible(inputLowerBoundsY);
	addAndMakeVisible(inputUpperBoundsY);

	// Fonts
	labelImportAudio.setFont(juce::Font("Arial", 18.0f, juce::Font::bold));
	labelSelectTrace.setFont(juce::Font(17.0f));
	labelZoom.setFont(juce::Font("Arial", 18.0f, juce::Font::bold));
	labelUpperBounds.setFont(juce::Font(17.0f));
	labelLowerBounds.setFont(juce::Font(17.0f));
	labelUpperBoundsX.setFont(juce::Font(17.0f));
	labelUpperBoundsY.setFont(juce::Font(17.0f));
	labelLowerBoundsX.setFont(juce::Font(17.0f));
	labelLowerBoundsY.setFont(juce::Font(17.0f));
	labelExport.setFont(juce::Font("Arial", 18.0f, juce::Font::bold));
	labelCursor.setFont(juce::Font("Arial", 14.0f, juce::Font::bold));
	labelPeak.setFont(juce::Font("Arial", 14.0f, juce::Font::bold));
	labelDropdownWindow.setFont(juce::Font("Arial", 14.0f, juce::Font::bold));
	labelDropdownAxis.setFont(juce::Font("Arial", 14.0f, juce::Font::bold));
	labelDropdownSize.setFont(juce::Font("Arial", 14.0f, juce::Font::bold));

	// Text
	labelImportAudio.setText("Import Audio", juce::dontSendNotification);
	labelSelectTrace.setText("Selected Traces", juce::dontSendNotification);
	labelPlot1.setText("Plot 1", juce::dontSendNotification);
	labelPlot2.setText("Plot 2", juce::dontSendNotification);
	labelZoom.setText("Zoom", juce::dontSendNotification);
	labelUpperBounds.setText("Upper", juce::dontSendNotification);
	labelLowerBounds.setText("Lower", juce::dontSendNotification);
	labelUpperBoundsX.setText("X", juce::dontSendNotification);
	labelUpperBoundsY.setText("Y", juce::dontSendNotification);
	labelLowerBoundsX.setText("X", juce::dontSendNotification);
	labelLowerBoundsY.setText("Y", juce::dontSendNotification);
	labelExport.setText("Export", juce::dontSendNotification);
	labelCursor.setText("Cursor", juce::dontSendNotification);
	labelCursorValue.setText("(0.0, 0.00)", juce::dontSendNotification);
	labelPeak.setText("Peak", juce::dontSendNotification);
	labelDropdownWindow.setText("Function", juce::dontSendNotification);
	labelDropdownSize.setText("Size", juce::dontSendNotification);
	labelDropdownAxis.setText("Axis", juce::dontSendNotification);
	//inputXmin.setText(std::to_string(xMin), juce::dontSendNotification);
	inputLowerBoundsX.setText(std::to_string(xMinFrequency), juce::dontSendNotification);
	inputUpperBoundsX.setText(std::to_string(xMaxFrequency), juce::dontSendNotification);
	inputLowerBoundsY.setText(std::to_string(yMin), juce::dontSendNotification);
	inputUpperBoundsY.setText(std::to_string(yMax), juce::dontSendNotification);

	// Colour
	labelPlot1.setColour(juce::Label::textColourId, juce::Colours::darkgrey);
	labelPlot2.setColour(juce::Label::textColourId, juce::Colours::darkgrey);
	labelImportAudio.setColour(juce::Label::backgroundColourId, juce::Colours::darkgrey);
	labelZoom.setColour(juce::Label::backgroundColourId, juce::Colours::darkgrey);
	labelCursor.setColour(juce::Label::textColourId, juce::Colours::white);
	labelPeak.setColour(juce::Label::textColourId, juce::Colours::white);
	labelDropdownWindow.setColour(juce::Label::textColourId, juce::Colours::white);
	labelDropdownAxis.setColour(juce::Label::textColourId, juce::Colours::white);
	labelDropdownSize.setColour(juce::Label::textColourId, juce::Colours::white);
	labelExport.setColour(juce::Label::backgroundColourId, juce::Colours::darkgrey);
	comboboxWindowFunction.setColour(juce::ComboBox::backgroundColourId, juce::Colours::white);
	comboboxWindowFunction.setColour(juce::ComboBox::textColourId, juce::Colours::black);
	comboboxWindowFunction.setColour(juce::ComboBox::arrowColourId, juce::Colours::darkgrey);
	comboboxAxisType.setColour(juce::ComboBox::backgroundColourId, juce::Colours::white);
	comboboxAxisType.setColour(juce::ComboBox::textColourId, juce::Colours::black);
	comboboxAxisType.setColour(juce::ComboBox::arrowColourId, juce::Colours::darkgrey);
	comboboxSizeSetting.setColour(juce::ComboBox::backgroundColourId, juce::Colours::white);
	comboboxSizeSetting.setColour(juce::ComboBox::textColourId, juce::Colours::black);
	comboboxSizeSetting.setColour(juce::ComboBox::arrowColourId, juce::Colours::darkgrey);
	buttonExport.setColour(juce::TextButton::buttonColourId, juce::Colours::white);
	buttonExport.setColour(juce::TextButton::textColourOnId, juce::Colours::black);
	buttonExport.setColour(juce::TextButton::textColourOffId, juce::Colours::black);
	toggleButtonPlot1.setColour(juce::ToggleButton::tickColourId, juce::Colours::white);
	toggleButtonPlot1.setColour(juce::ToggleButton::tickDisabledColourId, juce::Colours::lightgrey);
	toggleButtonPlot2.setColour(juce::ToggleButton::tickColourId, juce::Colours::white);
	toggleButtonPlot2.setColour(juce::ToggleButton::tickDisabledColourId, juce::Colours::lightgrey);
	inputLowerBoundsX.setColour(juce::Label::backgroundColourId, juce::Colours::white);
	inputLowerBoundsX.setColour(juce::Label::textColourId, juce::Colours::black);
	inputLowerBoundsX.setColour(juce::Label::textWhenEditingColourId, juce::Colours::black);
	inputUpperBoundsX.setColour(juce::Label::backgroundColourId, juce::Colours::white);
	inputUpperBoundsX.setColour(juce::Label::textColourId, juce::Colours::black);
	inputUpperBoundsX.setColour(juce::Label::textWhenEditingColourId, juce::Colours::black);
	inputLowerBoundsY.setColour(juce::Label::backgroundColourId, juce::Colours::white);
	inputLowerBoundsY.setColour(juce::Label::textColourId, juce::Colours::black);
	inputLowerBoundsY.setColour(juce::Label::textWhenEditingColourId, juce::Colours::black);
	inputUpperBoundsY.setColour(juce::Label::backgroundColourId, juce::Colours::white);
	inputUpperBoundsY.setColour(juce::Label::textColourId, juce::Colours::black);
	inputUpperBoundsY.setColour(juce::Label::textWhenEditingColourId, juce::Colours::black);

	// Editable
	labelCursorValue.setEditable(false);
	inputLowerBoundsX.setEditable(true);
	inputUpperBoundsX.setEditable(true);
	inputLowerBoundsY.setEditable(true);
	inputUpperBoundsY.setEditable(true);
	labelPlot1.setEditable(false);
	labelPlot2.setEditable(false);

	// Combobox Items
	comboboxWindowFunction.addItem("Blackman window", 1);
	comboboxWindowFunction.addItem("Blackman-Harris window", 2);
	comboboxWindowFunction.addItem("Flatop window", 3);
	comboboxWindowFunction.addItem("Hamming window", 4);
	comboboxWindowFunction.addItem("Hann window", 5);
	comboboxWindowFunction.addItem("Kaiser", 6);
	comboboxWindowFunction.addItem("Rectangular window", 7);
	comboboxWindowFunction.addItem("Triangular window", 8);
	comboboxAxisType.addItem("Linear Frequency", 1);
	comboboxAxisType.addItem("Log Frequency", 2);
	comboboxSizeSetting.addItem("128", 1);
	comboboxSizeSetting.addItem("256", 2);
	comboboxSizeSetting.addItem("512", 3);
	comboboxSizeSetting.addItem("1024", 4);

	// Set Selections / Toggle State
	comboboxWindowFunction.setSelectedId(5);
	comboboxAxisType.setSelectedId(2);
	comboboxSizeSetting.setSelectedId(4);
	buttonSelectPlot1.setClickingTogglesState(true);
	buttonSelectPlot2.setClickingTogglesState(true);
	toggleButtonPlot1.setClickingTogglesState(true);
	toggleButtonPlot2.setClickingTogglesState(true);
	if (isVisiblePlot1 == true)
	{
		toggleButtonPlot1.setToggleState(true, true);
	}
	if (isVisiblePlot2 == true)
	{
		toggleButtonPlot2.setToggleState(true, true);
	}

	// On user interaction
	toggleButtonPlot1.onClick = [this] { updateToggleState(1); };
	toggleButtonPlot2.onClick = [this] { updateToggleState(2); };
	inputLowerBoundsX.onTextChange = [this] { getBounds(); };
	inputUpperBoundsX.onTextChange = [this] { getBounds(); };
	inputLowerBoundsY.onTextChange = [this] { getBounds(); };
	inputUpperBoundsY.onTextChange = [this] { getBounds(); };
	comboboxWindowFunction.onChange = [this] { setWindowFunction(); };
	comboboxAxisType.onChange = [this] { setAxisType(); };
	comboboxSizeSetting.onChange = [this] { setBlockSize(); };
	buttonSelectPlot1.onClick = [&]() {
		plotIndexSelection = 0;
		setPlotIndex(0);
	};
	buttonSelectPlot2.onClick = [&]() {
		plotIndexSelection = 1;
		setPlotIndex(1);
	};
}

FFTSpectrumAnalyzerAudioProcessorEditor::~FFTSpectrumAnalyzerAudioProcessorEditor()
{
}

//==============================================================================
void FFTSpectrumAnalyzerAudioProcessorEditor::paint(juce::Graphics& g)
{
	g.fillAll(juce::Colours::black);
	g.setOpacity(1.0f);
	g.setColour(juce::Colours::white);

	juce::Path plot1;
	juce::Path plot2;
	juce::Path xAxisMarkers;
	juce::Path yAxisMarkersUp;
	juce::Path yAxisMarkersDown;
	juce::Path zeroTick; 

	int cornerSizeSelectionBox = 3;
	int cornersizeCheckbox = 4;
	int tickWidth = 5;
	int zeroTickWidth = 15;

	int sampleSize = 100;  // Adjust the number of samples being displayed as needed
	int logScale = 40;
	int axisFontSize = 12;

	int widthSelectionBox = 263;
	int widthCheckbox = 16;
	int widthSelectionBoundary = 243;
	int widthZoomBoundary = 245;
	int widthWhiteBox = 180;
	int widthDrawingWindowBorder = getWidth() - bufferXComponent;
	int widthCursorBox = widthWhiteBox;
	int widthPeakBox = widthWhiteBox;
	int widthLeftBox = xMarginRightMenu + bufferDrawingWindowBorder;
	int widthRightBox = getWidth();
	int widthTopBox = getWidth();
	int widthBottomBox = getWidth();
	int widthLeftBox2 = xMarginRightMenu;
	int widthBottomBox2 = getWidth();

	int heightSelectionBoundary = 1;
	int heightCheckbox = 16;
	int heightSelectionBox = 90;
	int heightBottomMenu = 240; // bottom menu that contains cursor, peak, fucntion, size, axis dropdowns
	int heightWhiteBox = 26;
	int heightTopBox = 3;
	int heightBottomBox2 = 10;
	int heightZoomBoundary = heightSelectionBoundary;
	int heightPeakBox = heightWhiteBox;
	int heightCursorBox = heightWhiteBox;
	int heightLeftBox = getHeight();
	int heightRightBox = getHeight();
	int heightBottomBox = getHeight();
	int heightLeftBox2 = getHeight();
	int heightDrawingWindowBorder = getHeight() - heightBottomMenu;

	int lengthXAxis = getWidth() - xMarginRightMenu;
	int lengthYAxis = heightDrawingWindowBorder - bufferDrawingWindowBorder;

	int xMarginCheckboxFill = 16;
	int xMarginZoomBoundary = 2.5 * bufferXComponent;
	int xMarginSelectionBoundary = 2.5 * bufferXComponent;
	int xMarginLeftBox = xMarginOrigin;
	int xMarginTopBox = xMarginOrigin;
	int xMarginRightBox = widthDrawingWindowBorder;
	//int xMarginRightBox = widthDrawingWindowBorder + 0.5;
	int xMarginBottonBox = xMarginOrigin;
	int xMarginLeftBox2 = xMarginOrigin;
	int xMarginBottomBox2 = xMarginOrigin;
	int xMarginCursorBox = xMarginXYAxis + 138;
	int xMarginPeakBox = xMarginCursorBox + 205;

	int yMarginSelectionBox = 2;
	int yMarginCheckbox1Fill = 74;
	int yMarginSelectionBoundary = heightPrimaryCategoryLabel + bufferYPrimarySecondaryLabel + heightSecondaryLabel + (23 * yMarginSelectionBox);
	int yMarginCheckbox2Fill = yMarginCheckbox1Fill + bufferYCheckbox;
	int yMarginZoomBoundary = (119.5 * yMarginSelectionBox);
	int yMarginStartPlot = (bufferY + lengthYAxis) / 2;
	int yMarginXYAxis = bufferY + lengthYAxis - 1;
	int yMarginLeftBox = xMarginOrigin;
	int yMarginRightBox = xMarginOrigin;
	int yMarginTopBox = xMarginOrigin;
	int yMarginBottomBox = heightDrawingWindowBorder;
	int yMarginLeftBox2 = xMarginOrigin;
	int yMarginBottomBox2 = heightDrawingWindowBorder;
	int yMarginCursorBox = heightDrawingWindowBorder + 55;
	int yMarginPeakBox = yMarginCursorBox;

	if (newSelection == true) {
		audioProcessor.setStepSize(stepSize);                             //this needs to be changed when the size is changed
		sampleSelections[rowIndex] = audioProcessor.getAccumulationBuffer();
		audioProcessor.clearAccumulationBuffer();
		processBuffer();
		newSelection = false;
	}

	if (setToLog) {
		xMax = std::log10(xMaxFrequency);
		xMin = std::log10(xMinFrequency);
	}
	else {
		xMax = xMaxFrequency;
		xMin = xMinFrequency;
	}

	float xDiff = xMax - xMin;
	if (xDiff <= 0)  // handles divide by zero errors 
	{			
		if (xMin == 0 || xMax == 0) {
			xMax = xMaxPrev;
			xMin = xMinPrev;
			xDiff = xMaxPrev - xMinPrev;
			inputLowerBoundsX.setText(std::to_string(xMinPrev), juce::dontSendNotification);
			inputUpperBoundsX.setText(std::to_string(xMaxPrev), juce::dontSendNotification);
		}
	}
	else
	{
		xMaxPrev = xMax;
		xMinPrev = xMin;
	}
	float scaleX = lengthXAxis / xDiff;  // Scaling X increments; pixels shown per sample
	float xShift = -xMin * scaleX;

	float yDiff = yMax - yMin;
	if (yDiff <= 0)  // handles divide by zero errors
	{
		yMax = yMaxPrev;
		yMin = yMinPrev;
		yDiff = yMaxPrev - yMinPrev;
		inputLowerBoundsY.setText(std::to_string(yMinPrev), juce::dontSendNotification);
		inputUpperBoundsY.setText(std::to_string(yMaxPrev), juce::dontSendNotification);
	}
	else
	{
		yMaxPrev = yMax;
		yMinPrev = yMin;
	}
	float scaleY = -lengthYAxis / yDiff;  // Scaling Y increments; pixels shown per sample
	float yShift = (yDiff - 2.0f * yMax) * scaleY / 2.0f;

	float plotYShift = yMarginStartPlot + yShift;


	if (audioProcessor.minBlockSize) {
		if (setToLog == true) {
			plot2.startNewSubPath(xMarginXYAxis + xShift, yMarginStartPlot + logScale * std::log10(binMag[1][0]) * scaleY + yShift);
			plot1.startNewSubPath(xMarginXYAxis + xShift, yMarginStartPlot + logScale * std::log10(binMag[0][0]) * scaleY + yShift);
			for (int i = 1; i < indexToFreqMap.size(); i++)
			{
				if (isVisiblePlot2 == true) {
					plot2.lineTo(std::log10(indexToFreqMap[i]) * scaleX + xMarginXYAxis + xShift, logScale * std::log10(binMag[1][i]) * scaleY + plotYShift);
				}
				if (isVisiblePlot1 == true) {
					plot1.lineTo(std::log10(indexToFreqMap[i]) * scaleX + xMarginXYAxis + xShift, logScale * std::log10(binMag[0][i]) * scaleY + plotYShift);
				}
			}
		}
		else {
			plot2.startNewSubPath(xMarginXYAxis + xShift, yMarginStartPlot + logScale * std::log10(binMag[1][0]) * scaleY + yShift);
			plot1.startNewSubPath(xMarginXYAxis + xShift, yMarginStartPlot + logScale * std::log10(binMag[0][0]) * scaleY + yShift);
			for (int i = 1; i < indexToFreqMap.size(); i++)
			{
				if (isVisiblePlot2 == true) {
					plot2.lineTo(indexToFreqMap[i] * scaleX + xMarginXYAxis + xShift, logScale * std::log10(binMag[1][i]) * scaleY + plotYShift);
				}
				if (isVisiblePlot1 == true) {
					plot1.lineTo(indexToFreqMap[i] * scaleX + xMarginXYAxis + xShift, logScale * std::log10(binMag[0][i]) * scaleY + plotYShift);
				}
			}
		}

		g.setColour(juce::Colours::lightgreen);
		g.strokePath(plot2, juce::PathStrokeType(3.0f));
		g.setColour(juce::Colours::cornflowerblue);
		g.strokePath(plot1, juce::PathStrokeType(3.0f));
	}
	else {
		g.setColour(juce::Colours::black);
		g.fillRoundedRectangle(xMarginRightMenu, bufferYComponent, widthDrawingWindowBorder, heightDrawingWindowBorder, 3);
		g.setColour(juce::Colours::white);
		g.drawText("Not enough data selected", juce::Rectangle<int>(xMarginRightMenu, bufferYComponent, widthDrawingWindowBorder, heightDrawingWindowBorder), juce::Justification::centred, true);
	}

	// Plot X Axis Markers
	for (int i = 1; i <= xDiff; i++) {
		if (setToLog) {
			xAxisMarkers.startNewSubPath(xMarginXYAxis + (i * scaleX), yMarginXYAxis - tickWidth);
			xAxisMarkers.lineTo(xMarginXYAxis + (i * scaleX), yMarginXYAxis + tickWidth);
			int xLabelNum = std::pow(10, i);
			auto xLabel = juce::String(xLabelNum);
			g.setColour(juce::Colours::white);
			g.setFont(axisFontSize);
			g.drawText(xLabel + "hZ", juce::Rectangle<int>(xMarginXYAxis + (i * scaleX) - 10, yMarginXYAxis + 6, 60, 20), juce::Justification::centredLeft, true);
		}
		else {
			// set to linear
			int xDivLinear;
			if (xDiff <= 1000) {
				xDivLinear = i % 100;
				if (xDivLinear == 0) {
					xAxisMarkers.startNewSubPath(xMarginXYAxis + (i * scaleX), yMarginXYAxis - tickWidth);
					xAxisMarkers.lineTo(xMarginXYAxis + (i * scaleX), yMarginXYAxis + tickWidth);
					int xLabelNum = i;
					auto xLabel = juce::String(xLabelNum);
					g.setColour(juce::Colours::white);
					g.setFont(axisFontSize);
					g.drawText(xLabel + "hZ", juce::Rectangle<int>(xMarginXYAxis + (i * scaleX) - 10, yMarginXYAxis + 6, 60, 20), juce::Justification::centredLeft, true);
				}
			}
			if (xDiff > 1000 && xDiff <= 4000) {
				xDivLinear = i % 500;
				if (xDivLinear == 0) {
					xAxisMarkers.startNewSubPath(xMarginXYAxis + (i * scaleX), yMarginXYAxis - tickWidth);
					xAxisMarkers.lineTo(xMarginXYAxis + (i * scaleX), yMarginXYAxis + tickWidth);
					int xLabelNum = i;
					auto xLabel = juce::String(xLabelNum);
					g.setColour(juce::Colours::white);
					g.setFont(axisFontSize);
					g.drawText(xLabel + "hZ", juce::Rectangle<int>(xMarginXYAxis + (i * scaleX) - 10, yMarginXYAxis + 6, 60, 20), juce::Justification::centredLeft, true);
				}
			}
			else if (xDiff > 4000 && xDiff <= 9000) {
				xDivLinear = i % 1000;
				if (xDivLinear == 0) {
					xAxisMarkers.startNewSubPath(xMarginXYAxis + (i * scaleX), yMarginXYAxis - tickWidth);
					xAxisMarkers.lineTo(xMarginXYAxis + (i * scaleX), yMarginXYAxis + tickWidth);
					int xLabelNum = i;
					auto xLabel = juce::String(xLabelNum);
					g.setColour(juce::Colours::white);
					g.setFont(axisFontSize);
					g.drawText(xLabel + "hZ", juce::Rectangle<int>(xMarginXYAxis + (i * scaleX) - 10, yMarginXYAxis + 6, 60, 20), juce::Justification::centredLeft, true);
				}
			}
			else if (xDiff > 9000 && xDiff <= 16000) {
				xDivLinear = i % 2000;
				if (xDivLinear == 0) {
					xAxisMarkers.startNewSubPath(xMarginXYAxis + (i * scaleX), yMarginXYAxis - tickWidth);
					xAxisMarkers.lineTo(xMarginXYAxis + (i * scaleX), yMarginXYAxis + tickWidth);
					int xLabelNum = i;
					auto xLabel = juce::String(xLabelNum);
					g.setColour(juce::Colours::white);
					g.setFont(axisFontSize);
					g.drawText(xLabel + "hZ", juce::Rectangle<int>(yMarginXYAxis + (i * scaleX) - 10, yMarginXYAxis + 6, 60, 20), juce::Justification::centredLeft, true);
				}
			}
			else if (xDiff > 16000) {
				xDivLinear = i % 5000;
				if (xDivLinear == 0) {
					xAxisMarkers.startNewSubPath(xMarginXYAxis + (i * scaleX), yMarginXYAxis - tickWidth);
					xAxisMarkers.lineTo(xMarginXYAxis + (i * scaleX), yMarginXYAxis + tickWidth);
					int xLabelNum = i;
					auto xLabel = juce::String(xLabelNum);
					g.setColour(juce::Colours::white);
					g.setFont(axisFontSize);
					g.drawText(xLabel + "hZ", juce::Rectangle<int>(xMarginXYAxis + (i * scaleX) - 10, yMarginXYAxis + 6, 60, 20), juce::Justification::centredLeft, true);
				}
			}
		}
	}
	g.setColour(juce::Colours::white);
	g.strokePath(xAxisMarkers, juce::PathStrokeType(2.0f));

	// Plot Y Axis Markers
	for (int i = 1; i <= yDiff; i++) {
		int yDiv = i % 6;
		if (yDiv == 0) {
			yAxisMarkersUp.startNewSubPath(xMarginXYAxis - tickWidth, yMarginStartPlot + (scaleY * i) + yShift);
			yAxisMarkersUp.lineTo(xMarginXYAxis + tickWidth, yMarginStartPlot + (scaleY * i) + yShift);  // drawing line markers moving up from midpoint
			yAxisMarkersDown.startNewSubPath(xMarginXYAxis - tickWidth, yMarginStartPlot - (scaleY * i) + yShift);
			yAxisMarkersDown.lineTo(xMarginXYAxis + tickWidth, yMarginStartPlot - (scaleY * i) + yShift);  // drawing line markers moving up from midpoint
		}
	}
	g.setColour(juce::Colours::white);
	g.strokePath(yAxisMarkersUp, juce::PathStrokeType(2.0f));
	g.strokePath(yAxisMarkersDown, juce::PathStrokeType(2.0f));

	//Plot zero on Y-axis
	zeroTick.startNewSubPath(xMarginXYAxis - zeroTickWidth, yMarginStartPlot + yShift);
	zeroTick.lineTo(xMarginXYAxis + zeroTickWidth, yMarginStartPlot + yShift);
	g.strokePath(zeroTick, juce::PathStrokeType(3.0f));


	//** draw boxes to hide out of bound plots **//
	juce::Rectangle<int> leftPanel(xMarginLeftBox, yMarginLeftBox, widthLeftBox, heightLeftBox);
	juce::Rectangle<int> rightPanel(xMarginRightBox, yMarginRightBox, widthRightBox, heightRightBox);
	juce::Rectangle<int> topPanel(xMarginTopBox, yMarginTopBox, widthTopBox, heightTopBox);
	juce::Rectangle<int> bottomPanel(xMarginBottonBox, yMarginBottomBox, widthBottomBox, heightBottomBox);
	g.setColour(juce::Colours::black);
	g.fillRect(leftPanel);
	g.fillRect(rightPanel);
	g.fillRect(topPanel);
	g.fillRect(bottomPanel);

	//** draw graph border **//
	juce::Path graphBoundary;
	graphBoundary.startNewSubPath(xMarginXYAxis, bufferYComponent);
	graphBoundary.lineTo(widthDrawingWindowBorder, bufferYComponent);
	graphBoundary.lineTo(widthDrawingWindowBorder, heightDrawingWindowBorder);
	graphBoundary.lineTo(xMarginXYAxis, heightDrawingWindowBorder);
	graphBoundary.lineTo(xMarginXYAxis, bufferYComponent);
	g.setColour(juce::Colours::slategrey);
	g.strokePath(graphBoundary, juce::PathStrokeType(1.0f));

	//** draw boxes to hide out of bound plots 
	juce::Rectangle<int> leftPanel2(xMarginLeftBox2, yMarginLeftBox2, widthLeftBox2, heightLeftBox2);
	juce::Rectangle<int> bottomPanel2(xMarginBottomBox2, yMarginBottomBox2, widthBottomBox2, heightBottomBox2);

	g.setColour(juce::Colours::black);
	g.fillRect(leftPanel2);
	g.fillRect(bottomPanel2);

	//** line to separate left-side components and right-side components **/
	juce::Rectangle<int> LeftRightMenuSeparator (xMarginRightMenu, xMarginOrigin, lineHeight, windowMaxHeight);
	g.setColour(juce::Colours::darkgrey);
	g.fillRect(LeftRightMenuSeparator);

	// draw white box around selections
	g.setColour(juce::Colours::white);
	g.fillRoundedRectangle(bufferXComponent, yMarginSelectionBox, widthSelectionBox, heightSelectionBox, cornerSizeSelectionBox);

	// fill in checkboxes
	if (isVisiblePlot1 == true) {
		g.setColour(juce::Colours::dodgerblue);
		g.fillRoundedRectangle(xMarginCheckboxFill, yMarginCheckbox1Fill, widthCheckbox, heightCheckbox, cornersizeCheckbox);
	}
	if (isVisiblePlot2 == true) {
		g.setColour(juce::Colours::dodgerblue);
		g.fillRoundedRectangle(xMarginCheckboxFill, yMarginCheckbox2Fill, widthCheckbox, heightCheckbox, cornersizeCheckbox);
	}

	// draw line to separate plot selections
	g.setColour(juce::Colours::lightgrey);
	g.fillRect(xMarginSelectionBoundary, yMarginSelectionBoundary, widthSelectionBoundary, heightSelectionBoundary);

	//** line to separate upper and lower x/y bounds in ZOOM **//
	g.setColour(juce::Colours::darkgrey);
	g.fillRect(xMarginZoomBoundary, yMarginZoomBoundary, widthZoomBoundary, heightZoomBoundary);


	// Peak 
	float cursorYPeak = findPeak();
	if (cursorYPeak != 0) {
		g.setColour(juce::Colours::red);
		juce::Rectangle<int> peakLine(calculateX(setToLog, cursorPeak), bufferYComponent, 1, lengthYAxis);
		g.fillRect(peakLine);
		g.setColour(juce::Colours::white);
		if (setToLog) {
			if (isVisiblePlot1 || isVisiblePlot2)
				labelPeakValue.setText("(" + floatToStringPrecision(std::pow(10, screenToGraph(calculateX(setToLog, cursorPeak))), 2) + " Hz, " + floatToStringPrecision(cursorYPeak, 2) + " dB)", juce::dontSendNotification);
		}
		else {
			if(isVisiblePlot1 || isVisiblePlot2)
				labelPeakValue.setText("(" + floatToStringPrecision(screenToGraph(calculateX(setToLog, cursorPeak)), 2) + " Hz, " + floatToStringPrecision(cursorYPeak, 2) + " dB)", juce::dontSendNotification);
		}
	}
}

void FFTSpectrumAnalyzerAudioProcessorEditor::timerCallback()
{
	if (!isRunning && audioProcessor.getProcBlockCalled()) {
		isRunning = true;
	}
	else if (isRunning && !audioProcessor.getProcBlockCalled()) {
		isRunning = false;
		newSelection = true;
		audioProcessor.setInitialBlock();
		repaint();
	}
	audioProcessor.resetProcBlockCalled();
}

void FFTSpectrumAnalyzerAudioProcessorEditor::handleNewSelection(int numBins, int rowSize, int rowIndex)
{
	if (amountOfPlots == 0) {  //prepping all existing rows
		for (int r = 0; r < rowSize; r++) {
			audioProcessor.prepSelection(numBins, rowSize, r);
		}
	}

	else if (amountOfPlots > prevAmountOfPlots)
	{  //handling new row selection
		if (rowIndex > rowSize)
		{
			audioProcessor.prepSelection(numBins, rowSize, rowIndex);
		}
		else {
			return;
		}  //there is no selection made, return
	}
	prevAmountOfPlots = amountOfPlots;
	amountOfPlots++;
}

void FFTSpectrumAnalyzerAudioProcessorEditor::resized()
{
	int widthBorder = getWidth() - bufferXComponent;
	int widthDrawingWindowBorder = getWidth() - bufferXComponent;
	int widthButtonExport = 95;
	int widthButtonSelect = 90;
	int widthButtonToggle = 30;
	int widthComboBox = 160;
	int widthInputTextbox = 60;
	int widthLabelPlot = 50;

	int heightControlWidget = 24;
	int heightBottomMenu = 240;
	int heightDrawingWindowBorder = getHeight() - heightBottomMenu;
	int heightButtonExport = heightControlWidget + 4;
	int heightButtonSelect = heightControlWidget;
	int heightButtonToggle = heightControlWidget;
	int heightComboBox = 30;
	int heightInputTextbox = heightControlWidget - 2;
	int heightLabelPlot = heightControlWidget;

	int lengthXAxis = getWidth() - xMarginRightMenu;
	int lengthYAxis = heightDrawingWindowBorder - bufferDrawingWindowBorder;

	int xMarginDropdownMenus = xMarginRightMenu;
	int xMarginLabelCursor = xMarginDropdownMenus + 133;
	int xMarginLabelCursorValue = xMarginLabelCursor;
	int xMarginLabelPeak = xMarginLabelCursor + 205;
	int xMarginLabelDropdownWindow = xMarginDropdownMenus + 65;
	int xMarginLabelPeakValue = xMarginLabelPeak;
	int xMarginComboboxWindowFunction = xMarginLabelDropdownWindow + 4;
	int xMarginLabelDropdownAxis = xMarginComboboxWindowFunction + 180;
	int xMarginLabelDropdownSize = xMarginLabelDropdownAxis + 180;
	int xMarginComboboxAxisType = xMarginLabelDropdownAxis + 4;
	int xMarginComboboxSizeSetting = xMarginLabelDropdownSize + 4; 
	int xMarginToggleButtonPlot1 = 2 * bufferXComponent;
	int xMarginToggleButtonPlot2 = xMarginToggleButtonPlot1;
	int xMarginLabelPlot1 = 4 * xMarginToggleButtonPlot1;
	int xMarginLabelPlot2 = xMarginLabelPlot1;
	int xMarginButtonSelectPlot1 = 3.5 * xMarginLabelPlot1;
	int xMarginButtonSelectPlot2 = xMarginButtonSelectPlot1;
	int xMarginButtonExport = bufferXComponent;
	int xMarginLabelUpperBoundsX = 20.5 * bufferXComponent;
	int xMarginLabelUpperBoundsY = 40.5 * bufferXComponent;
	int xMarginInputUpperBoundsX = 10 * bufferXComponent;
	int xMarginInputUpperBoundsY = 30 * bufferXComponent;
	int xMarginInputLowerBoundsX = xMarginInputUpperBoundsX;
	int xMarginLabelLowerBoundsX = xMarginLabelUpperBoundsX;
	int xMarginInputLowerBoundsY = xMarginInputUpperBoundsY;
	int xMarginLabelLowerBoundsY = xMarginLabelUpperBoundsY;

	float yMarginXYAxis = bufferY + lengthYAxis - 1;
	//float yMarginPlot = (yBuffer + lengthYAxis) / 2;
	int yMarginLabelSelectTrace = heightPrimaryCategoryLabel + bufferYPrimarySecondaryLabel;
	int yMarginLabelZoom = yMarginLabelSelectTrace + (22.5 * bufferYComponent);
	int yMarginLabelExport = yMarginLabelSelectTrace + (42 * bufferYComponent);
	int yMarginLabelCursor = heightDrawingWindowBorder + 30;
	int yMarginLabelCursorValue = yMarginLabelCursor + 22;
	int yMarginLabelPeak = yMarginLabelCursor;
	int yNarginLabelPeakValue = yMarginLabelCursorValue;
	int yMarginLabelDropdownWindow = yMarginLabelPeak + 62;
	int yMarginComboboxWindowFunction = yMarginLabelDropdownWindow + 22;
	int yMarginLabelDropdownAxis = yMarginLabelDropdownWindow;
	int yMarginComboboxAxisType = yMarginComboboxWindowFunction;
	int yMarginLabelDropdownSize = yMarginLabelDropdownAxis;
	int yMarginComboboxSizeSetting = yMarginComboboxWindowFunction;
	int yMarginToggleButtonPlot1 = heightPrimaryCategoryLabel + bufferYPrimarySecondaryLabel + heightSecondaryLabel + (6 * yMarginSelectionBox);
	int yMarginToggleButtonPlot2 = yMarginToggleButtonPlot1 + (23 * yMarginSelectionBox);
	int yMarginLabelPlot1 = yMarginToggleButtonPlot1;
	int yMarginLabelPlot2 = yMarginToggleButtonPlot2;
	int yMarginButtonSelectPlot1 = yMarginToggleButtonPlot1;
	int yMarginButtonSelectPlot2 = yMarginToggleButtonPlot2;
	int yMarginButtonExport = yMarginLabelExport + heightPrimaryCategoryLabel + (1.5 * bufferYPrimarySecondaryLabel);
	int yMarginLabelUpperBounds = yMarginLabelZoom + heightPrimaryCategoryLabel + bufferYPrimarySecondaryLabel;
	int yMarginLabelLowerBounds = yMarginLabelUpperBounds + (8 * bufferYComponent);
	int yMarginLabelUpperBoundsX = yMarginLabelUpperBounds;
	int yMarginLabelUpperBoundsY = yMarginLabelUpperBounds;
	int yMarginInputUpperBoundsX = yMarginLabelUpperBounds + 2;
	int yMarginInputUpperBoundsY = yMarginLabelUpperBounds + 2;
	int yMarginInputLowerBoundsX = yMarginLabelLowerBounds + 2;
	int yMarginLabelLowerBoundsX = yMarginLabelLowerBounds;
	int yMarginInputLowerBoundsY = yMarginLabelLowerBounds + 2;
	int yMarginLabelLowerBoundsY = yMarginLabelLowerBounds;

	labelPlot1.setBounds(xMarginLabelPlot1, yMarginLabelPlot1, widthLabelPlot, heightLabelPlot);
	labelPlot2.setBounds(xMarginLabelPlot2, yMarginLabelPlot2, widthLabelPlot, heightLabelPlot);
	labelUpperBounds.setBounds(xMarginOrigin, yMarginLabelUpperBounds, widthSecondaryLabel, heightSecondaryLabel);
	labelLowerBounds.setBounds(xMarginOrigin, yMarginLabelLowerBounds, widthSecondaryLabel, heightSecondaryLabel);
	labelUpperBoundsX.setBounds(xMarginLabelUpperBoundsX, yMarginLabelUpperBoundsX, widthSecondaryLabel, heightSecondaryLabel);
	labelUpperBoundsY.setBounds(xMarginLabelUpperBoundsY, yMarginLabelUpperBoundsY, widthInputTextbox, heightInputTextbox);
	labelLowerBoundsX.setBounds(xMarginLabelLowerBoundsX, yMarginLabelLowerBoundsX, widthSecondaryLabel, heightSecondaryLabel);
	labelLowerBoundsY.setBounds(xMarginLabelLowerBoundsY, yMarginLabelLowerBoundsY, widthSecondaryLabel, heightSecondaryLabel);
	labelExport.setBounds(xMarginOrigin, yMarginLabelExport, xMarginRightMenu, heightPrimaryCategoryLabel);
	labelImportAudio.setBounds(xMarginOrigin, yMarginOrigin, xMarginRightMenu, heightPrimaryCategoryLabel);
	labelSelectTrace.setBounds(xMarginOrigin, yMarginLabelSelectTrace, widthSecondaryLabel, heightSecondaryLabel);
	labelZoom.setBounds(xMarginOrigin, yMarginLabelZoom, xMarginRightMenu, heightPrimaryCategoryLabel);
	labelCursor.setBounds(xMarginLabelCursor, yMarginLabelCursor, widthSecondaryLabel, heightSecondaryLabel);
	labelCursorValue.setBounds(xMarginLabelCursorValue, yMarginLabelCursorValue, widthSecondaryLabel, heightSecondaryLabel);
	labelPeak.setBounds(xMarginLabelPeak, yMarginLabelPeak, widthSecondaryLabel, heightSecondaryLabel);
	labelPeakValue.setBounds(xMarginLabelPeakValue, yNarginLabelPeakValue, widthSecondaryLabel, heightSecondaryLabel);
	labelDropdownWindow.setBounds(xMarginLabelDropdownWindow, yMarginLabelDropdownWindow, widthSecondaryLabel, heightSecondaryLabel);
	labelDropdownAxis.setBounds(xMarginLabelDropdownAxis, yMarginLabelDropdownAxis, widthSecondaryLabel, heightSecondaryLabel);
	labelDropdownSize.setBounds(xMarginLabelDropdownSize, yMarginLabelDropdownSize, widthSecondaryLabel, heightSecondaryLabel);
	comboboxWindowFunction.setBounds(xMarginComboboxWindowFunction, yMarginComboboxWindowFunction, widthComboBox, heightComboBox);
	comboboxAxisType.setBounds(xMarginComboboxAxisType, yMarginComboboxAxisType, widthComboBox, heightComboBox);
	comboboxSizeSetting.setBounds(xMarginComboboxSizeSetting, yMarginComboboxSizeSetting, widthComboBox, heightComboBox);

	inputUpperBoundsX.setBounds(xMarginInputUpperBoundsX, yMarginInputUpperBoundsX, widthInputTextbox, heightInputTextbox);
	inputUpperBoundsY.setBounds(xMarginInputUpperBoundsY, yMarginInputUpperBoundsY, widthInputTextbox, heightInputTextbox);
	inputLowerBoundsX.setBounds(xMarginInputLowerBoundsX, yMarginInputLowerBoundsX, widthInputTextbox, heightInputTextbox);
	inputLowerBoundsY.setBounds(xMarginInputUpperBoundsY, yMarginInputLowerBoundsY, widthInputTextbox, heightInputTextbox);
	
	buttonSelectPlot1.setBounds(xMarginButtonSelectPlot1, yMarginButtonSelectPlot1, widthButtonSelect, heightButtonSelect);
	buttonSelectPlot2.setBounds(xMarginButtonSelectPlot2, yMarginButtonSelectPlot2, widthButtonSelect, heightButtonSelect);
	buttonExport.setBounds(xMarginButtonExport, yMarginButtonExport, widthButtonExport, heightButtonExport);
	toggleButtonPlot1.setBounds(xMarginToggleButtonPlot1, yMarginToggleButtonPlot1, widthButtonToggle, heightButtonToggle);
	toggleButtonPlot2.setBounds(xMarginToggleButtonPlot2, yMarginToggleButtonPlot2, widthButtonToggle, heightButtonToggle);

}

void FFTSpectrumAnalyzerAudioProcessorEditor::setFreqData(int fftData) {
	numBins = fftSize / 2 + 1;
	numFreqBins = fftSize / 2;
	stepSize = fftSize / 2;
	//indexToFreqMap.resize(numBins);
	binMag.resize(rowSize, std::vector<float>(numBins, std::numeric_limits<float>::epsilon()));
}

void FFTSpectrumAnalyzerAudioProcessorEditor::getBounds()
{
	float minVal = indexToFreqMap[0];
	float maxVal = 24000;
	float yMaxVal = 12;
	float yMinVal = -96;
	juce::String temp = inputLowerBoundsX.getText(false);
	float val = std::atof(temp.toStdString().c_str());
	if (setToLog) {
		if (val >= minVal && val <= maxVal)
		{
			if (val > 0) {
				xMinFrequency = val;
			}
		}
		else { inputLowerBoundsX.setText(std::to_string(xMin), juce::dontSendNotification); }
	}
	else {
		if (val >= minVal && val <= maxVal)
		{
			xMinFrequency = val;
		}
		else { inputLowerBoundsX.setText(std::to_string(xMin), juce::dontSendNotification); }
	}

	temp = inputUpperBoundsX.getText(false);
	val = std::atof(temp.toStdString().c_str());
	if (val >= minVal && val <= maxVal)
	{
		if (val != 0) {
			xMaxFrequency = val;
		}
	}
	else { inputUpperBoundsX.setText(std::to_string(xMax), juce::dontSendNotification); }

	temp = inputLowerBoundsY.getText(false);
	val = std::atof(temp.toStdString().c_str());
	if (val >= yMinVal && val <= yMaxVal)
	{
		yMin = val;
	}
	else { inputLowerBoundsY.setText(std::to_string(yMin), juce::dontSendNotification); }

	temp = inputUpperBoundsY.getText(false);
	val = std::atof(temp.toStdString().c_str());
	if (val >= yMinVal && val <= yMaxVal)
	{
		yMax = val;
	}
	else { inputUpperBoundsY.setText(std::to_string(yMax), juce::dontSendNotification); }
	repaint();
}


void FFTSpectrumAnalyzerAudioProcessorEditor::setPlotIndex(int plotIndex)
{
	rowIndex = plotIndex;
	audioProcessor.setRowIndex(rowIndex);
	if (plotIndex == 0)
	{
		buttonSelectPlot1.setButtonText("Selected");
		buttonSelectPlot2.setButtonText("Select");
	}
	else if (plotIndex == 1)
	{
		buttonSelectPlot2.setButtonText("Selected");
		buttonSelectPlot1.setButtonText("Select");
	}
}

void FFTSpectrumAnalyzerAudioProcessorEditor::updateToggleState(int plotId)
{
	if (plotId == 1)
	{
		setVisibility(1);
	}
	else
		setVisibility(2);
	repaint();
}


void FFTSpectrumAnalyzerAudioProcessorEditor::setVisibility(int plotId)
{
	if (plotId == 1)
	{
		if (isVisiblePlot1 == false) {
			isVisiblePlot1 = true;
		}
		else if (isVisiblePlot1 == true) {
			isVisiblePlot1 = false;
		}
	}
	else
	{
		if (isVisiblePlot2 == false) {
			isVisiblePlot2 = true;
		}
		else if (isVisiblePlot2 == true) {
			isVisiblePlot2 = false;
		}
	}
}

void FFTSpectrumAnalyzerAudioProcessorEditor::setWindowFunction() {
	juce::dsp::WindowingFunction<float>::WindowingMethod newWindow;
	switch (comboboxWindowFunction.getSelectedId()) {
	case 1:
		newWindow = juce::dsp::WindowingFunction<float>::WindowingMethod::blackman;
		setWindow(newWindow);
		repaint();
		break;
	case 2:
		newWindow = juce::dsp::WindowingFunction<float>::WindowingMethod::blackmanHarris;
		setWindow(newWindow);
		repaint();
		break;
	case 3:
		newWindow = juce::dsp::WindowingFunction<float>::WindowingMethod::flatTop;
		setWindow(newWindow);
		repaint();
		break;
	case 4:
		newWindow = juce::dsp::WindowingFunction<float>::WindowingMethod::hamming;
		setWindow(newWindow);
		repaint();
		break;
	case 5:
		newWindow = juce::dsp::WindowingFunction<float>::WindowingMethod::hann;
		setWindow(newWindow);
		repaint();
		break;
	case 6:
		newWindow = juce::dsp::WindowingFunction<float>::WindowingMethod::kaiser;
		setWindow(newWindow);
		repaint();
		break;
	case 7:
		newWindow = juce::dsp::WindowingFunction<float>::WindowingMethod::rectangular;
		setWindow(newWindow);
		repaint();
		break;
	case 8:
		newWindow = juce::dsp::WindowingFunction<float>::WindowingMethod::triangular;
		setWindow(newWindow);
		repaint();
		break;
	}

}

void FFTSpectrumAnalyzerAudioProcessorEditor::setBlockSize() {
	auto selection = comboboxSizeSetting.getText();
	fftSize = selection.getIntValue();
	setFreqData(fftSize);
	repaint();
}

void FFTSpectrumAnalyzerAudioProcessorEditor::setAxisType() {
	if (setToLog == true) {
		setToLog = false;
		repaint();
	}
	else {
		setToLog = true;
		repaint();
	}
}

void FFTSpectrumAnalyzerAudioProcessorEditor::setWindow(juce::dsp::WindowingFunction<float>::WindowingMethod type) {
	juce::dsp::WindowingFunction<float> window(fftSize, type);
	window.fillWindowingTables(fftSize, type);
}

void FFTSpectrumAnalyzerAudioProcessorEditor::processBuffer() {

	zeroBuffers();

	int bufferShift = 0;

	juce::dsp::FFT forwardFFT(std::log2(fftSize));

	int sampleRate = audioProcessor.getBlockSampleRate();

	maxFreq = sampleRate / 2;
	//x variable for labeling
	for (int i = 0; i < numBins; i++) {
		indexToFreqMap[i] = i * ((float)maxFreq / (float)numFreqBins);
	}

	while (bufferShift <= sampleSelections[rowIndex].size() - stepSize) {
		//while (buffSize >= numFreqBins) {

		std::copy(bufferLeft.begin(), bufferLeft.begin() + stepSize, bufferLeft.begin() + stepSize);
		std::copy(bufferRight.begin() + stepSize, bufferRight.end(), bufferLeft.begin());
		std::copy(bufferRight.begin(), bufferRight.begin() + stepSize, bufferRight.begin() + stepSize);

		std::copy(sampleSelections[rowIndex].begin() + bufferShift, sampleSelections[rowIndex].begin() + (bufferShift + stepSize), bufferRight.begin());
		//buffer.read(bufferRight.data(), numFreqBins);
		std::copy(bufferRight.begin(), bufferRight.end(), windowBufferRight.begin());
		windowBufferLeft = bufferLeft;
		editWindow.multiplyWithWindowingTable(windowBufferRight.data(), fftSize);
		editWindow.multiplyWithWindowingTable(windowBufferLeft.data(), fftSize);
		forwardFFT.performRealOnlyForwardTransform(windowBufferRight.data(), true);
		fftCounter++;

		for (int i = 0; i < numBins; i++) {
			binMag[rowIndex][i] += sqrt(pow(windowBufferRight[2 * i], 2) + pow(windowBufferRight[2 * i + 1], 2)) / numFreqBins;
		}
		bufferShift += numFreqBins;
	}
	if (fftCounter != 0)
	{
		for (int i = 0; i < numBins; i++) {
			binMag[rowIndex][i] /= fftCounter;
		}
	}
}

void FFTSpectrumAnalyzerAudioProcessorEditor::zeroBuffers() {
	bufferLeft.resize(fftSize);
	std::fill(bufferLeft.begin(), bufferLeft.end(), 0.0f);
	bufferRight.resize(fftSize);
	std::fill(bufferRight.begin(), bufferRight.end(), 0.0f);
	windowBufferRight.resize(fftSize * 2);
	std::fill(windowBufferRight.begin(), windowBufferRight.end(), 0.0f);
	windowBufferLeft.resize(fftSize);
	std::fill(windowBufferLeft.begin(), windowBufferLeft.end(), 0.0f);
	binMag[rowIndex].resize(numBins);
	std::fill(binMag[rowIndex].begin(), binMag[rowIndex].end(), 0.0f);
	indexToFreqMap.resize(numBins);
	std::fill(indexToFreqMap.begin(), indexToFreqMap.end(), 0.0f);
	fftCounter = 0;
}


void FFTSpectrumAnalyzerAudioProcessorEditor::mouseMove(const juce::MouseEvent& event)
{
	float graphWest = (getWidth() * 0.295) - 1;
	float graphNorth = bufferYComponent;
	float graphEast = (getWidth() - bufferXComponent) + 0.5;
	float graphSouth = (getHeight() - 240) * 0.95;

	cursorX = event.getMouseDownX();
	float cursorY = event.getMouseDownY();
	//invalid bounds
	if (cursorX < graphWest || cursorX > graphEast || cursorY < graphNorth || cursorY > graphSouth) {
		labelCursorValue.setText("(0.0 Hz, 0.00 dB)", juce::dontSendNotification);
	}
	//valid bounds
	else {
		if (audioProcessor.minBlockSize) {
			//get index based on cursor
			int i = 1;
			while (calculateX(setToLog, i) < cursorX) {
				i++;
			}
			if (setToLog) {
				float xCoord = std::pow(10, screenToGraph(cursorX));
				if (isVisiblePlot2 || isVisiblePlot1) {
					labelCursorValue.setText("(" + floatToStringPrecision(xCoord, 1) + " Hz, " + floatToStringPrecision(getYCoord(plotIndexSelection, setToLog, i), 2) + " dB)", juce::dontSendNotification);
				}
			}
			else {
				float xCoord = screenToGraph(cursorX);
				if (isVisiblePlot2 || isVisiblePlot1) {
					labelCursorValue.setText("(" + floatToStringPrecision(xCoord, 1) + " Hz, " + floatToStringPrecision(getYCoord(plotIndexSelection, setToLog, i), 2) + " dB)", juce::dontSendNotification);
				}
			}
		}
	}
	repaint();
}

float FFTSpectrumAnalyzerAudioProcessorEditor::calculateX(bool log, int index) {
	float start = getWidth() * 0.295 - 1;
	float lengthAxis = getWidth() - bufferXComponent;
	float range = xMax - xMin;
	float ratio = lengthAxis / range;
	float shift = -xMin * ratio;

	if (log) {
		return std::log10(indexToFreqMap[index]) * ratio + start + shift;
	}
	else {
		return indexToFreqMap[index] * ratio + start + shift;
	}
}

int FFTSpectrumAnalyzerAudioProcessorEditor::findPeak() {
	int i = 0;
	float maxValue = -1000;
	float temp;
	while (i < numBins - 1) {
		temp = getYCoord(plotIndexSelection, setToLog, i);
		if (temp > maxValue) {
			maxValue = temp;
			cursorPeak = i;
		}
		i++;
	}
	return maxValue;
}

float FFTSpectrumAnalyzerAudioProcessorEditor::getYCoord(int plotNumber, bool log, int index) {
	return 40 * std::log10(binMag[plotNumber][index]);
}

float FFTSpectrumAnalyzerAudioProcessorEditor::screenToGraph(float screenCoord) {

	float start = getWidth() * 0.295 - 1;
	float lengthAxis = getWidth() - bufferXComponent;
	float range = xMax - xMin;
	float ratio = lengthAxis / range;
	float shift = -xMin * ratio;
	
	return (screenCoord + shift - start) / ratio;
}

float FFTSpectrumAnalyzerAudioProcessorEditor::graphToScreen(int graphCoord) {
	float start = getWidth() * 0.295 - 1;
	float lengthAxis = getWidth() - bufferXComponent;
	float range = xMax - xMin;
	float ratio = lengthAxis / range;
	float shift = -xMin * ratio;

	return (graphCoord * ratio) + start - shift;
}

std::string FFTSpectrumAnalyzerAudioProcessorEditor::floatToStringPrecision(float f, int p)
{
	std::ostringstream oss;
	oss << std::fixed << std::setprecision(p) << f;
	return oss.str();
}