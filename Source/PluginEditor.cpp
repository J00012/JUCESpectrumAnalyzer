/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <string>
#include <algorithm>

bool FFTSpectrumAnalyzerAudioProcessorEditor::isRunning = false;
bool FFTSpectrumAnalyzerAudioProcessorEditor::newSelection = false;
bool FFTSpectrumAnalyzerAudioProcessorEditor::isVisiblePlot1 = true;
bool FFTSpectrumAnalyzerAudioProcessorEditor::isVisiblePlot2 = true;
bool FFTSpectrumAnalyzerAudioProcessorEditor::setToLog = false;
bool FFTSpectrumAnalyzerAudioProcessorEditor::conCall = true;
bool FFTSpectrumAnalyzerAudioProcessorEditor::blockProcessed = false;
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
float FFTSpectrumAnalyzerAudioProcessorEditor::peakRange = yMax - yMin;
float FFTSpectrumAnalyzerAudioProcessorEditor::cursorX;

int FFTSpectrumAnalyzerAudioProcessorEditor::cursorPeak = 0;
int FFTSpectrumAnalyzerAudioProcessorEditor::plotIndexSelection = 0;
int FFTSpectrumAnalyzerAudioProcessorEditor::windowWidth = 950;
int FFTSpectrumAnalyzerAudioProcessorEditor::windowHeight = 550 + 2;
int FFTSpectrumAnalyzerAudioProcessorEditor::windowMaxWidth = 2160;
int FFTSpectrumAnalyzerAudioProcessorEditor::windowMaxHeight = 1080;

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
int FFTSpectrumAnalyzerAudioProcessorEditor::windowVar = 0;
char FFTSpectrumAnalyzerAudioProcessorEditor::axis = 'x';


//Processor vectors

//i changed index to freq map
std::vector<float> FFTSpectrumAnalyzerAudioProcessorEditor::indexToFreqMap;
std::vector< std::vector<float>> FFTSpectrumAnalyzerAudioProcessorEditor::binMag;
std::vector< std::vector<float>> FFTSpectrumAnalyzerAudioProcessorEditor::sampleSelections;
std::vector<float> FFTSpectrumAnalyzerAudioProcessorEditor::bufferRight = { 0 };
std::vector<float> FFTSpectrumAnalyzerAudioProcessorEditor::bufferLeft = { 0 };
std::vector<float> FFTSpectrumAnalyzerAudioProcessorEditor::windowBufferRight = { 0 };
std::vector<float> FFTSpectrumAnalyzerAudioProcessorEditor::windowBufferLeft = { 0 };
std::vector<std::string> FFTSpectrumAnalyzerAudioProcessorEditor::numPlots = {};
std::vector<std::string> FFTSpectrumAnalyzerAudioProcessorEditor::windowFunctionTypes = { "Blackman window","Blackman-Harris window", "Flatop window", "Hamming window", "Hann window", "Kaiser", "Rectangular window", "Triangular window" };
std::vector<std::string> FFTSpectrumAnalyzerAudioProcessorEditor::axisTypes = { "Linear Frequency", "Logarithmic Frequency"};
std::vector<std::string> FFTSpectrumAnalyzerAudioProcessorEditor::sizeOptions = { "128", "256", "512", "1024", "2048", "4096", "8192", "16384"};
//juce::dsp::FFT FFTSpectrumAnalyzerAudioProcessorEditor::editFFT(0);			  
																				  
FFTSpectrumAnalyzerAudioProcessorEditor::plotItem FFTSpectrumAnalyzerAudioProcessorEditor::plotInfo[7] = {
	{false, juce::Colours::lightgreen, juce::Path(), 74},
	{false, juce::Colours::cornflowerblue,juce::Path(), 120},
	{false, juce::Colours::purple},
	{false, juce::Colours::cyan},
	{false, juce::Colours::coral},
	{false, juce::Colours::goldenrod},
	{false, juce::Colours::slateblue}
};

juce::dsp::WindowingFunction<float> FFTSpectrumAnalyzerAudioProcessorEditor::windowData(windowVar, juce::dsp::WindowingFunction<float>::hann);

//==============================================================================
FFTSpectrumAnalyzerAudioProcessorEditor::FFTSpectrumAnalyzerAudioProcessorEditor(FFTSpectrumAnalyzerAudioProcessor& p)
	: AudioProcessorEditor(&p), audioProcessor(p)
{
	setOpaque(true);
	startTimer(500);

	setSize(widthWindowMin, heightWindowMin);
	setResizable(true, true);
	setResizeLimits(widthWindowMin, heightWindowMin, widthWindowMax, heightWindowMax);

	sampleSelections.resize(2);
	setPlotIndex(rowIndex);
	setFreqData(fftSize);
	initializeBinMag();
	//audioProcessor.zeroAllSelections(numBins, rowSize);
	//audioProcessor.prepBuffers(fftS);
	//binMag = audioProcessor.getBinSet();
	zeroBuffers();

	//for (int i = 0; i <= numPlots.size(); i++) {
	//	plotToggleButtons.push_back("toggleButtonPlot" + (std::to_string(i + 1)));
	//	plotSelectButtons.push_back("buttonSelectPlot" + (std::to_string(i + 1)));
	//	plotLabels.push_back("labelPlot" + (std::to_string(i + 1)));
	//}

	//for (int i = 1; i <= numPlots.size(); i++) {
	//	juce::Label std::to_string(plotLabels[i]{ "Plot " + i });
	//	juce::ToggleButton plotToggleButtons[i];
	//}


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
	addAndMakeVisible(inputLowerBoundsX);
	addAndMakeVisible(inputUpperBoundsX);
	addAndMakeVisible(inputLowerBoundsY);
	addAndMakeVisible(inputUpperBoundsY);
	addAndMakeVisible(comboboxWindowFunction);
	addAndMakeVisible(comboboxAxisType);
	addAndMakeVisible(comboboxSizeSetting);
	addAndMakeVisible(buttonExport);
	addAndMakeVisible(buttonSelectPlot1);
	addAndMakeVisible(buttonSelectPlot2);
	addAndMakeVisible(buttonSelectPlot3);
	addAndMakeVisible(buttonSelectPlot4);
	addAndMakeVisible(buttonSelectPlot5);
	addAndMakeVisible(buttonSelectPlot6);
	addAndMakeVisible(buttonSelectPlot7);
	addAndMakeVisible(toggleButtonPlot1);
	addAndMakeVisible(toggleButtonPlot2);
	addAndMakeVisible(toggleButtonPlot3);
	addAndMakeVisible(toggleButtonPlot4);
	addAndMakeVisible(toggleButtonPlot5);
	addAndMakeVisible(toggleButtonPlot6);
	addAndMakeVisible(toggleButtonPlot7);

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
	labelPlot3.setText("Plot 3", juce::dontSendNotification);
	labelPlot4.setText("Plot 4", juce::dontSendNotification);
	labelPlot5.setText("Plot 5", juce::dontSendNotification);
	labelPlot6.setText("Plot 6", juce::dontSendNotification);
	labelPlot7.setText("Plot 7", juce::dontSendNotification);
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
	toggleButtonPlot1.setColour(juce::ToggleButton::tickColourId, juce::Colours::black);
	toggleButtonPlot1.setColour(juce::ToggleButton::tickDisabledColourId, juce::Colours::lightgrey);
	toggleButtonPlot2.setColour(juce::ToggleButton::tickColourId, juce::Colours::black);
	toggleButtonPlot2.setColour(juce::ToggleButton::tickDisabledColourId, juce::Colours::lightgrey);
	toggleButtonPlot3.setColour(juce::ToggleButton::tickColourId, juce::Colours::black);
	toggleButtonPlot3.setColour(juce::ToggleButton::tickDisabledColourId, juce::Colours::lightgrey);
	toggleButtonPlot4.setColour(juce::ToggleButton::tickColourId, juce::Colours::black);
	toggleButtonPlot4.setColour(juce::ToggleButton::tickDisabledColourId, juce::Colours::lightgrey);
	toggleButtonPlot5.setColour(juce::ToggleButton::tickColourId, juce::Colours::black);
	toggleButtonPlot5.setColour(juce::ToggleButton::tickDisabledColourId, juce::Colours::lightgrey);
	toggleButtonPlot6.setColour(juce::ToggleButton::tickColourId, juce::Colours::black);
	toggleButtonPlot6.setColour(juce::ToggleButton::tickDisabledColourId, juce::Colours::lightgrey);
	toggleButtonPlot7.setColour(juce::ToggleButton::tickColourId, juce::Colours::black);
	toggleButtonPlot1.setColour(juce::ToggleButton::tickDisabledColourId, juce::Colours::lightgrey);
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
	for (int i = 0; i < windowFunctionTypes.size(); i++) {
		comboboxWindowFunction.addItem(windowFunctionTypes[i], i + 1);
	}
	for (int i = 0; i < axisTypes.size(); i++) {
		comboboxAxisType.addItem(axisTypes[i], i + 1);
	}
	for (int i = 0; i < sizeOptions.size(); i++) {
		comboboxSizeSetting.addItem(sizeOptions[i], i + 1);
	}

	// Set Selections / Toggle State
	comboboxWindowFunction.setSelectedId(5);
	comboboxAxisType.setSelectedId(2);
	comboboxSizeSetting.setSelectedId(4);
	buttonSelectPlot1.setClickingTogglesState(true);
	buttonSelectPlot2.setClickingTogglesState(true);
	buttonSelectPlot3.setClickingTogglesState(true);
	buttonSelectPlot4.setClickingTogglesState(true);
	buttonSelectPlot5.setClickingTogglesState(true);
	buttonSelectPlot5.setClickingTogglesState(true);
	buttonSelectPlot6.setClickingTogglesState(true);
	buttonSelectPlot7.setClickingTogglesState(true);
	toggleButtonPlot1.setClickingTogglesState(true);
	toggleButtonPlot2.setClickingTogglesState(true);
	toggleButtonPlot3.setClickingTogglesState(true);
	toggleButtonPlot4.setClickingTogglesState(true);
	toggleButtonPlot5.setClickingTogglesState(true);
	toggleButtonPlot6.setClickingTogglesState(true);
	toggleButtonPlot7.setClickingTogglesState(true);

	// On user interaction
	inputLowerBoundsX.onTextChange = [this] { getBounds(); };
	inputUpperBoundsX.onTextChange = [this] { getBounds(); };
	inputLowerBoundsY.onTextChange = [this] { getBounds(); };
	inputUpperBoundsY.onTextChange = [this] { getBounds(); };
	comboboxWindowFunction.onChange = [this] { setWindowFunction();
	if (blockProcessed) {
		int temp = rowIndex;
		for (int i = 0; i < rowSize; i++) {
			if (sampleSelections[i].size() != 0) {  //check if there is data in acc buffer
				rowIndex = i;
				processBuffer();
			}
		}
		rowIndex = temp;
	}
	repaint(); 
	};
	comboboxAxisType.onChange = [this] { setAxisType(); };
	comboboxSizeSetting.onChange = [this] { setBlockSize(); zeroBuffers();
	if (blockProcessed) {
		int temp = rowIndex;
		for (int i = 0; i < rowSize; i++) {
			if (sampleSelections[i].size() != 0) {   //check if there is data in acc buffer
				rowIndex = i;
				processBuffer();
			}
		}
		rowIndex = temp;
	}
	repaint();
	};

	toggleButtonPlot1.onClick = [this] { setPlotVisibility(0); };
	toggleButtonPlot2.onClick = [this] { setPlotVisibility(1); };
	toggleButtonPlot3.onClick = [this] { setPlotVisibility(2); };
	toggleButtonPlot4.onClick = [this] { setPlotVisibility(3); };
	toggleButtonPlot5.onClick = [this] { setPlotVisibility(4); };
	toggleButtonPlot6.onClick = [this] { setPlotVisibility(5); };
	toggleButtonPlot7.onClick = [this] { setPlotVisibility(6); };

	buttonSelectPlot1.onClick = [&]() {
		plotIndexSelection = 0;
		setPlotIndex(0);
		};
	buttonSelectPlot2.onClick = [&]() {
		plotIndexSelection = 1;
		setPlotIndex(1);
		};

	if (plotInfo[0].isVisible == true)
	{
		toggleButtonPlot1.setToggleState(true, true);
	}
	if (plotInfo[1].isVisible == true)
	{
		toggleButtonPlot2.setToggleState(true, true);
	}
	if (plotInfo[2].isVisible == true)
	{
		toggleButtonPlot3.setToggleState(true, true);
	}
	if (plotInfo[3].isVisible == true)
	{
		toggleButtonPlot4.setToggleState(true, true);
	}
	if (plotInfo[4].isVisible == true)
	{
		toggleButtonPlot5.setToggleState(true, true);
	}
	if (plotInfo[5].isVisible == true)
	{
		toggleButtonPlot6.setToggleState(true, true);
	}
	if (plotInfo[6].isVisible == true)
	{
		toggleButtonPlot7.setToggleState(true, true);
	}

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
	juce::Path xAxis;
	juce::Path xAxisMarkers;
	juce::Path yAxis;
	juce::Path yAxisMarkersUp;
	juce::Path yAxisMarkersDown;
	juce::Path zeroTick; 

	int sampleSize = 100;  // Adjust the number of samples being displayed as needed
	int axisFontSize = 12;

	const char* labelTextX = "Hz";
	const char* labelTextY = "dB";

	int lengthXAxis = getAxisLength('x');
	int lengthYAxis = getAxisLength('y');

	int widthRightTopBottomBox = getWidth();
	int widthDrawingWindow = lengthXAxis;

	int heightWhiteBox = heightMediumWidget + paddingSmall;
	int heightLeftRightBottomBox = getHeight();
	int heightDrawingWindow = lengthYAxis;

	int xMarginRightBox = xMarginXYAxis + lengthXAxis;

	int yMarginZeroTick = (paddingSmall + lengthYAxis) / 2;
	int yMarginDrawingWindowUpperBorder = paddingSmall + paddingExtraSmall;
	int yMarginDrawingWindowLowerBorder = lengthYAxis + paddingSmall;

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

	float plotYShift = yMarginZeroTick + yShift;


	// Graph plots
	if (audioProcessor.minBlockSize) {
		for (int i = 0; i < rowSize; i++) {

			if (plotInfo[i].isVisible == true) {
				plotInfo[i].path.clear();
				plotInfo[i].path.startNewSubPath(xMarginXYAxis + xShift, yMarginZeroTick + logScale * std::log10(binMag[i][0]) * scaleY + yShift);

				if (setToLog == true) {
					//if (plotInfo[i].isVisible == true) {
					for (int j = 1; j < indexToFreqMap.size(); j++) {
						plotInfo[i].path.lineTo(std::log10(indexToFreqMap[j]) * scaleX + xMarginXYAxis + xShift, logScale * std::log10(binMag[i][j]) * scaleY + plotYShift);
					}
				}
				else {
					//if (plotInfo[i].isVisible == true) {
					for (int j = 1; j < indexToFreqMap.size(); j++) {
						plotInfo[i].path.lineTo(indexToFreqMap[j] * scaleX + xMarginXYAxis + xShift, logScale * std::log10(binMag[i][j]) * scaleY + plotYShift);
					}
				}
				g.setColour(plotInfo[i].color);
				g.strokePath(plotInfo[i].path, juce::PathStrokeType(3.0f));
			}
		}
	}
	else {
		g.setColour(juce::Colours::darkgrey);
		juce::Rectangle<int> RectangleDataSizeErrorMessage(xMarginXYAxis, paddingSmall, lengthXAxis, lengthYAxis);
		g.fillRect(RectangleDataSizeErrorMessage);
		g.setColour(juce::Colours::white);
		g.drawText("Not enough data selected", RectangleDataSizeErrorMessage, juce::Justification::centred, true);
	}

	//** draw boxes to hide out of bound plots **//
	juce::Rectangle<int> leftPanel(xMarginOrigin, xMarginOrigin, xMarginXYAxis, heightLeftRightBottomBox);
	juce::Rectangle<int> rightPanel(xMarginRightBox, xMarginOrigin, widthRightTopBottomBox, heightLeftRightBottomBox);
	juce::Rectangle<int> topPanel(xMarginOrigin, xMarginOrigin, widthRightTopBottomBox, paddingSmall);
	juce::Rectangle<int> bottomPanel(xMarginOrigin, yMarginDrawingWindowLowerBorder, widthRightTopBottomBox, heightLeftRightBottomBox);
	g.setColour(juce::Colours::black);
	g.fillRect(leftPanel);
	g.fillRect(rightPanel);
	g.fillRect(topPanel);
	g.fillRect(bottomPanel);


	// Plot X Axis Markers
	axis = 'x';
	g.setColour(juce::Colours::white);
	g.setFont(axisFontSize);
	for (int i = 1; i <= xDiff; i++) {
		float xMarginLabelBounds = xMarginXYAxis + (i * scaleX);
		if (setToLog) {
			int xLabelAxisNum = std::pow(logPower, i);
			auto xLabelAxisNumText = juce::String(xLabelAxisNum) + labelTextX;
			writeAxisLabels(g, xAxisMarkers, xLabelAxisNumText, xMarginLabelBounds, yMarginDrawingWindowLowerBorder, scaleTextOffsetX, axis);
		}
		else {
			// set to linear
			int xDivLinear;
			auto xLabelAxisNumText = juce::String(i) + labelTextX;
			if (xDiff <= 1000) {
				xDivLinear = i % 100;
				if (xDivLinear == 0) {
					writeAxisLabels(g, xAxisMarkers, xLabelAxisNumText, xMarginLabelBounds, yMarginDrawingWindowLowerBorder, scaleTextOffsetX, axis);
				}
			}
			if (xDiff > 1000 && xDiff <= 4000) {
				xDivLinear = i % 500;
				if (xDivLinear == 0) {
					writeAxisLabels(g, xAxisMarkers, xLabelAxisNumText, xMarginLabelBounds, yMarginDrawingWindowLowerBorder, scaleTextOffsetX, axis);
				}
			}
			else if (xDiff > 4000 && xDiff <= 9000) {
				xDivLinear = i % 1000;
				if (xDivLinear == 0) {
					writeAxisLabels(g, xAxisMarkers, xLabelAxisNumText, xMarginLabelBounds, yMarginDrawingWindowLowerBorder, scaleTextOffsetX, axis);
				}
				else if (xDiff > 9000 && xDiff <= 16000) {
					xDivLinear = i % 2000;
					if (xDivLinear == 0) {
						writeAxisLabels(g, xAxisMarkers, xLabelAxisNumText, xMarginLabelBounds, yMarginDrawingWindowLowerBorder, scaleTextOffsetX, axis);
					}
				}
				else if (xDiff > 16000) {
					xDivLinear = i % 5000;
					if (xDivLinear == 0) {
						writeAxisLabels(g, xAxisMarkers, xLabelAxisNumText, xMarginLabelBounds, yMarginDrawingWindowLowerBorder, scaleTextOffsetX, axis);
					}
				}
			}
		}
		g.setColour(juce::Colours::white);
		g.strokePath(xAxisMarkers, juce::PathStrokeType(2.0f));
	}

	// Plot Y Axis Markers
	axis = 'y';
	for (int i = yMinVal; i < 0; i++) {
		float yMarginLabelBounds = yMarginZeroTick + (scaleY * i) + yShift;
		auto yAxisLabelNumText = juce::String(i) + labelTextY;
		if (yDiff <= 10) {
			int yDiv = i % 1;
			if (yDiv == 0) {
				writeAxisLabels(g, yAxisMarkersUp, yAxisLabelNumText, xMarginXYAxis, yMarginLabelBounds, scaleTextOffsetY, axis);
			}
		}
		else if (yDiff > 10 && yDiff <= 29) {
			int yDiv = i % 2;
			if (yDiv == 0) {
				writeAxisLabels(g, yAxisMarkersUp, yAxisLabelNumText, xMarginXYAxis, yMarginLabelBounds, scaleTextOffsetY, axis);
			}
		}
		else if (yDiff > 29 && yDiff <= 49) {
			int yDiv = i % 6;
			if (yDiv == 0) {
				writeAxisLabels(g, yAxisMarkersUp, yAxisLabelNumText, xMarginXYAxis, yMarginLabelBounds, scaleTextOffsetY, axis);
			}
		}
		else if (yDiff > 49 && yDiff <= 69) {
			int yDiv = i % 8;
			if (yDiv == 0) {
				writeAxisLabels(g, yAxisMarkersUp, yAxisLabelNumText, xMarginXYAxis, yMarginLabelBounds, scaleTextOffsetY, axis);
			}
		}
		else if (yDiff > 69 && yDiff <= 89) {
			int yDiv = i % 10;
			if (yDiv == 0) {
				writeAxisLabels(g, yAxisMarkersUp, yAxisLabelNumText, xMarginXYAxis, yMarginLabelBounds, scaleTextOffsetY, axis);
			}
		}
		else {
			int yDiv = i % 6;
			if (yDiv == 0) {
				writeAxisLabels(g, yAxisMarkersUp, yAxisLabelNumText, xMarginXYAxis, yMarginLabelBounds, scaleTextOffsetY, axis);
			}
		}
	}
	g.setColour(juce::Colours::white);
	g.strokePath(yAxisMarkersUp, juce::PathStrokeType(2.0f));
	g.strokePath(yAxisMarkersDown, juce::PathStrokeType(2.0f));

	/*
	//Plot zero on Y-axis
	zeroTick.startNewSubPath(xMarginXYAxis, yMarginStartPlot + yShift);
	zeroTick.lineTo(xMarginXYAxis + zeroTickWidth, yMarginStartPlot + yShift);
	g.strokePath(zeroTick, juce::PathStrokeType(3.0f));
	*/

	// Peak 
	float cursorYPeak = findPeak(50);
	if (cursorYPeak != 0) {
		g.setColour(juce::Colours::red);
		juce::Rectangle<int> peakLine(calculateX(setToLog, cursorPeak), yMarginDrawingWindowUpperBorder, thicknessLine, lengthYAxis - paddingMedium); //FIXME : peak extends too tall
		g.fillRect(peakLine);
		g.setColour(juce::Colours::white);
		if (setToLog) {
			if (isVisiblePlot1 || isVisiblePlot2)
				labelPeakValue.setText("(" + floatToStringPrecision(std::pow(logPower, screenToGraph(calculateX(setToLog, cursorPeak))), precisionValue2) + xAxisValueText + floatToStringPrecision(cursorYPeak, precisionValue2) + yAxisValueText + ")", juce::dontSendNotification);
		}
		else {
			if (isVisiblePlot1 || isVisiblePlot2)
				labelPeakValue.setText("(" + floatToStringPrecision(screenToGraph(calculateX(setToLog, cursorPeak)), precisionValue2) + xAxisValueText + floatToStringPrecision(cursorYPeak, precisionValue2) + yAxisValueText + ")", juce::dontSendNotification);
		}
	}
	

	//** draw graph border **//
	juce::Path graphBoundary;
	graphBoundary.startNewSubPath(xMarginXYAxis, paddingSmall);
	graphBoundary.lineTo(xMarginRightBox, paddingSmall);
	graphBoundary.lineTo(xMarginRightBox, yMarginDrawingWindowLowerBorder);
	graphBoundary.lineTo(xMarginXYAxis, yMarginDrawingWindowLowerBorder);
	graphBoundary.lineTo(xMarginXYAxis, paddingSmall);
	g.setColour(juce::Colours::slategrey);
	g.strokePath(graphBoundary, juce::PathStrokeType(1.0f));

	//** draw boxes to hide out of bound y labels
	//juce::Rectangle<int> topPanel2(originPixel, originPixel, getWidth(), paddingMediumSmall);
	juce::Rectangle<int> bottomPanel2(xMarginOrigin, yMarginDrawingWindowLowerBorder + paddingSmall + heightSmallWidget, widthRightTopBottomBox, getHeight());
	g.setColour(juce::Colours::black);
	//g.fillRect(topPanel2);
	g.fillRect(bottomPanel2);

	//** line to separate left-side components and right-side components **/
	juce::Rectangle<int> LeftRightMenuSeparator(xMarginRightMenu, xMarginOrigin, thicknessLine, heightWindowMax);
	g.setColour(juce::Colours::darkgrey);
	g.fillRect(LeftRightMenuSeparator);

	// draw white box around selections
	g.setColour(juce::Colours::white);
	g.fillRoundedRectangle(paddingSmall, yMarginSelectionBox, widthSelectionBox, heightSelectionBox, cornerSizeSelectionBox);

	//for (int i = 0; i < rowSize; i++) {
	//	// fill in checkboxes
	//	if (plotInfo[i].isVisible == true) {
	//		g.setColour(juce::Colours::dodgerblue);
	//		g.fillRoundedRectangle(xMarginFirstLeftMenuWidget, plotInfo[i].checkBoxPos, widthExtraSmallWidget, heightSmallWidget, cornersizeCheckbox);
	//	}
	//}

	// draw line to separate plot selections
	g.setColour(juce::Colours::lightgrey);
	g.fillRect(xMarginSelectionBoundary, yMarginSelectionBoundary, widthSelectionBoundary, thicknessLine);

	//** line to separate upper and lower x/y bounds in ZOOM **//
	g.setColour(juce::Colours::darkgrey);
	g.fillRect(xMarginZoomBoundary, yMarginZoomBoundary, widthZoomBoundary, thicknessLine);
}

void FFTSpectrumAnalyzerAudioProcessorEditor::writeAxisLabels(juce::Graphics& g, juce::Path axisMarkers, juce::String text, float x, float y, int textOffset, char axis)
{
	if (axis == 'x') {
		axisMarkers.startNewSubPath(x, y);
		axisMarkers.lineTo(x, y - tickWidth);
		g.drawText(text, juce::Rectangle<int>(x - textOffset, y + paddingSmall, widthMediumWidget, heightSmallWidget), juce::Justification::centredLeft, true);
	}
	else if (axis == 'y') {
		axisMarkers.startNewSubPath(x, y);
		axisMarkers.lineTo(x + tickWidth, y);
		g.drawText(text, juce::Rectangle<int>(x - (widthMediumSmallWidget + paddingSmall), y - textOffset, widthMediumWidget, heightSmallWidget), juce::Justification::centredLeft, true);
	}
}

int FFTSpectrumAnalyzerAudioProcessorEditor::getAxisLength(char axis) {
	int lengthAxis = 0;
	if (axis == 'x') {
		lengthAxis = getWidth() - (xMarginXYAxis + paddingLarge);
	}
	else if (axis == 'y'){
		lengthAxis = getHeight() - (paddingSmall + heightBottomMenu + paddingLarge);
	}
	return lengthAxis;
}

void FFTSpectrumAnalyzerAudioProcessorEditor::timerCallback()
{
	if (!isRunning && audioProcessor.getProcBlockCalled()) {
		isRunning = true;
	}
	else if (isRunning && !audioProcessor.getProcBlockCalled()) {
		isRunning = false;
		newSelection = true;
		blockProcessed = true;
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

 //make getaxis width a a function 

void FFTSpectrumAnalyzerAudioProcessorEditor::resized()
{
	int lengthYAxis = getAxisLength('y');

	int xMarginButtonSelectPlots = xMarginLabelPlots + widthLargeWidget + paddingMedium;
	int	xMarginLabelBoundsLetterX = xMarginXBoundsColumn + widthInputTextbox + paddingSmall;
	int xMarginLabelBoundsLetterY = xMarginYBoundsColumn + widthInputTextbox + paddingSmall;

	int xMarginBottomMenuGuiElements = xMarginRightMenu + paddingLarge;
	int xMarginLabelPeak = xMarginBottomMenuGuiElements + widthSecondaryLabel + paddingLarge;
	int xMarginDropdownAxis = xMarginBottomMenuGuiElements + paddingMedium + widthExtraLargeWidget + paddingMedium;
	int xMarginDropdownSize = xMarginDropdownAxis + paddingMedium + widthExtraLargeWidget + paddingMedium;

	int yMarginToggleButtonPlot2 = yMarginRowPlot2;
	int yMarginLabelExport = yMarginLabelZoom + heightMediumWidget + heightZoomWindow;
	int yMarginButtonExport = yMarginLabelExport + heightMediumWidget + paddingMedium;
	int yMarginLabelUpperBounds = yMarginLabelZoom + heightMediumWidget + paddingMedium;
	int yMarginLabelLowerBounds = yMarginZoomBoundary + paddingMedium;
	int yMarginLabelUpperBoundsLetter = yMarginLabelUpperBounds + paddingExtraSmall;
	int yMarginInputLowerBoundsLetter = yMarginLabelLowerBounds + paddingExtraSmall;
	
	int yMarginLabelBottomMenuRow1 = lengthYAxis + paddingLarge;
	int yMarginLabelBottomMenuRow2 = yMarginLabelBottomMenuRow1 + paddingMediumLarge;
	int yMarginLabelBottomMenuRow3 = yMarginLabelBottomMenuRow2 + paddingLarge;
	int yMarginLabelBottomMenuRow4 = yMarginLabelBottomMenuRow3 + paddingMediumLarge;
	
	labelPlot1.setBounds(xMarginLabelPlots, yMarginRowPlot1, widthLargeWidget, heightMediumWidget);
	labelPlot2.setBounds(xMarginLabelPlots, yMarginRowPlot2, widthLargeWidget, heightMediumWidget);
	labelUpperBounds.setBounds(xMarginOrigin, yMarginLabelUpperBounds, widthLargeWidget, heightMediumWidget);
	labelLowerBounds.setBounds(xMarginOrigin, yMarginLabelLowerBounds, widthLargeWidget, heightMediumWidget);
	labelUpperBoundsX.setBounds(xMarginLabelBoundsLetterX, yMarginLabelUpperBoundsLetter, widthLargeWidget, heightMediumWidget);
	labelUpperBoundsY.setBounds(xMarginLabelBoundsLetterY, yMarginLabelUpperBoundsLetter, widthLargeWidget, heightMediumWidget);
	labelLowerBoundsX.setBounds(xMarginLabelBoundsLetterX, yMarginInputLowerBoundsLetter, widthLargeWidget, heightMediumWidget);
	labelLowerBoundsY.setBounds(xMarginLabelBoundsLetterY, yMarginInputLowerBoundsLetter, widthLargeWidget, heightMediumWidget);
	labelExport.setBounds(xMarginOrigin, yMarginLabelExport, xMarginRightMenu, heightMediumWidget);
	labelImportAudio.setBounds(xMarginOrigin, yMarginOrigin, xMarginRightMenu, heightMediumWidget);
	labelSelectTrace.setBounds(xMarginOrigin, yMarginLabelSelectTrace, widthSecondaryLabel, heightMediumWidget);
	labelZoom.setBounds(xMarginOrigin, yMarginLabelZoom, xMarginRightMenu, heightMediumWidget);
	labelCursor.setBounds(xMarginBottomMenuGuiElements, yMarginLabelBottomMenuRow1, widthSecondaryLabel, heightMediumWidget);
	labelCursorValue.setBounds(xMarginBottomMenuGuiElements, yMarginLabelBottomMenuRow2, widthSecondaryLabel, heightMediumWidget);
	labelPeak.setBounds(xMarginLabelPeak, yMarginLabelBottomMenuRow1, widthLargeWidget, heightMediumWidget);
	labelPeakValue.setBounds(xMarginLabelPeak, yMarginLabelBottomMenuRow2, widthSecondaryLabel, heightMediumWidget);
	labelDropdownWindow.setBounds(xMarginBottomMenuGuiElements, yMarginLabelBottomMenuRow3, widthSecondaryLabel, heightMediumWidget);
	labelDropdownAxis.setBounds(xMarginDropdownAxis, yMarginLabelBottomMenuRow3, widthSecondaryLabel, heightMediumWidget);
	labelDropdownSize.setBounds(xMarginDropdownSize, yMarginLabelBottomMenuRow3, widthSecondaryLabel, heightMediumWidget);
	comboboxWindowFunction.setBounds(xMarginBottomMenuGuiElements, yMarginLabelBottomMenuRow4, widthExtraLargeWidget, heightLargeWidget);
	comboboxAxisType.setBounds(xMarginDropdownAxis, yMarginLabelBottomMenuRow4, widthExtraLargeWidget, heightLargeWidget);
	comboboxSizeSetting.setBounds(xMarginDropdownSize, yMarginLabelBottomMenuRow4, widthExtraLargeWidget, heightLargeWidget);

	inputUpperBoundsX.setBounds(xMarginXBoundsColumn, yMarginLabelUpperBoundsLetter, widthInputTextbox, heightMediumWidget);
	inputUpperBoundsY.setBounds(xMarginYBoundsColumn, yMarginLabelUpperBoundsLetter, widthInputTextbox, heightMediumWidget);
	inputLowerBoundsX.setBounds(xMarginXBoundsColumn, yMarginInputLowerBoundsLetter, widthInputTextbox, heightMediumWidget);
	inputLowerBoundsY.setBounds(xMarginYBoundsColumn, yMarginInputLowerBoundsLetter, widthInputTextbox, heightMediumWidget);

	buttonExport.setBounds(xMarginFirstLeftMenuWidget, yMarginButtonExport, widthLargeWidget, heightLargeWidget);
	buttonSelectPlot1.setBounds(xMarginButtonSelectPlots, yMarginRowPlot1, widthLargeWidget, heightMediumWidget);
	buttonSelectPlot2.setBounds(xMarginButtonSelectPlots, yMarginRowPlot2, widthLargeWidget, heightMediumWidget);
	toggleButtonPlot1.setBounds(xMarginFirstLeftMenuWidget, yMarginRowPlot1, widthSmallWidget, heightMediumWidget);
	toggleButtonPlot2.setBounds(xMarginFirstLeftMenuWidget, yMarginRowPlot2, widthSmallWidget, heightMediumWidget);
}

void FFTSpectrumAnalyzerAudioProcessorEditor::setFreqData(int fftData) {
	fftSize = fftData;
	numBins = fftSize / 2 + 1;
	numFreqBins = fftSize / 2;
	stepSize = fftSize / 2;
	//indexToFreqMap.resize(numBins);
	binMag.resize(rowSize, std::vector<float>(numBins, std::numeric_limits<float>::epsilon()));
}

void FFTSpectrumAnalyzerAudioProcessorEditor::getBounds()
{
	float BoundsValueMin = indexToFreqMap[0];
	float BoundsValueMax = 24000;
	float yMaxVal = 0;
	float yMinVal = -90;
	juce::String temp = inputLowerBoundsX.getText(false);
	float val = std::atof(temp.toStdString().c_str());
	if (setToLog) {
		if (val >= BoundsValueMin && val <= BoundsValueMax)
		{
			if (val > 0) {
				xMinFrequency = val;
			}
		}
		else { inputLowerBoundsX.setText(std::to_string(xMin), juce::dontSendNotification); }
	}
	else {
		if (val >= BoundsValueMin && val <= BoundsValueMax)
		{
			xMinFrequency = val;
		}
		else { inputLowerBoundsX.setText(std::to_string(xMin), juce::dontSendNotification); }
	}

	temp = inputUpperBoundsX.getText(false);
	val = std::atof(temp.toStdString().c_str());
	if (val >= BoundsValueMin && val <= BoundsValueMax)
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
	audioProcessor.setRowIndex(rowIndex);
	if (plotIndex == 0)
	{
		buttonSelectPlot1.setButtonText(textSelected);
		buttonSelectPlot2.setButtonText(textNotSelected);
		buttonSelectPlot3.setButtonText(textNotSelected);
		buttonSelectPlot4.setButtonText(textNotSelected);
		buttonSelectPlot5.setButtonText(textNotSelected);
		buttonSelectPlot6.setButtonText(textNotSelected);
		buttonSelectPlot7.setButtonText(textNotSelected);
	}
	else if (plotIndex == 1)
	{
		buttonSelectPlot1.setButtonText(textNotSelected);
		buttonSelectPlot2.setButtonText(textSelected);
		buttonSelectPlot3.setButtonText(textNotSelected);
		buttonSelectPlot4.setButtonText(textNotSelected);
		buttonSelectPlot5.setButtonText(textNotSelected);
		buttonSelectPlot6.setButtonText(textNotSelected);
		buttonSelectPlot7.setButtonText(textNotSelected);
	}
	else if (plotIndex == 2)
	{
		buttonSelectPlot1.setButtonText(textNotSelected);
		buttonSelectPlot2.setButtonText(textNotSelected);
		buttonSelectPlot3.setButtonText(textSelected);
		buttonSelectPlot4.setButtonText(textNotSelected);
		buttonSelectPlot5.setButtonText(textNotSelected);
		buttonSelectPlot6.setButtonText(textNotSelected);
		buttonSelectPlot7.setButtonText(textNotSelected);
	}
	else if (plotIndex == 3)
	{
		buttonSelectPlot1.setButtonText(textNotSelected);
		buttonSelectPlot2.setButtonText(textNotSelected);
		buttonSelectPlot3.setButtonText(textNotSelected);
		buttonSelectPlot4.setButtonText(textSelected);
		buttonSelectPlot5.setButtonText(textNotSelected);
		buttonSelectPlot6.setButtonText(textNotSelected);
		buttonSelectPlot7.setButtonText(textNotSelected);
	}
	else if (plotIndex == 4)
	{
		buttonSelectPlot1.setButtonText(textNotSelected);
		buttonSelectPlot2.setButtonText(textNotSelected);
		buttonSelectPlot3.setButtonText(textNotSelected);
		buttonSelectPlot4.setButtonText(textNotSelected);
		buttonSelectPlot5.setButtonText(textSelected);
		buttonSelectPlot6.setButtonText(textNotSelected);
		buttonSelectPlot7.setButtonText(textNotSelected);
	}
	else if (plotIndex == 5)
	{
		buttonSelectPlot1.setButtonText(textNotSelected);
		buttonSelectPlot2.setButtonText(textNotSelected);
		buttonSelectPlot3.setButtonText(textNotSelected);
		buttonSelectPlot4.setButtonText(textNotSelected);
		buttonSelectPlot5.setButtonText(textNotSelected);
		buttonSelectPlot6.setButtonText(textSelected);
		buttonSelectPlot7.setButtonText(textNotSelected);
	}
	else if (plotIndex == 6)
	{
		buttonSelectPlot1.setButtonText(textNotSelected);
		buttonSelectPlot2.setButtonText(textNotSelected);
		buttonSelectPlot3.setButtonText(textNotSelected);
		buttonSelectPlot4.setButtonText(textNotSelected);
		buttonSelectPlot5.setButtonText(textNotSelected);
		buttonSelectPlot6.setButtonText(textNotSelected);
		buttonSelectPlot7.setButtonText(textSelected);
	}
}


void FFTSpectrumAnalyzerAudioProcessorEditor::setPlotVisibility(int plotId)
{
	plotInfo[plotId].isVisible = !plotInfo[plotId].isVisible;
	repaint();
}

void FFTSpectrumAnalyzerAudioProcessorEditor::setWindowFunction() {
	//juce::dsp::WindowingFunction<float>window(1024, juce::dsp::WindowingFunction<float>::hamming);
	//juce::dsp::WindowingFunction<float>::fillWindowingTables(fftSize, juce::dsp::WindowingFunction<float>::hamming);
	juce::dsp::WindowingFunction<float>::WindowingMethod newWindow;

	switch (comboboxWindowFunction.getSelectedId())
	{
	case 1:
		windowData.fillWindowingTables(fftSize, juce::dsp::WindowingFunction<float>::blackman);
		break;
	case 2:
		windowData.fillWindowingTables(fftSize, juce::dsp::WindowingFunction<float>::blackmanHarris);
		break;
	case 3:
		windowData.fillWindowingTables(fftSize, juce::dsp::WindowingFunction<float>::flatTop);
		break;
	case 4:
		windowData.fillWindowingTables(fftSize, juce::dsp::WindowingFunction<float>::hamming);
		break;
	case 5:
		windowData.fillWindowingTables(fftSize, juce::dsp::WindowingFunction<float>::hann);
		break;
	case 6:
		windowData.fillWindowingTables(fftSize, juce::dsp::WindowingFunction<float>::kaiser);
		break;
	case 7:
		windowData.fillWindowingTables(fftSize, juce::dsp::WindowingFunction<float>::rectangular);
		break;
	case 8:
		windowData.fillWindowingTables(fftSize, juce::dsp::WindowingFunction<float>::triangular);
		break;
	}
}

void FFTSpectrumAnalyzerAudioProcessorEditor::setBlockSize() {
	auto selection = comboboxSizeSetting.getText();
	fftSize = selection.getIntValue();
	setFreqData(fftSize);
}

void FFTSpectrumAnalyzerAudioProcessorEditor::setAxisType() {
	if (setToLog == true) {
		setToLog = false;
	}
	else {
		setToLog = true;
	}
	repaint();
}

void FFTSpectrumAnalyzerAudioProcessorEditor::initializeBinMag() {
	binMag.resize(2, std::vector<float>(numBins, std::numeric_limits<float>::epsilon()));
}

void FFTSpectrumAnalyzerAudioProcessorEditor::setWindow(juce::dsp::WindowingFunction<float>::WindowingMethod type) {
	//juce::dsp::WindowingFunction<float> windowData(fftSize, type);
	//windowData.fillWindowingTables(fftSize, type);
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
		windowData.multiplyWithWindowingTable(windowBufferRight.data(), fftSize);
		windowData.multiplyWithWindowingTable(windowBufferLeft.data(), fftSize);
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

	float cursorX = event.getMouseDownX();
	float cursorY = event.getMouseDownY();

	//invalid bounds
	if (!inBounds(cursorX, cursorY)) {
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
			//cursorIndex = i;
			
			for (int i = 0; i < rowSize; i++) {
				if (plotInfo[i].isVisible) {
					if (setToLog) {
						float xCoord = std::pow(logPower, screenToGraph(calculateX(setToLog, i)));
						labelCursorValue.setText("(" + floatToStringPrecision(xCoord, precisionValue1) + xAxisValueText + floatToStringPrecision(getYCoord(plotIndexSelection, setToLog, i), precisionValue2) + yAxisValueText + ")", juce::dontSendNotification);
					}
					else {
						float xCoord = screenToGraph(calculateX(setToLog, i));
						labelCursorValue.setText("(" + floatToStringPrecision(xCoord, precisionValue1) + xAxisValueText + floatToStringPrecision(getYCoord(plotIndexSelection, setToLog, i), precisionValue2) + yAxisValueText + ")", juce::dontSendNotification);
					}
				}
			}
		}
	}
	repaint();
}

float FFTSpectrumAnalyzerAudioProcessorEditor::calculateX(bool log, int index) 
{
	int lengthXAxis = getAxisLength('x');
	float peakRange = xMax - xMin;
	float ratio = lengthXAxis / peakRange;
	float shift = -xMin * ratio;

	if (log) {
		return std::log10(indexToFreqMap[index]) * ratio + xMarginXYAxis + shift;
	}
	else {
		return indexToFreqMap[index] * ratio + xMarginXYAxis + shift;
	}
}

float FFTSpectrumAnalyzerAudioProcessorEditor::calculateY(int plotSelection, int index) 
{
	int lengthYAxis = getAxisLength('y');
	float ratio = -lengthYAxis / peakRange;
	float shift = (peakRange - 2.0f * yMax) * ratio / 2.0f;
	float start = xMarginXYAxis;

	return std::log10(binMag[plotSelection][index]) * logScale * ratio + shift + xMarginXYAxis;
}

int FFTSpectrumAnalyzerAudioProcessorEditor::findPeak(int samples) 
{
	int i = 0;
	float maxValue = -10000;
	float temp;
	while (i < numBins - 1) {
		temp = getYCoord(plotIndexSelection, setToLog, i);
		if (temp > maxValue) {
			maxValue = temp;
			cursorPeak = i;
		}
		i++;
	}
	if (maxValue == -10000)
		return 0;
	return maxValue;
}

bool FFTSpectrumAnalyzerAudioProcessorEditor::inBounds(float x, float y) 
{
	int lengthXAxis = getAxisLength('x');
	int lengthYAxis = getAxisLength('y');

	float graphWest = xMarginXYAxis;
	float graphNorth = paddingSmall;
	float graphEast = xMarginXYAxis + lengthXAxis;
	float graphSouth = paddingSmall + lengthYAxis;

	if (x < graphWest || x > graphEast || y < graphNorth || y > graphSouth) {
		return false;
	}
	return true;
}

float FFTSpectrumAnalyzerAudioProcessorEditor::getYCoord(int plotNumber, bool log, int index) {
	return logScale * std::log10(binMag[plotNumber][index]);
}

float FFTSpectrumAnalyzerAudioProcessorEditor::screenToGraph(float screenCoord) {
	int lengthXAxis = getAxisLength('x');
	float ratio = lengthXAxis / peakRange;
	float shift = -xMin * ratio;
	
	return (screenCoord + shift - xMarginXYAxis) / ratio;}

float FFTSpectrumAnalyzerAudioProcessorEditor::graphToScreen(int graphCoord) 
{
	int lengthXAxis = getAxisLength('x');;
	float ratio = lengthXAxis / peakRange;
	float shift = -xMin * ratio;

	return (graphCoord * ratio) + xMarginXYAxis - shift;
}

std::string FFTSpectrumAnalyzerAudioProcessorEditor::floatToStringPrecision(float f, int p)
{
	std::ostringstream oss;
	oss << std::fixed << std::setprecision(p) << f;
	return oss.str();
}