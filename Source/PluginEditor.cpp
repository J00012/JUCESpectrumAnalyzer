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
bool FFTSpectrumAnalyzerAudioProcessorEditor::setToLog;
bool FFTSpectrumAnalyzerAudioProcessorEditor::conCall = true;
bool FFTSpectrumAnalyzerAudioProcessorEditor::blockProcessed = false;
bool FFTSpectrumAnalyzerAudioProcessorEditor::initialLambda = true;
bool FFTSpectrumAnalyzerAudioProcessorEditor::selectionSizeError = false;
bool FFTSpectrumAnalyzerAudioProcessorEditor::darkMode = true;
bool FFTSpectrumAnalyzerAudioProcessorEditor::gridOff = true;
bool FFTSpectrumAnalyzerAudioProcessorEditor::procBuff = false;
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
int FFTSpectrumAnalyzerAudioProcessorEditor::cursorIndex;
int FFTSpectrumAnalyzerAudioProcessorEditor::windowWidth = 950;
int FFTSpectrumAnalyzerAudioProcessorEditor::windowHeight = 550 + 2;
int FFTSpectrumAnalyzerAudioProcessorEditor::windowMaxWidth = 2160;
int FFTSpectrumAnalyzerAudioProcessorEditor::windowMaxHeight = 1080;

//ROW INDEX STUFF!!!
int FFTSpectrumAnalyzerAudioProcessorEditor::rowSize = 4;
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
int FFTSpectrumAnalyzerAudioProcessorEditor::initialAxisState;
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
std::vector<std::string> FFTSpectrumAnalyzerAudioProcessorEditor::windowFunctionTypes = { "Blackman window","Blackman-Harris window", "Flatop window", "Hamming window", "Hann window", "Kaiser", "Rectangular window", "Triangular window" };
std::vector<std::string> FFTSpectrumAnalyzerAudioProcessorEditor::axisTypes = { "Linear Frequency", "Logarithmic Frequency"};
std::vector<std::string> FFTSpectrumAnalyzerAudioProcessorEditor::sizeOptions = { "128", "256", "512", "1024", "2048", "4096", "8192", "16384"};		  
																				  
FFTSpectrumAnalyzerAudioProcessorEditor::plotItem FFTSpectrumAnalyzerAudioProcessorEditor::plotInfo[4] = {
	{true, juce::Colours::lightgreen, juce::Path()},
	{true, juce::Colours::cornflowerblue,juce::Path()},
	{true, juce::Colours::purple,juce::Path()},
	{true, juce::Colours::darkorange, juce::Path()}
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

	sampleSelections.resize(4);
	setPlotIndex(rowIndex);
	setFreqData(fftSize);
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
	addAndMakeVisible(labelPlot3);
	addAndMakeVisible(labelPlot4);
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
	addAndMakeVisible(labelAppearanceMode); 
	addAndMakeVisible(labelGrid); 
	addAndMakeVisible(toggleButtonPlot1);
	addAndMakeVisible(toggleButtonPlot2);
	addAndMakeVisible(toggleButtonPlot3);
	addAndMakeVisible(toggleButtonPlot4);
	addAndMakeVisible(togglePluginAppearance);
	addAndMakeVisible(toggleGrid);

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
	labelAppearanceMode.setFont(juce::Font("Arial", 12.0f, juce::Font::plain));
	labelGrid.setFont(juce::Font("Arial", 12.0f, juce::Font::plain));

	// Text
	labelImportAudio.setText("Import Audio", juce::dontSendNotification);
	labelSelectTrace.setText("Selected Traces", juce::dontSendNotification);
	labelPlot1.setText("Trace 1", juce::dontSendNotification);
	labelPlot2.setText("Trace 2", juce::dontSendNotification);
	labelPlot3.setText("Trace 3", juce::dontSendNotification);
	labelPlot4.setText("Trace 4", juce::dontSendNotification);
	labelAppearanceMode.setText("Enable light mode", juce::dontSendNotification); 
	labelGrid.setText("Enable grid", juce::dontSendNotification); 
	labelZoom.setText("Zoom", juce::dontSendNotification);
	labelUpperBounds.setText("Upper", juce::dontSendNotification);
	labelLowerBounds.setText("Lower", juce::dontSendNotification);
	labelUpperBoundsX.setText("X", juce::dontSendNotification);
	labelUpperBoundsY.setText("Y", juce::dontSendNotification);
	labelLowerBoundsX.setText("X", juce::dontSendNotification);
	labelLowerBoundsY.setText("Y", juce::dontSendNotification);
	labelExport.setText("Export", juce::dontSendNotification);
	labelCursor.setText("Cursor", juce::dontSendNotification);
	labelCursorValue.setText("(0.0 Hz, 0.00 dB)", juce::dontSendNotification);
	labelPeak.setText("Peak", juce::dontSendNotification);
	labelDropdownWindow.setText("Function", juce::dontSendNotification);
	labelDropdownSize.setText("Size", juce::dontSendNotification);
	labelDropdownAxis.setText("Axis", juce::dontSendNotification);
	inputLowerBoundsX.setText(std::to_string((int)xMinFrequency), juce::dontSendNotification);
	inputUpperBoundsX.setText(std::to_string((int)xMaxFrequency), juce::dontSendNotification);
	inputLowerBoundsY.setText(std::to_string((int)yMin), juce::dontSendNotification);
	inputUpperBoundsY.setText(std::to_string((int)yMax), juce::dontSendNotification);

	// Colour
	labelImportAudio.setColour(juce::Label::backgroundColourId, juce::Colours::darkgrey);
	labelZoom.setColour(juce::Label::backgroundColourId, juce::Colours::darkgrey);
	labelExport.setColour(juce::Label::backgroundColourId, juce::Colours::darkgrey);
	labelSelectTrace.setColour(juce::Label::textColourId, juce::Colours::white);
	labelCursor.setColour(juce::Label::textColourId, juce::Colours::white);
	labelPeak.setColour(juce::Label::textColourId, juce::Colours::white);
	labelCursorValue.setColour(juce::Label::textColourId, juce::Colours::white);
	labelPeakValue.setColour(juce::Label::textColourId, juce::Colours::white);
	labelAppearanceMode.setColour(juce::Label::textColourId, juce::Colours::white); 
	labelGrid.setColour(juce::Label::textColourId, juce::Colours::white);
	labelDropdownWindow.setColour(juce::Label::textColourId, juce::Colours::white);
	labelDropdownSize.setColour(juce::Label::textColourId, juce::Colours::white);
	labelDropdownAxis.setColour(juce::Label::textColourId, juce::Colours::white);
	labelPlot1.setColour(juce::Label::textColourId, juce::Colours::darkgrey);
	labelPlot2.setColour(juce::Label::textColourId, juce::Colours::darkgrey);
	labelPlot3.setColour(juce::Label::textColourId, juce::Colours::darkgrey);
	labelPlot4.setColour(juce::Label::textColourId, juce::Colours::darkgrey);
	labelDropdownWindow.setColour(juce::Label::textColourId, juce::Colours::white);
	labelDropdownAxis.setColour(juce::Label::textColourId, juce::Colours::white);
	labelDropdownSize.setColour(juce::Label::textColourId, juce::Colours::white);
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
	togglePluginAppearance.setColour(juce::ToggleButton::tickColourId, juce::Colours::darkgrey); 
	togglePluginAppearance.setColour(juce::ToggleButton::tickDisabledColourId, juce::Colours::lightgrey); 
	toggleGrid.setColour(juce::ToggleButton::tickColourId, juce::Colours::darkgrey);
	toggleGrid.setColour(juce::ToggleButton::tickDisabledColourId, juce::Colours::lightgrey); 
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
	labelPlot3.setEditable(false);
	labelPlot4.setEditable(false);

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
	comboboxAxisType.setSelectedId(1);
	comboboxSizeSetting.setSelectedId(4);
	buttonSelectPlot1.setClickingTogglesState(true);
	buttonSelectPlot2.setClickingTogglesState(true);
	buttonSelectPlot3.setClickingTogglesState(true);
	buttonSelectPlot4.setClickingTogglesState(true);
	toggleButtonPlot1.setClickingTogglesState(true);
	toggleButtonPlot2.setClickingTogglesState(true);
	toggleButtonPlot3.setClickingTogglesState(true);
	toggleButtonPlot4.setClickingTogglesState(true); 
	togglePluginAppearance.setClickingTogglesState(true);
	toggleGrid.setClickingTogglesState(true); 

	// On user interaction
	inputLowerBoundsX.onTextChange = [this] { getBounds(); };
	inputUpperBoundsX.onTextChange = [this] { getBounds(); };
	inputLowerBoundsY.onTextChange = [this] { getBounds(); };
	inputUpperBoundsY.onTextChange = [this] { getBounds(); };
	comboboxWindowFunction.onChange = [this] {
		setWindowFunction();
		zeroBuffers();
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

	comboboxSizeSetting.onChange = [this] { 
		setBlockSize();
		setWindowFunction();
		zeroBuffers();
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
		cursorIndex = 1;
		repaint();
	};

	setToLog = true;
	initialAxisState = 1;

	if (darkMode == true) {
		togglePluginAppearance.setToggleState(false , true); 
	}
	else {
		togglePluginAppearance.setToggleState(true, true); 
	} 

	if (gridOff == true) {
		toggleGrid.setToggleState(false, true); 
	}
	else {
		toggleGrid.setToggleState(true, true); 
	}

	initialLambda = true;
	toggleButtonPlot1.onClick = [this] {  if (!initialLambda) {
		plotInfo[0].isVisible = toggleButtonPlot1.getToggleState(); repaint();} };
	toggleButtonPlot2.onClick = [this] {  if (!initialLambda) {  
		plotInfo[1].isVisible = toggleButtonPlot2.getToggleState(); repaint();} };
	toggleButtonPlot3.onClick = [this] {  if (!initialLambda) {  
		plotInfo[2].isVisible = toggleButtonPlot3.getToggleState(); repaint();} };
	toggleButtonPlot4.onClick = [this] {  if (!initialLambda) {  
		plotInfo[3].isVisible = toggleButtonPlot4.getToggleState(); repaint();}
	else { initialLambda = false; }};

	buttonSelectPlot1.onClick = [&]() {
		rowIndex = 0;
		setPlotIndex(0);
		};
	buttonSelectPlot2.onClick = [&]() {
		rowIndex = 1;
		setPlotIndex(1);
		};
	buttonSelectPlot3.onClick = [&]() {
		rowIndex = 2;
		setPlotIndex(2);
		};
	buttonSelectPlot4.onClick = [&]() {
		rowIndex = 3;
		setPlotIndex(3);
		};

	togglePluginAppearance.onClick = [&]() {
		if (darkMode == true) {
			darkMode = false;
		}
		else {
			darkMode = true;
		}
		repaint();
		};

	toggleGrid.onClick = [&]() {
		if (gridOff == true) {
			gridOff = false;
		}
		else {
			gridOff = true;
		}
		repaint();
		};

	buttonExport.onClick = [&]() {
		exportDataToCSV();
		};

	toggleButtonPlot1.setToggleState(plotInfo[0].isVisible, true);
	toggleButtonPlot2.setToggleState(plotInfo[1].isVisible, true);
	toggleButtonPlot3.setToggleState(plotInfo[2].isVisible, true);
	toggleButtonPlot4.setToggleState(plotInfo[3].isVisible, true);
}

FFTSpectrumAnalyzerAudioProcessorEditor::~FFTSpectrumAnalyzerAudioProcessorEditor()
{
}

//==============================================================================
void FFTSpectrumAnalyzerAudioProcessorEditor::paint(juce::Graphics& g)
{
	if (darkMode == true) {
		g.fillAll(juce::Colours::black);
	}
	else {
		g.fillAll(juce::Colours::white);
	}
	g.setOpacity(1.0f);
	g.setColour(juce::Colours::white);

	// label color when set to light mode
	setAppearance();

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
		if (initialAxisState == 1) {
			setToLog = false;
		}
		audioProcessor.setStepSize(stepSize);                             //this needs to be changed when the size is changed
		sampleSelections[rowIndex] = audioProcessor.getAccumulationBuffer();
		audioProcessor.clearAccumulationBuffer();
		processBuffer();
		newSelection = false;
	}
	initialAxisState = 0;

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
			inputLowerBoundsX.setText(std::to_string((int)xMinPrev), juce::dontSendNotification);
			inputUpperBoundsX.setText(std::to_string((int)xMaxPrev), juce::dontSendNotification);
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
		inputLowerBoundsY.setText(std::to_string((int)yMinPrev), juce::dontSendNotification);
		inputUpperBoundsY.setText(std::to_string((int)yMaxPrev), juce::dontSendNotification);
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
	if (selectionSizeValid()) {
		for (int i = 0; i < rowSize; i++) {

			if (plotInfo[i].isVisible) {
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
	if (darkMode) {
		g.setColour(juce::Colours::black);
	}
	else {
		g.setColour(juce::Colours::white);
	}
	g.fillRect(leftPanel);
	g.fillRect(rightPanel);
	g.fillRect(topPanel);
	g.fillRect(bottomPanel);

	// Plot X Axis Markers
	axis = 'x';
	if (darkMode) {
		g.setColour(juce::Colours::whitesmoke);
	}
	else {
		g.setColour(juce::Colours::black);
	}
	g.setFont(axisFontSize);

	for (int i = 1; i <= xDiff; ++i) {
		float xMarginLabelBounds = xMarginXYAxis + (i * scaleX);
		if (setToLog) {
			int xLabelAxisNum = std::pow(logPower, i);
			auto xLabelAxisNumText = juce::String(xLabelAxisNum) + labelTextX;
			writeAxisLabels(g, xAxisMarkers, xLabelAxisNumText, xMarginLabelBounds, yMarginDrawingWindowLowerBorder, scaleTextOffsetX, axis);
		}
		int xDivLinear = i;

		if (!setToLog) {
			if (xDiff <= 100)			xDivLinear %= 10;
			else if (xDiff <= 1000)		xDivLinear %= 100;
			else if (xDiff <= 4000)		xDivLinear %= 500;
			else if (xDiff <= 9000)		xDivLinear %= 1000;
			else if (xDiff <= 16000)	xDivLinear %= 2000;
			else						xDivLinear %= 5000;
		}

		if (xDivLinear == 0) {
			int xLabelAxisNum = setToLog ? std::pow(logPower, i) : i;
			auto xLabelAxisNumText = juce::String(xLabelAxisNum) + labelTextX;
			writeAxisLabels(g, xAxisMarkers, xLabelAxisNumText, xMarginLabelBounds, yMarginDrawingWindowLowerBorder, scaleTextOffsetX, axis);
		}
	}

	if (darkMode) {
		g.setColour(juce::Colours::whitesmoke);
	}
	else {
		g.setColour(juce::Colours::black);
	}
	g.strokePath(xAxisMarkers, juce::PathStrokeType(2.0f));

	// Plot Y Axis Markers
	axis = 'y';
	for (int i = yMin; i < 0; ++i) {
		float yMarginLabelBounds = yMarginZeroTick + (scaleY * i) + yShift;
		int divisor = 0;

		if (yDiff <= 10)      divisor = 1;
		else if (yDiff <= 29) divisor = 2;
		else if (yDiff <= 49) divisor = 6;
		else if (yDiff <= 69) divisor = 8;
		else if (yDiff <= 89) divisor = 10;
		else if (yDiff == 90) divisor = 6;
		else if (yDiff <= 150) divisor = 10;
		else                  divisor = 15;

		if (i % divisor == 0) {
			auto yAxisLabelNumText = juce::String(i) + labelTextY;
			writeAxisLabels(g, yAxisMarkersUp, yAxisLabelNumText, xMarginXYAxis, yMarginLabelBounds, scaleTextOffsetY, axis);
		}
	} 

	if (darkMode) {
		g.setColour(juce::Colours::whitesmoke);
	}
	else {
		g.setColour(juce::Colours::black);
	}
	g.strokePath(yAxisMarkersUp, juce::PathStrokeType(2.0f));
	g.strokePath(yAxisMarkersDown, juce::PathStrokeType(2.0f));

	// Peak 
	if (plotInfo[rowIndex].isVisible) {
		//no trace selected
		if (!audioProcessor.minBlockSize) {
			labelPeakValue.setText("(0.0 Hz, 0.00 dB)", juce::dontSendNotification);
		}
		else {
			int cursorPeakIndex = findPeak((int)(stepSize * .1));
			g.setColour(juce::Colours::red);
			juce::Rectangle<int> peakLine(calculateX(cursorPeakIndex), paddingSmall, thicknessLine, getAxisLength('y'));
			g.fillRect(peakLine);

			if (cursorPeakIndex >= binMag[rowIndex].size()) {
				labelPeakValue.setText("(0.0 Hz, 0.00 dB)", juce::dontSendNotification);
			}
			else {
				//calculate x and y coordinate of peak
				float peakX = setToLog ? std::pow(logPower, screenToGraph(calculateX(cursorPeakIndex))) : screenToGraph(calculateX(cursorPeakIndex));
				labelPeakValue.setText("(" + floatToStringPrecision(peakX, precisionValue1) + xAxisValueText + floatToStringPrecision(getYCoord(cursorPeakIndex), precisionValue2) + yAxisValueText + ")", juce::dontSendNotification);

				//Graph Plot Marker
				if (darkMode) {
					g.setColour(juce::Colours::white);
				}
				else {
					g.setColour(juce::Colours::red);
				}
				float circleX = calculateX(cursorIndex);
				float circleY = calculateY(cursorIndex);
				if (inBounds(circleX, circleY)) {
					g.drawEllipse(circleX, circleY, thicknessLine, thicknessLine, heightExtraSmallWidget);
				}
			}
		}
	}
	
	//** draw graph border **//
	juce::Path graphBoundary;
	graphBoundary.startNewSubPath(xMarginXYAxis, paddingSmall);
	graphBoundary.lineTo(xMarginRightBox, paddingSmall);
	graphBoundary.lineTo(xMarginRightBox, yMarginDrawingWindowLowerBorder);
	graphBoundary.lineTo(xMarginXYAxis, yMarginDrawingWindowLowerBorder);
	graphBoundary.lineTo(xMarginXYAxis, paddingSmall);
	g.setColour(juce::Colours::darkgrey);
	g.strokePath(graphBoundary, juce::PathStrokeType(1.0f));

	//** draw boxes to hide out of bound y labels
	juce::Rectangle<int> bottomPanel2(xMarginOrigin, yMarginDrawingWindowLowerBorder + paddingSmall, xMarginXYAxis, getHeight());
	if (darkMode) {
		g.setColour(juce::Colours::black);
	}
	else {
		g.setColour(juce::Colours::white);
	}
	g.fillRect(bottomPanel2);

	//** line to separate left-side components and right-side components **/
	juce::Rectangle<int> LeftRightMenuSeparator(xMarginRightMenu, xMarginOrigin, thicknessLine, heightWindowMax);
	if (darkMode) {
		g.setColour(juce::Colours::darkgrey);
	}
	else {
		g.setColour(juce::Colours::grey);
	}
	g.fillRect(LeftRightMenuSeparator);

	// draw white box around selections
	g.setColour(juce::Colours::white);
	g.fillRoundedRectangle(paddingSmall, yMarginSelectionBox, widthSelectionBox, heightSelectionBox, cornerSizeSelectionBox);

	if (!darkMode) {
		g.setColour(juce::Colours::darkgrey);
		g.drawRoundedRectangle(paddingSmall, yMarginSelectionBox, widthSelectionBox, heightSelectionBox, cornerSizeSelectionBox, borderSizeSelectionBox);
	}

	// draw lines to separate plot selections
	g.setColour(juce::Colours::lightgrey);
	g.fillRect(xMarginSelectionBoundary, yMarginSelectionBoundary1, widthSelectionBoundary, thicknessLine);
	g.fillRect(xMarginSelectionBoundary, yMarginSelectionBoundary2, widthSelectionBoundary, thicknessLine);
	g.fillRect(xMarginSelectionBoundary, yMarginSelectionBoundary3, widthSelectionBoundary, thicknessLine);

	//draw circles that have color of plot
	g.setColour(juce::Colours::lightgrey);
	g.drawEllipse(xMarginPlotColorID, yMarginRowPlotColorID1, widthExtraSmallWidget, heightSmallWidget, thicknessLine);
	g.drawEllipse(xMarginPlotColorID, yMarginRowPlotColorID2, widthExtraSmallWidget, heightSmallWidget, thicknessLine);
	g.drawEllipse(xMarginPlotColorID, yMarginRowPlotColorID3, widthExtraSmallWidget, heightSmallWidget, thicknessLine);
	g.drawEllipse(xMarginPlotColorID, yMarginRowPlotColorID4, widthExtraSmallWidget, heightSmallWidget, thicknessLine);
	g.setColour(juce::Colours::lightgreen);
	g.fillEllipse(xMarginPlotColorID, yMarginRowPlotColorID1, widthExtraSmallWidget, heightSmallWidget);
	g.setColour(juce::Colours::cornflowerblue);
	g.fillEllipse(xMarginPlotColorID, yMarginRowPlotColorID2, widthExtraSmallWidget, heightSmallWidget);
	g.setColour(juce::Colours::purple);
	g.fillEllipse(xMarginPlotColorID, yMarginRowPlotColorID3, widthExtraSmallWidget, heightSmallWidget);
	g.setColour(juce::Colours::darkorange);
	g.fillEllipse(xMarginPlotColorID, yMarginRowPlotColorID4, widthExtraSmallWidget, heightSmallWidget);

	//** line to separate upper and lower x/y bounds in ZOOM **//
	g.setColour(juce::Colours::darkgrey);
	g.fillRect(xMarginZoomBoundary, yMarginZoomBoundary, widthZoomBoundary, thicknessLine);
}

void FFTSpectrumAnalyzerAudioProcessorEditor::writeAxisLabels(juce::Graphics& g, juce::Path axisMarkers, juce::String text, float x, float y, int textOffset, char axis)
{
	int lengthXAxis = getAxisLength('x');
	int lengthYAxis = getAxisLength('y');
	if (axis == 'x') {
		axisMarkers.startNewSubPath(x, y);
		axisMarkers.lineTo(x, y - lengthYAxis);
		if (!gridOff) {
			g.strokePath(axisMarkers, juce::PathStrokeType(1.0f));
		}
		g.drawText(text, juce::Rectangle<int>(x - textOffset, y + paddingSmall, widthMediumWidget, heightSmallWidget), juce::Justification::centredLeft, true);
	}
	else if (axis == 'y') {
		axisMarkers.startNewSubPath(x, y);
		axisMarkers.lineTo(x + lengthXAxis, y);
		if (!gridOff) {
			g.strokePath(axisMarkers, juce::PathStrokeType(1.0f));
		}
		g.drawText(text, juce::Rectangle<int>(x - (widthMediumSmallWidget + paddingSmall), y - textOffset, widthMediumWidget, heightSmallWidget), juce::Justification::centredLeft, true);
	}
}

int FFTSpectrumAnalyzerAudioProcessorEditor::getAxisLength(char axis) 
{
	if (axis == 'x') {
		return getWidth() - (xMarginXYAxis + paddingLarge);
	}
	else if (axis == 'y'){
		return getHeight() - (paddingSmall + heightBottomMenu + paddingLarge);
	}
	return 0;
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

	int xMarginToggleAppearance = paddingMedium;
	int xMarginLabelApperance = xMarginToggleAppearance + widthSmallWidget;

	int xMarginToggleGrid = xMarginLabelApperance + widthLargeWidget + paddingMedium;
	int xMarginLabelGrid = xMarginToggleGrid + widthSmallWidget;

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

	int yMarginToggleAppearance = getHeight() - (heightMediumWidget + paddingSmall);
	int yMarginLabelAppearance = yMarginToggleAppearance - paddingMediumSmall;

	int yMarginToggleGrid = yMarginToggleAppearance; 
	int yMarginLabelGrid = yMarginToggleGrid - paddingMediumSmall;
	
	labelPlot1.setBounds(xMarginLabelPlots, yMarginRowPlot1, widthLargeWidget, heightMediumWidget);
	labelPlot2.setBounds(xMarginLabelPlots, yMarginRowPlot2, widthLargeWidget, heightMediumWidget);
	labelPlot3.setBounds(xMarginLabelPlots, yMarginRowPlot3, widthLargeWidget, heightMediumWidget);
	labelPlot4.setBounds(xMarginLabelPlots, yMarginRowPlot4, widthLargeWidget, heightMediumWidget);
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
	labelAppearanceMode.setBounds(xMarginLabelApperance, yMarginLabelAppearance, widthLargeWidget, heightMediumWidget);
	labelGrid.setBounds(xMarginLabelGrid, yMarginLabelGrid, widthLargeWidget, heightMediumWidget);
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
	buttonSelectPlot3.setBounds(xMarginButtonSelectPlots, yMarginRowPlot3, widthLargeWidget, heightMediumWidget);
	buttonSelectPlot4.setBounds(xMarginButtonSelectPlots, yMarginRowPlot4, widthLargeWidget, heightMediumWidget);
	toggleButtonPlot1.setBounds(xMarginFirstLeftMenuWidget, yMarginRowPlot1, widthSmallWidget, heightMediumWidget);
	toggleButtonPlot2.setBounds(xMarginFirstLeftMenuWidget, yMarginRowPlot2, widthSmallWidget, heightMediumWidget);
	toggleButtonPlot3.setBounds(xMarginFirstLeftMenuWidget, yMarginRowPlot3, widthSmallWidget, heightMediumWidget);
	toggleButtonPlot4.setBounds(xMarginFirstLeftMenuWidget, yMarginRowPlot4, widthSmallWidget, heightMediumWidget);
	togglePluginAppearance.setBounds(xMarginToggleAppearance, yMarginToggleAppearance, widthSmallWidget, heightSmallWidget);
	toggleGrid.setBounds(xMarginToggleGrid, yMarginToggleGrid, widthSmallWidget, heightSmallWidget);
}

void FFTSpectrumAnalyzerAudioProcessorEditor::setFreqData(int fftData) {
	fftSize = fftData;
	numBins = fftSize / 2 + 1;
	numFreqBins = fftSize / 2;
	stepSize = fftSize / 2;
	binMag.resize(rowSize, std::vector<float>(numBins, std::numeric_limits<float>::epsilon()));
}

void FFTSpectrumAnalyzerAudioProcessorEditor::getBounds()
{
	float BoundsValueMin = indexToFreqMap[0];
	float BoundsValueMax = 24000;
	float yMaxVal = 0;
	float yMinVal = -200;
	juce::String temp = inputLowerBoundsX.getText(false);
	float val = std::atof(temp.toStdString().c_str());
	if (setToLog) {
		if (val >= BoundsValueMin && val <= BoundsValueMax)
		{
			if (val > 0) {
				xMinFrequency = val;
			}
		}
		else { inputLowerBoundsX.setText(std::to_string((int)xMin), juce::dontSendNotification); }
	}
	else {
		if (val >= BoundsValueMin && val <= BoundsValueMax)
		{
			xMinFrequency = val;
		}
		else { inputLowerBoundsX.setText(std::to_string((int)xMin), juce::dontSendNotification); }
	}

	temp = inputUpperBoundsX.getText(false);
	val = std::atof(temp.toStdString().c_str());
	if (val >= BoundsValueMin && val <= BoundsValueMax)
	{
		if (val != 0) {
			xMaxFrequency = val;
		}
	}
	else { inputUpperBoundsX.setText(std::to_string((int)xMax), juce::dontSendNotification); }

	temp = inputLowerBoundsY.getText(false);
	val = std::atof(temp.toStdString().c_str());
	if (val >= yMinVal && val <= yMaxVal)
	{
		yMin = val;
	}
	else { inputLowerBoundsY.setText(std::to_string((int)yMin), juce::dontSendNotification); }

	temp = inputUpperBoundsY.getText(false);
	val = std::atof(temp.toStdString().c_str());
	if (val >= yMinVal && val <= yMaxVal)
	{
		yMax = val;
	}
	else { inputUpperBoundsY.setText(std::to_string((int)yMax), juce::dontSendNotification); }
	repaint();
}

void FFTSpectrumAnalyzerAudioProcessorEditor::setPlotIndex(int plotIndex)
{
	//audioProcessor.setRowIndex(rowIndex);
	if (plotIndex == 0)
	{
		buttonSelectPlot1.setButtonText(textSelected);
		buttonSelectPlot2.setButtonText(textNotSelected);
		buttonSelectPlot3.setButtonText(textNotSelected);
		buttonSelectPlot4.setButtonText(textNotSelected);
	}
	else if (plotIndex == 1)
	{
		buttonSelectPlot1.setButtonText(textNotSelected);
		buttonSelectPlot2.setButtonText(textSelected);
		buttonSelectPlot3.setButtonText(textNotSelected);
		buttonSelectPlot4.setButtonText(textNotSelected);
	}
	else if (plotIndex == 2)
	{
		buttonSelectPlot1.setButtonText(textNotSelected);
		buttonSelectPlot2.setButtonText(textNotSelected);
		buttonSelectPlot3.setButtonText(textSelected);
		buttonSelectPlot4.setButtonText(textNotSelected);
	}
	else if (plotIndex == 3)
	{
		buttonSelectPlot1.setButtonText(textNotSelected);
		buttonSelectPlot2.setButtonText(textNotSelected);
		buttonSelectPlot3.setButtonText(textNotSelected);
		buttonSelectPlot4.setButtonText(textSelected);
	}
}

void FFTSpectrumAnalyzerAudioProcessorEditor::setWindowFunction() {
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
	if (setToLog) {
		setToLog = false;
	}
	else {
		setToLog = true;
	}
	repaint();
}

void FFTSpectrumAnalyzerAudioProcessorEditor::setAppearance() {
	if (darkMode) {
		labelImportAudio.setColour(juce::Label::backgroundColourId, juce::Colours::darkgrey);
		labelZoom.setColour(juce::Label::backgroundColourId, juce::Colours::darkgrey);
		labelExport.setColour(juce::Label::backgroundColourId, juce::Colours::darkgrey);
		labelUpperBounds.setColour(juce::Label::textColourId, juce::Colours::white);
		labelLowerBounds.setColour(juce::Label::textColourId, juce::Colours::white);
		labelUpperBoundsX.setColour(juce::Label::textColourId, juce::Colours::white);
		labelUpperBoundsY.setColour(juce::Label::textColourId, juce::Colours::white);
		labelLowerBoundsX.setColour(juce::Label::textColourId, juce::Colours::white);
		labelLowerBoundsY.setColour(juce::Label::textColourId, juce::Colours::white);
		inputLowerBoundsX.setColour(juce::Label::outlineColourId, juce::Colours::black);
		inputUpperBoundsX.setColour(juce::Label::outlineColourId, juce::Colours::black);
		inputLowerBoundsY.setColour(juce::Label::outlineColourId, juce::Colours::black);
		inputUpperBoundsY.setColour(juce::Label::outlineColourId, juce::Colours::black);
		labelSelectTrace.setColour(juce::Label::textColourId, juce::Colours::white);
		labelAppearanceMode.setColour(juce::Label::textColourId, juce::Colours::white);
		labelGrid.setColour(juce::Label::textColourId, juce::Colours::white);
		labelCursor.setColour(juce::Label::textColourId, juce::Colours::white);
		labelPeak.setColour(juce::Label::textColourId, juce::Colours::white);
		labelCursorValue.setColour(juce::Label::textColourId, juce::Colours::white);
		labelPeakValue.setColour(juce::Label::textColourId, juce::Colours::white);
		labelDropdownWindow.setColour(juce::Label::textColourId, juce::Colours::white);
		labelDropdownSize.setColour(juce::Label::textColourId, juce::Colours::white);
		labelDropdownAxis.setColour(juce::Label::textColourId, juce::Colours::white);
		buttonExport.setColour(juce::TextButton::buttonColourId, juce::Colours::white);
		buttonExport.setColour(juce::TextButton::buttonOnColourId, juce::Colours::white);
		buttonExport.setColour(juce::TextButton::textColourOnId, juce::Colours::black);
		buttonExport.setColour(juce::TextButton::textColourOffId, juce::Colours::black);
	}
	else {
		labelImportAudio.setColour(juce::Label::backgroundColourId, juce::Colours::grey);
		labelZoom.setColour(juce::Label::backgroundColourId, juce::Colours::grey);
		labelExport.setColour(juce::Label::backgroundColourId, juce::Colours::grey);
		labelUpperBounds.setColour(juce::Label::textColourId, juce::Colours::darkgrey);
		labelLowerBounds.setColour(juce::Label::textColourId, juce::Colours::darkgrey);
		labelUpperBoundsX.setColour(juce::Label::textColourId, juce::Colours::darkgrey);
		labelUpperBoundsY.setColour(juce::Label::textColourId, juce::Colours::darkgrey);
		labelLowerBoundsX.setColour(juce::Label::textColourId, juce::Colours::darkgrey);
		labelLowerBoundsY.setColour(juce::Label::textColourId, juce::Colours::darkgrey);
		inputLowerBoundsX.setColour(juce::Label::outlineColourId, juce::Colours::darkgrey);
		inputUpperBoundsX.setColour(juce::Label::outlineColourId, juce::Colours::darkgrey);
		inputLowerBoundsY.setColour(juce::Label::outlineColourId, juce::Colours::darkgrey);
		inputUpperBoundsY.setColour(juce::Label::outlineColourId, juce::Colours::darkgrey);
		labelSelectTrace.setColour(juce::Label::textColourId, juce::Colours::darkgrey);
		labelAppearanceMode.setColour(juce::Label::textColourId, juce::Colours::darkgrey);
		labelGrid.setColour(juce::Label::textColourId, juce::Colours::darkgrey);
		labelCursor.setColour(juce::Label::textColourId, juce::Colours::darkgrey);
		labelPeak.setColour(juce::Label::textColourId, juce::Colours::darkgrey);
		labelCursorValue.setColour(juce::Label::textColourId, juce::Colours::black);
		labelPeakValue.setColour(juce::Label::textColourId, juce::Colours::black);
		labelDropdownWindow.setColour(juce::Label::textColourId, juce::Colours::darkgrey);
		labelDropdownSize.setColour(juce::Label::textColourId, juce::Colours::darkgrey);
		labelDropdownAxis.setColour(juce::Label::textColourId, juce::Colours::darkgrey);
		buttonExport.setColour(juce::TextButton::buttonColourId, juce::Colours::whitesmoke);
		buttonExport.setColour(juce::TextButton::buttonOnColourId, juce::Colours::whitesmoke);
		buttonExport.setColour(juce::TextButton::textColourOnId, juce::Colours::darkgrey);
		buttonExport.setColour(juce::TextButton::textColourOffId, juce::Colours::darkgrey);
	}
}

void FFTSpectrumAnalyzerAudioProcessorEditor::exportDataToCSV()
{
	juce::FileChooser chooser("Select a location to save the CSV file",
		juce::File::getSpecialLocation(juce::File::userDesktopDirectory),
		"*.csv");

	if (chooser.browseForFileToSave(true)) {
		juce::File fileToSave(chooser.getResult());
		juce::FileOutputStream outputStream(fileToSave);

		if (outputStream.openedOk()) {
			//write data to .csv
			for (const auto& row : binMag) {
				for (size_t i = 0; i < row.size(); ++i) {
					outputStream << row[i];
					if (i < row.size() - 1)
						outputStream << ",";
				}
				outputStream << "\n";
			}
		}
		else {
			juce::AlertWindow::showMessageBox(juce::AlertWindow::WarningIcon,
				"Error",
				"Failed to open file for writing.");
		}
	}
}

bool FFTSpectrumAnalyzerAudioProcessorEditor::selectionSizeValid() {
	if (selectionSizeError || !audioProcessor.minBlockSize) {
		return false;
	}
	return true;
}

void FFTSpectrumAnalyzerAudioProcessorEditor::processBuffer() {

	if (sampleSelections[rowIndex].size() < stepSize) {
		selectionSizeError = true;
	}
	else {
		procBuff = true;
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
			std::copy(bufferRight.begin() + stepSize, bufferRight.end(), bufferRight.begin());
			std::copy(sampleSelections[rowIndex].begin() + bufferShift, sampleSelections[rowIndex].begin() + (bufferShift + stepSize), bufferRight.begin() + stepSize);
			std::copy(bufferRight.begin(), bufferRight.end(), windowBufferRight.begin());
			windowData.multiplyWithWindowingTable(windowBufferRight.data(), fftSize);
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
		selectionSizeError = false;
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

	if (procBuff) {
		std::fill(binMag[rowIndex].begin(), binMag[rowIndex].end(), std::numeric_limits<float>::epsilon());
		procBuff = false;
	}
	else {
		for (int i = 0; i < rowSize; i++) {
			binMag[i].resize(numBins);
			std::fill(binMag[i].begin(), binMag[i].end(), std::numeric_limits<float>::epsilon());
		}
	}
	indexToFreqMap.resize(numBins);
	std::fill(indexToFreqMap.begin(), indexToFreqMap.end(), 0.0f);
	fftCounter = 0;
}

void FFTSpectrumAnalyzerAudioProcessorEditor::mouseMove(const juce::MouseEvent& event) 
{
	float cursorX = event.getMouseDownX();
	float cursorY = event.getMouseDownY();

	//invalid bounds
	if (!inBounds(cursorX, cursorY) || !audioProcessor.minBlockSize) {
		labelCursorValue.setText("(0.0 Hz, 0.00 dB)", juce::dontSendNotification);
	} //valid bounds
	else {
		//get index based on cursor
		int i = calculateIndex(cursorX);
		cursorIndex = i;

		if (cursorIndex >= binMag[rowIndex].size()) {
			return;
		}
		else {
			if (plotInfo[rowIndex].isVisible) {
				//calculate x coordinate
				float xCoord = setToLog ? std::pow(logPower, screenToGraph(calculateX(i))) : screenToGraph(calculateX(i));
				//set label text
				if (darkMode) {
					labelCursorValue.setColour(juce::Label::textColourId, juce::Colours::white);
				}
				else {
					labelCursorValue.setColour(juce::Label::textColourId, juce::Colours::black);
				}
				labelCursorValue.setText("(" + floatToStringPrecision(xCoord, precisionValue1) + xAxisValueText + floatToStringPrecision(getYCoord(i), precisionValue2) + yAxisValueText + ")", juce::dontSendNotification);
			}
			repaint();
		}
	}
}

float FFTSpectrumAnalyzerAudioProcessorEditor::calculateX(int index) 
{
	float range = xMax - xMin;
	float ratio = getAxisLength('x') / range;
	float shift = -xMin * ratio;

	if (index >= indexToFreqMap.size()) {
		return 25000;
	}

	if (setToLog) {
		return std::log10(indexToFreqMap[index]) * ratio + xMarginXYAxis + shift;
	}
	else {
		return indexToFreqMap[index] * ratio + xMarginXYAxis + shift;
	}
}

float FFTSpectrumAnalyzerAudioProcessorEditor::calculateY(int index) 
{
	float range = yMax - yMin;
	int axis = getAxisLength('y');
	float ratio = -axis / range;
	float shift = (range - 2.0f * yMax) * ratio / 2.0f;
	int start = (paddingSmall + axis) / 2;
	
	return std::log10(binMag[rowIndex][index]) * logScale * ratio + start + shift;
}

int FFTSpectrumAnalyzerAudioProcessorEditor::calculateIndex(float cursor)
{
	int index = 1;
	while (calculateX(index) < cursor) {
		index++;
	}
	return index;
}

int FFTSpectrumAnalyzerAudioProcessorEditor::findPeak(int samples) 
{
	int leftPeak, rightPeak;
	leftPeak = rightPeak = cursorIndex;

	//check right
	for (int i = cursorIndex + 1; i < std::min(cursorIndex + samples, fftSize / 2); ++i) {
		if (i >= binMag[rowIndex].size() || rightPeak >= binMag[rowIndex].size()) {
			break;
		}
		if (getYCoord(i) > getYCoord(rightPeak)) {
			rightPeak = i;
		}
	}
	//check left
	if (cursorIndex > 0) {
		for (int i = cursorIndex - 1; i > std::max(cursorIndex - (samples / 2), 0); --i) {
			if (i >= binMag[rowIndex].size() || leftPeak >= binMag[rowIndex].size()) {
				break;
			}
			if (getYCoord(i) > getYCoord(leftPeak)) {
				leftPeak = i;
			}
		}
	}
	//calculate distance from cursor
	int leftDistance = std::abs(cursorIndex - leftPeak);
	int rightDistance = std::abs(rightPeak - cursorIndex);

	//if at peak
	if (cursorIndex == leftPeak)
		return rightPeak;
	if (cursorIndex == rightPeak)
		return leftPeak;

	//return closest peak
	if (leftDistance < rightDistance)
		return leftPeak;
	else
		return rightPeak;
}


bool FFTSpectrumAnalyzerAudioProcessorEditor::inBounds(float x, float y) 
{
	float graphWest = xMarginXYAxis;
	float graphNorth = paddingSmall;
	float graphEast = xMarginXYAxis + getAxisLength('x');
	float graphSouth = paddingSmall + getAxisLength('y');

	if (x < graphWest || x > graphEast || y < graphNorth || y > graphSouth) {
		return false;
	}
	return true;
}

float FFTSpectrumAnalyzerAudioProcessorEditor::getYCoord(int index) 
{
	return logScale * std::log10(binMag[rowIndex][index]);
}

float FFTSpectrumAnalyzerAudioProcessorEditor::screenToGraph(float screenCoord) 
{
	float range = xMax - xMin;
	float ratio = getAxisLength('x') / range;
	float shift = -xMin * ratio;
	
	return (screenCoord + shift - xMarginXYAxis) / ratio;}

float FFTSpectrumAnalyzerAudioProcessorEditor::graphToScreen(int graphCoord) 
{
	float range = xMax - xMin;
	float ratio = getAxisLength('x') / range;
	float shift = -xMin * ratio;

	return (graphCoord * ratio) + xMarginXYAxis - shift;
}

std::string FFTSpectrumAnalyzerAudioProcessorEditor::floatToStringPrecision(float f, int p)
{
	std::ostringstream oss;
	oss << std::fixed << std::setprecision(p) << f;
	return oss.str();
}