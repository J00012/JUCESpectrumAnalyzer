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
bool FFTSpectrumAnalyzerAudioProcessorEditor::isGraph = false;
bool FFTSpectrumAnalyzerAudioProcessorEditor::isVisiblePlot1 = true;
bool FFTSpectrumAnalyzerAudioProcessorEditor::isVisiblePlot2 = true;
float FFTSpectrumAnalyzerAudioProcessorEditor::xMinPrev = 0;
float FFTSpectrumAnalyzerAudioProcessorEditor::xMin = 0;
float FFTSpectrumAnalyzerAudioProcessorEditor::xMaxPrev = 100;
float FFTSpectrumAnalyzerAudioProcessorEditor::xMax = 8000;
float FFTSpectrumAnalyzerAudioProcessorEditor::yMinPrev = -1;
float FFTSpectrumAnalyzerAudioProcessorEditor::yMin = -90;
float FFTSpectrumAnalyzerAudioProcessorEditor::yMaxPrev = 1;
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

bool FFTSpectrumAnalyzerAudioProcessorEditor::setToLog = false;

//Processor vectors
std::vector<float> FFTSpectrumAnalyzerAudioProcessorEditor::indexToFreqMap = { 0 };
std::vector< std::vector<float>> FFTSpectrumAnalyzerAudioProcessorEditor::binMag;
std::vector< std::vector<float>> FFTSpectrumAnalyzerAudioProcessorEditor::sampleSelections;
std::vector<float> FFTSpectrumAnalyzerAudioProcessorEditor::bufferRight = { 0 };
std::vector<float> FFTSpectrumAnalyzerAudioProcessorEditor::bufferLeft = { 0 };
std::vector<float> FFTSpectrumAnalyzerAudioProcessorEditor::windowBufferRight = { 0 };
std::vector<float> FFTSpectrumAnalyzerAudioProcessorEditor::windowBufferLeft = { 0 };

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
	addAndMakeVisible(labelPeakPlot);
	addAndMakeVisible(labelDropdownWindow);
	addAndMakeVisible(labelDropdownAxis);
	addAndMakeVisible(labelSizeDropdown);
	addAndMakeVisible(comboboxWindowFunction);
	addAndMakeVisible(comboboxAxis);
	addAndMakeVisible(comboboxSize);
	addAndMakeVisible(buttonExport);
	addAndMakeVisible(buttonPlot1);
	addAndMakeVisible(buttonPlot2);
	addAndMakeVisible(toggleButtonPlot1);
	addAndMakeVisible(toggleButtonPlot2);

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
	labelSizeDropdown.setFont(juce::Font("Arial", 14.0f, juce::Font::bold));

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
	labelSizeDropdown.setText("Size", juce::dontSendNotification);
	labelDropdownAxis.setText("Axis", juce::dontSendNotification);
	inputXmin.setText(std::to_string(xMin), juce::dontSendNotification);
	inputXmax.setText(std::to_string(xMax), juce::dontSendNotification);
	inputYmin.setText(std::to_string(yMin), juce::dontSendNotification);
	inputYmax.setText(std::to_string(yMax), juce::dontSendNotification);

	// Colour
	labelPlot1.setColour(juce::Label::textColourId, juce::Colours::darkgrey);
	labelPlot2.setColour(juce::Label::textColourId, juce::Colours::darkgrey);
	labelImportAudio.setColour(juce::Label::backgroundColourId, juce::Colours::darkgrey);
	labelZoom.setColour(juce::Label::backgroundColourId, juce::Colours::darkgrey);
	labelCursor.setColour(juce::Label::textColourId, juce::Colours::white);
	labelPeak.setColour(juce::Label::textColourId, juce::Colours::white);
	labelDropdownWindow.setColour(juce::Label::textColourId, juce::Colours::white);
	labelDropdownAxis.setColour(juce::Label::textColourId, juce::Colours::white);
	labelSizeDropdown.setColour(juce::Label::textColourId, juce::Colours::white);
	labelExport.setColour(juce::Label::backgroundColourId, juce::Colours::darkgrey);
	comboboxWindowFunction.setColour(juce::ComboBox::backgroundColourId, juce::Colours::white);
	comboboxWindowFunction.setColour(juce::ComboBox::textColourId, juce::Colours::black);
	comboboxWindowFunction.setColour(juce::ComboBox::arrowColourId, juce::Colours::darkgrey);
	comboboxAxis.setColour(juce::ComboBox::backgroundColourId, juce::Colours::white);
	comboboxAxis.setColour(juce::ComboBox::textColourId, juce::Colours::black);
	comboboxAxis.setColour(juce::ComboBox::arrowColourId, juce::Colours::darkgrey);
	comboboxSize.setColour(juce::ComboBox::backgroundColourId, juce::Colours::white);
	comboboxSize.setColour(juce::ComboBox::textColourId, juce::Colours::black);
	comboboxSize.setColour(juce::ComboBox::arrowColourId, juce::Colours::darkgrey);
	buttonExport.setColour(juce::TextButton::buttonColourId, juce::Colours::white);
	buttonExport.setColour(juce::TextButton::textColourOnId, juce::Colours::black);
	buttonExport.setColour(juce::TextButton::textColourOffId, juce::Colours::black);
	toggleButtonPlot1.setColour(juce::ToggleButton::tickColourId, juce::Colours::white);
	toggleButtonPlot1.setColour(juce::ToggleButton::tickDisabledColourId, juce::Colours::lightgrey);
	toggleButtonPlot2.setColour(juce::ToggleButton::tickColourId, juce::Colours::white);
	toggleButtonPlot2.setColour(juce::ToggleButton::tickDisabledColourId, juce::Colours::lightgrey);
	inputXmin.setColour(juce::Label::backgroundColourId, juce::Colours::white);
	inputXmin.setColour(juce::Label::textColourId, juce::Colours::black);
	inputXmin.setColour(juce::Label::textWhenEditingColourId, juce::Colours::black);
	inputXmax.setColour(juce::Label::backgroundColourId, juce::Colours::white);
	inputXmax.setColour(juce::Label::textColourId, juce::Colours::black);
	inputXmax.setColour(juce::Label::textWhenEditingColourId, juce::Colours::black);
	inputYmin.setColour(juce::Label::backgroundColourId, juce::Colours::white);
	inputYmin.setColour(juce::Label::textColourId, juce::Colours::black);
	inputYmin.setColour(juce::Label::textWhenEditingColourId, juce::Colours::black);
	inputYmax.setColour(juce::Label::backgroundColourId, juce::Colours::white);
	inputYmax.setColour(juce::Label::textColourId, juce::Colours::black);
	inputYmax.setColour(juce::Label::textWhenEditingColourId, juce::Colours::black);

	// Editable
	labelCursorValue.setEditable(false);
	inputXmin.setEditable(true);
	inputXmax.setEditable(true);
	inputYmin.setEditable(true);
	inputYmax.setEditable(true);
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
	comboboxAxis.addItem("Linear Frequency", 1);
	comboboxAxis.addItem("Log Frequency", 2);
	comboboxSize.addItem("128", 1);
	comboboxSize.addItem("256", 2);
	comboboxSize.addItem("512", 3);
	comboboxSize.addItem("1024", 4);

	// Set Toggle State
	buttonPlot1.setClickingTogglesState(true);
	buttonPlot2.setClickingTogglesState(true);
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


	// Set Selection
	comboboxWindowFunction.setSelectedId(5);
	comboboxAxis.setSelectedId(2);
	comboboxSize.setSelectedId(4);

	// On user interaction
	toggleButtonPlot1.onClick = [this] { updateToggleState(1); };
	toggleButtonPlot2.onClick = [this] { updateToggleState(2); };
	inputXmin.onTextChange = [this] { getBounds(); };
	inputXmax.onTextChange = [this] { getBounds(); };
	inputYmin.onTextChange = [this] { getBounds(); };
	inputYmax.onTextChange = [this] { getBounds(); };
	comboboxWindowFunction.onChange = [this] { setWindowFunction(); };
	comboboxAxis.onChange = [this] { setAxisType(); };
	comboboxSize.onChange = [this] { setBlockSize(); };
	buttonPlot1.onClick = [&]() {
		plotIndexSelection = 0;
		setPlotIndex(0);
	};
	buttonPlot2.onClick = [&]() {
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
	juce::Path xAxis;
	juce::Path xAxisMarkers;
	juce::Path yAxis;
	juce::Path yAxisMarkersUp;
	juce::Path yAxisMarkersDown;
	juce::Path zeroTick;

	//** graph scaling variables **//
	float magic_num1 = 0.295;
	float magic_num2 = 240;
	float magic_num3 = 2;
	float magic_num4 = 12;
	float magic_num5 = 0.95;
	float magic_num6 = 1;
	float magic_num7 = 3;
	float xAxisScale = 0.702;

	float border_xBuffer = getWidth() * magic_num1;
	float border_yBuffer = y_componentOffset;
	float widthBorder = getWidth() - xOffsetComponent;
	float heightBorder = getHeight() - magic_num2;
	float xBuffer = border_xBuffer + magic_num3;
	float yBuffer = border_yBuffer + magic_num4;
	float lengthXAxis = widthBorder;
	float lengthYAxis = heightBorder * magic_num5;
	float yStartXYAxis = yBuffer + lengthYAxis - magic_num6;
	float xStartXYAxis = xBuffer - magic_num7;
	float yStartPlot = (yBuffer + lengthYAxis) / 2;

	int sampleSize = 100;  // Adjust the number of samples being displayed as needed
	int logScale = 40;
	int tickWidth = 5;
	int zeroTickWidth = 15;


	if (newSelection == true) {
		audioProcessor.setStepSize(stepSize);                             //this needs to be changed when the size is changed
		sampleSelections[rowIndex] = audioProcessor.getAccumulationBuffer();
		audioProcessor.clearAccumulationBuffer();
		processBuffer();
		newSelection = false;
	}

	
	float xDiff = xMax - xMin;
	if (xDiff <= 0)  // handles divide by zero errors 
	{
		xMax = xMaxPrev;
		xMin = xMinPrev;
		xDiff = xMaxPrev - xMinPrev;
		inputXmin.setText(std::to_string(xMinPrev), juce::dontSendNotification);
		inputXmax.setText(std::to_string(xMaxPrev), juce::dontSendNotification);
	}
	else
	{
		xMaxPrev = xMax;
		xMinPrev = xMin;
	}
	float scaleX = lengthXAxis / xDiff;  // Scaling X increments; pixels shown per sample
	float xShift = -xMin * scaleX;

	xMax = std::log10(xMax);

	int log_xMax = xMax;

	float yDiff = yMax - yMin;
	if (yDiff <= 0)  // handles divide by zero errors
	{
		yMax = yMaxPrev;
		yMin = yMinPrev;
		yDiff = yMaxPrev - yMinPrev;
		inputYmin.setText(std::to_string(yMinPrev), juce::dontSendNotification);
		inputYmax.setText(std::to_string(yMaxPrev), juce::dontSendNotification);
	}
	else
	{
		yMaxPrev = yMax;
		yMinPrev = yMin;
	}
	float scaleY = -lengthYAxis / yDiff;  // Scaling Y increments; pixels shown per sample
	float yShift = (yDiff - 2.0f * yMax) * scaleY / 2.0f;

	float plotYShift = yStartPlot + yShift;

	// Graph plots
	if (audioProcessor.minBlockSize) {
		if (setToLog == true) {
			plot2.startNewSubPath(xStartXYAxis + xShift, yStartPlot + logScale * std::log10(binMag[1][0]) * scaleY + yShift);
			plot1.startNewSubPath(xStartXYAxis + xShift, yStartPlot + logScale * std::log10(binMag[0][0]) * scaleY + yShift);
			for (int i = 1; i < indexToFreqMap.size(); i++)
			{
				if (isVisiblePlot2 == true) {
					plot2.lineTo(std::log10(indexToFreqMap[i]) * xAxisScale * scaleX + xStartXYAxis + xShift, logScale * std::log10(binMag[1][i]) * scaleY + plotYShift);
				}
				if (isVisiblePlot1 == true) {
					plot1.lineTo(std::log10(indexToFreqMap[i]) * xAxisScale * scaleX + xStartXYAxis + xShift, logScale * std::log10(binMag[0][i]) * scaleY + plotYShift);
				}
			}
		}
		else {
			//xMax = maxFreq / 5;
			plot2.startNewSubPath(xStartXYAxis + xShift, yStartPlot + logScale * std::log10(binMag[1][0]) * scaleY + yShift);
			plot1.startNewSubPath(xStartXYAxis + xShift, yStartPlot + logScale * std::log10(binMag[0][0]) * scaleY + yShift);
			for (int i = 1; i < indexToFreqMap.size(); i++)
			{
				if (isVisiblePlot2 == true) {
					plot2.lineTo(indexToFreqMap[i] * xAxisScale * scaleX + xStartXYAxis + xShift, logScale * std::log10(binMag[1][i]) * scaleY + plotYShift);
				}
				if (isVisiblePlot1 == true) {
					plot1.lineTo(indexToFreqMap[i] * xAxisScale * scaleX + xStartXYAxis + xShift, logScale * std::log10(binMag[0][i]) * scaleY + plotYShift);
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
		g.fillRoundedRectangle(border_xBuffer, border_yBuffer, widthBorder, heightBorder, 3);
		g.setColour(juce::Colours::white);
		g.drawText("Not enough data selected", juce::Rectangle<int>(border_xBuffer, border_yBuffer, widthBorder, heightBorder), juce::Justification::centred, true);
	}

	// Axis variables
	int numXMarkers = xDiff;
	int numYMarkers = yDiff;

	// Plot X Axis Markers
	for (int i = 1; i <= numXMarkers; i++) {
		xAxisMarkers.startNewSubPath(xStartXYAxis + (i * xAxisScale * scaleX), yStartXYAxis - tickWidth);
		xAxisMarkers.lineTo(xStartXYAxis + (i * xAxisScale * scaleX), yStartXYAxis + tickWidth);
	}
	g.setColour(juce::Colours::white);
	g.strokePath(xAxisMarkers, juce::PathStrokeType(2.0f));

	// Plot Y Axis Markers
	for (int i = 1; i <= numYMarkers; i++) {
		yAxisMarkersUp.startNewSubPath(xStartXYAxis - tickWidth, yStartPlot + (scaleY * i) + yShift);
		yAxisMarkersUp.lineTo(xStartXYAxis + tickWidth, yStartPlot + (scaleY * i) + yShift);  // drawing line markers moving up from midpoint
		yAxisMarkersDown.startNewSubPath(xStartXYAxis - tickWidth, yStartPlot - (scaleY * i) + yShift);
		yAxisMarkersDown.lineTo(xStartXYAxis + tickWidth, yStartPlot - (scaleY * i) + yShift);  // drawing line markers moving up from midpoint
	}
	g.setColour(juce::Colours::white);
	g.strokePath(yAxisMarkersUp, juce::PathStrokeType(2.0f));
	g.strokePath(yAxisMarkersDown, juce::PathStrokeType(2.0f));

	//Plot zero on Y-axis
	zeroTick.startNewSubPath(xStartXYAxis - zeroTickWidth, yStartPlot + yShift);
	zeroTick.lineTo(xStartXYAxis + zeroTickWidth, yStartPlot + yShift);
	g.strokePath(zeroTick, juce::PathStrokeType(3.0f));

	//** draw graph border **//
	juce::Path graphBoundary;
	graphBoundary.startNewSubPath(border_xBuffer, border_yBuffer);
	graphBoundary.lineTo(widthBorder, border_yBuffer);
	graphBoundary.lineTo(widthBorder, heightBorder);
	graphBoundary.lineTo(border_xBuffer, heightBorder);
	graphBoundary.lineTo(border_xBuffer, border_yBuffer);
	g.setColour(juce::Colours::slategrey);
	g.strokePath(graphBoundary, juce::PathStrokeType(1.0f));

	//** draw boxes to hide out of bound plots **//
	int originPixel = 0;

	int xOffsetLeftBox = originPixel;
	int xOffsetTopBox = originPixel;
	int xOffsetRightBox = widthBorder;
	//int xOffsetRightBox = widthBorder + 0.5;
	int xOffsetBottonBox = originPixel;

	int yOffsetLeftBox = originPixel;
	int yOffsetRightBox = originPixel;
	int yOffsetTopBox = originPixel;
	int yOffsetBottomBox = heightBorder;

	int widthLeftBox = border_xBuffer;
	int widthRightBox = getWidth();
	int widthTopBox = getWidth();
	int widthBottomBox = getWidth();

	int heightLeftBox = getHeight();
	int heightRightBox = getHeight();
	int heightTopBox = border_yBuffer;
	int heightBottomBox = getHeight();
	
	juce::Rectangle<int> leftPanel(xOffsetLeftBox, yOffsetLeftBox, widthLeftBox, heightLeftBox);
	juce::Rectangle<int> rightPanel(xOffsetRightBox, yOffsetRightBox, widthRightBox, heightRightBox);
	juce::Rectangle<int> topPanel(xOffsetTopBox, yOffsetTopBox, widthTopBox, heightTopBox);
	juce::Rectangle<int> bottomPanel(xOffsetBottonBox, yOffsetBottomBox, widthBottomBox, heightBottomBox);
	g.setColour(juce::Colours::black);
	g.fillRect(leftPanel);
	g.fillRect(rightPanel);
	g.fillRect(topPanel);
	g.fillRect(bottomPanel);

	//** line to separate left-side components and right-side components **//
	g.setColour(juce::Colours::darkgrey);
	g.fillRect(widthPrimaryCategoryLabel, originPixel, lineHeight, windowMaxHeight);



	//** white box for cursor label **//
	int xMargin_cursorBox = xStartXYAxis + 138;
	int yMargin_cursorBox = heightBorder + 55;
	int width_cursorBox = 180;
	int height_cursorBox = 26;

	//** white box for peak label **//
	int xMargin_peakBox = xMargin_cursorBox + 205;
	int yMargin_peakBox = yMargin_cursorBox;
	int width_peakBox = 180;
	int height_peakBox = 26;

	// draw white box
	g.setColour(juce::Colours::white);
	g.fillRoundedRectangle(xOffsetComponent, yMarginSelectionBox, widthSelectionBox, heightSelectionBox, cornerSizeSelectionBox);

	// fill in checkboxes
	if (isVisiblePlot1 == true) {
		g.setColour(juce::Colours::dodgerblue);
		g.fillRoundedRectangle(xMarginCheckboxFill, yMarginCheckbox1Fill, widthCheckbox, heightCheckbox, cornersizeCheckbox);
	}
	if (isVisiblePlot2 == true) {
		g.setColour(juce::Colours::dodgerblue);
		g.fillRoundedRectangle(xMarginCheckboxFill, yMarginCheckbox2Fill, widthCheckbox, heightCheckbox, cornersizeCheckbox);
	}

	// draw line to seprate plot selections
	g.setColour(juce::Colours::lightgrey);
	g.fillRect(xMarginSelectionBoundary, yMarginSelectionBoundary, widthSelectionBoundary, heightSelectionBoundary);

	//** line to separate upper and lower x/y bounds in ZOOM **//
	g.setColour(juce::Colours::darkgrey);
	g.fillRect(xMarginZoomBoundary, yMarginZoomBoundary, widthZoomBoundary, heightZoomBoundary);


	// Peak 
	float cursorYPeak = findPeak();
	if (cursorYPeak != 0) {
		g.setColour(juce::Colours::red);
		juce::Rectangle<int> peakLine(graphToScreen(cursorPeak), y_componentOffset, 1, lengthYAxis);
		g.fillRect(peakLine);
		g.setColour(juce::Colours::white);
		if (isVisiblePlot1)
			labelPeakPlot.setText("(" + floatToStringPrecision((float)cursorPeak, 1) + ", " + floatToStringPrecision(cursorYPeak, 2) + ")", juce::dontSendNotification);
		if (isVisiblePlot2)
			labelPeakPlot.setText("(" + floatToStringPrecision((float)cursorPeak, 1) + ", " + floatToStringPrecision(cursorYPeak, 2) + ")", juce::dontSendNotification);
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
	// This is generally where you'll want to lay out the positions of any
	// subcomponents in your editor..

	//** graph scaling variables **//
	float border_xBuffer = getWidth() * 0.295;
	float border_yBuffer = y_componentOffset;
	float widthBorder = getWidth() - xOffsetComponent;
	float heightBorder = getHeight() - 240;
	float xBuffer = border_xBuffer + 2;
	float yBuffer = border_yBuffer + 12;
	float lengthXAxis = widthBorder;
	float lengthYAxis = heightBorder * .95;
	float yStartXYAxis = yBuffer + lengthYAxis - 1;
	float xStartXYAxis = xBuffer - 3;
	float yStartPlot = (yBuffer + lengthYAxis) / 2;

	//** margins for primary labels **//
	int yMargin_selectTraceLabel = heightPrimaryCategoryLabel + yOffsetPrimarySecondaryLabel;
	int yMargin_zoomLabel = yMargin_selectTraceLabel + (22.5 * y_componentOffset);
	int yMargin_exportLabel = yMargin_selectTraceLabel + (42 * y_componentOffset);

	// secondary gui element width
	int width_toggleButton = 30;
	int width_plotLabel = 50;
	int width_selectButton = 90;
	int width_inputTextbox = 60;
	int width_exportButton = 95;
	int width_comboBox = 160;

	// secondary gui element height
	int heightControlWidget = 24;
	int height_toggleButton = heightControlWidget;
	int height_plotLabel = heightControlWidget;
	int height_selectButton = heightControlWidget;
	int height_inputTextbox = heightControlWidget - 2;
	int height_exportButton = heightControlWidget + 4;
	int height_comboBox = 30;

	//** margins for combo

	//** cursor *//
	// label
	int xMargin_cursorLabel = xStartXYAxis + 133;
	int yMargin_cursorLabel = heightBorder + 30;
	int xMargin_cursorPlot = xMargin_cursorLabel;
	int yMargin_cursorPlot = yMargin_cursorLabel + 22;


	//** peak **//
	// label
	int xMargin_peakLabel = xMargin_cursorLabel + 205;
	int yMargin_peaklabel = yMargin_cursorLabel;
	int xMargin_peakPlot = xMargin_peakLabel;
	int yMargin_peakPlot = yMargin_cursorPlot;


	//** window function **//
	//label
	int xMargin_windowLabel = xStartXYAxis + 65;
	int yMargin_windowLabel = yMargin_peaklabel + 62;
	// combobox
	int xMargin_winCombo = xMargin_windowLabel + 4;
	int yMargin_winCombo = yMargin_windowLabel + 22;

	//** axis **//
	// label
	int xMargin_axisLabel = xMargin_winCombo + 180;
	int yMargin_axisLabel = yMargin_windowLabel;
	// combobox
	int xMargin_axisCombo = xMargin_axisLabel + 4;
	int yMargin_axisCombo = yMargin_winCombo;


	//** size **//
	// label
	int xMargin_sizeLabel = xMargin_axisLabel + 180;
	int yMargin_sizeLabel = yMargin_axisLabel;
	// combobox
	int xMargin_sizeCombo = xMargin_sizeLabel + 4;
	int yMargin_sizeCombo = yMargin_winCombo;

	//** Set bounds for right side elements **//
	labelCursorValue.setBounds(xMargin_cursorLabel, yMargin_cursorLabel, width_secondaryLabel, heightSecondaryLabel);
	labelCursorValue.setBounds(xMargin_cursorPlot, yMargin_cursorPlot, width_secondaryLabel, heightSecondaryLabel);
	labelPeak.setBounds(xMargin_peakLabel, yMargin_peaklabel, width_secondaryLabel, heightSecondaryLabel);
	labelPeakPlot.setBounds(xMargin_peakPlot, yMargin_peakPlot, width_secondaryLabel, heightSecondaryLabel);
	labelWindowDropdown.setBounds(xMargin_windowLabel, yMargin_windowLabel, width_secondaryLabel, heightSecondaryLabel);
	labelDropdownAxis.setBounds(xMargin_axisLabel, yMargin_axisLabel, width_secondaryLabel, heightSecondaryLabel);
	labelSizeDropdown.setBounds(xMargin_sizeLabel, yMargin_sizeLabel, width_secondaryLabel, heightSecondaryLabel);

	//cursorFunction.setBounds(xMargin_cursorCombo, yMargin_cursorCombo, width_comboBox, height_comboBox);
	//peakFunction.setBounds(xMargin_peakCombo, yMargin_peakCombo, width_comboBox, height_comboBox);
	comboboxWindowFunction.setBounds(xMargin_winCombo, yMargin_winCombo, width_comboBox, height_comboBox);
	comboboxAxis.setBounds(xMargin_axisCombo, yMargin_axisCombo, width_comboBox, height_comboBox);
	size.setBounds(xMargin_sizeCombo, yMargin_sizeCombo, width_comboBox, height_comboBox);


	//** plot 1 **//
	// toggle button 1
	int xMargin_toggleButton1 = 2 * xOffsetComponent;
	int yMargin_toggleButton1 = heightPrimaryCategoryLabel + yOffsetPrimarySecondaryLabel + heightSecondaryLabel + (6 * yOffsetSelectionBox);
	// plot label 1
	int xMargin_plotLabel1 = 4 * xMargin_toggleButton1;
	int yMargin_plotLabel1 = yMargin_toggleButton1;
	// selection button 1
	int xMargin_selectButton1 = 3.5 * xMargin_plotLabel1;
	int yMargin_selectButton1 = yMargin_toggleButton1;

	//** plot 2 **//
	// toggle button 2
	int xMargin_toggleButton2 = xMargin_toggleButton1;
	int yMargin_toggleBotton2 = yMargin_toggleButton1 + (23 * yOffsetSelectionBox);
	// plot label 2
	int xMargin_plotLabel2 = xMargin_plotLabel1;
	int yMargin_plotLabel2 = yMargin_toggleBotton2;
	// selection button 2
	int xMargin_selectButton2 = xMargin_selectButton1;
	int yMargin_selectButton2 = yMargin_toggleBotton2;

	//** upper bounds **//
	// upper label
	int yMargin_upperLabel = yMargin_zoomLabel + heightPrimaryCategoryLabel + yOffsetPrimarySecondaryLabel;

	// xMax input
	int xMargin_xMax = 10 * xOffsetComponent;
	int yMargin_xMax = yMargin_upperLabel + 2;

	// x label
	int xMargin_xMaxLabel = 20.5 * xOffsetComponent;
	int yMargin_xMaxLabel = yMargin_upperLabel;

	// yMax input
	int xMargin_yMax = 30 * xOffsetComponent;
	int yMargin_yMax = yMargin_upperLabel + 2;

	// y label
	int xMargin_yMaxLabel = 40.5 * xOffsetComponent;;
	int yMargin_yMaxLabel = yMargin_upperLabel;

	//** lower bounds **//
	// lower label
	int yMargin_lowerLabel = yMargin_upperLabel + (8 * y_componentOffset);

	// xMin input
	int xMargin_xMin = xMargin_xMax;
	int yMargin_xMin = yMargin_lowerLabel + 2;

	// x label
	int xMargin_xMinLabel = xMargin_xMaxLabel;
	int yMargin_xMinLabel = yMargin_lowerLabel;

	// yMin input
	int xMargin_yMin = xMargin_yMax;
	int yMargin_yMin = yMargin_lowerLabel + 2;

	// y label
	int xMargin_yMinLabel = xMargin_yMaxLabel;
	int yMargin_yMinLabel = yMargin_lowerLabel;

	//** export button **//
	int xMargin_exportButton = xOffsetComponent;
	int yMargin_exportButton = yMargin_exportLabel + heightPrimaryCategoryLabel + (1.5 * yOffsetPrimarySecondaryLabel);

	//** set bounds for GUI elements **//
	labelImportAudio.setBounds(0, 0, widthPrimaryCategoryLabel, heightPrimaryCategoryLabel);
	labelSelectTrace.setBounds(0, yMargin_selectTraceLabel, width_secondaryLabel, heightSecondaryLabel);
	labelZoom.setBounds(0, yMargin_zoomLabel, widthPrimaryCategoryLabel, heightPrimaryCategoryLabel);
	labelExport.setBounds(0, yMargin_exportLabel, widthPrimaryCategoryLabel, heightPrimaryCategoryLabel);

	//** set bounds for secondary GUI elements **//
	toggleButtonPlot1.setBounds(xMargin_toggleButton1, yMargin_toggleButton1, width_toggleButton, height_toggleButton);
	toggleButtonPlot2.setBounds(xMargin_toggleButton2, yMargin_toggleBotton2, width_toggleButton, height_toggleButton);

	labelPlot1.setBounds(xMargin_plotLabel1, yMargin_plotLabel1, width_plotLabel, height_plotLabel);
	labelPlot2.setBounds(xMargin_plotLabel2, yMargin_plotLabel2, width_plotLabel, height_plotLabel);

	buttonPlot1.setBounds(xMargin_selectButton1, yMargin_selectButton1, width_selectButton, height_selectButton);
	buttonPlot2.setBounds(xMargin_selectButton2, yMargin_selectButton2, width_selectButton, height_selectButton);

	labelUpperBounds.setBounds(0, yMargin_upperLabel, width_secondaryLabel, heightSecondaryLabel);
	inputXmax.setBounds(xMargin_xMax, yMargin_xMax, width_inputTextbox, height_inputTextbox);
	inputUpperBoundsX.setBounds(xMargin_xMaxLabel, yMargin_xMaxLabel, width_secondaryLabel, heightSecondaryLabel);
	inputYmax.setBounds(xMargin_yMax, yMargin_yMax, width_inputTextbox, height_inputTextbox);
	inputUpperBoundsY.setBounds(xMargin_yMaxLabel, yMargin_yMaxLabel, width_inputTextbox, height_inputTextbox);

	labelLowerBounds.setBounds(0, yMargin_lowerLabel, width_secondaryLabel, heightSecondaryLabel);
	inputXmin.setBounds(xMargin_xMin, yMargin_xMin, width_inputTextbox, height_inputTextbox);
	inputLowerBoundsX.setBounds(xMargin_xMinLabel, yMargin_xMinLabel, width_secondaryLabel, heightSecondaryLabel);
	inputYmin.setBounds(xMargin_yMax, yMargin_yMin, width_inputTextbox, height_inputTextbox);
	inputLowerBoundsY.setBounds(xMargin_yMinLabel, yMargin_yMinLabel, width_secondaryLabel, heightSecondaryLabel);

	buttonExport.setBounds(xMargin_exportButton, yMargin_exportButton, width_exportButton, height_exportButton);
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
	float minVal = -1000;
	float maxVal = 24000;
	juce::String temp = inputXmin.getText(false);
	float val = std::atof(temp.toStdString().c_str());
	if (val >= minVal && val <= maxVal)
	{
		xMin = val;
	}
	else { inputXmin.setText(std::to_string(xMin), juce::dontSendNotification); }

	temp = inputXmax.getText(false);
	val = std::atof(temp.toStdString().c_str());
	if (val >= minVal && val <= maxVal)
	{
		xMax = val;
	}
	else { inputXmax.setText(std::to_string(xMax), juce::dontSendNotification); }

	temp = inputYmin.getText(false);
	val = std::atof(temp.toStdString().c_str());
	if (val >= minVal && val <= maxVal)
	{
		yMin = val;
	}
	else { inputYmin.setText(std::to_string(yMin), juce::dontSendNotification); }

	temp = inputYmax.getText(false);
	val = std::atof(temp.toStdString().c_str());
	if (val >= minVal && val <= maxVal)
	{
		yMax = val;
	}
	else { inputYmax.setText(std::to_string(yMax), juce::dontSendNotification); }
	repaint();
}


void FFTSpectrumAnalyzerAudioProcessorEditor::setPlotIndex(int plotIndex)
{
	rowIndex = plotIndex;
	audioProcessor.setRowIndex(rowIndex);
	if (plotIndex == 0)
	{
		buttonPlot1.setButtonText("Selected");
		buttonPlot2.setButtonText("Select");
	}
	else if (plotIndex == 1)
	{
		buttonPlot2.setButtonText("Selected");
		buttonPlot1.setButtonText("Select");
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
	auto selection = size.getText();
	fftSize = selection.getIntValue();
	setFreqData(fftSize);
	//audioProcessor.setInitialAccBuffer();
	//processBuffer();
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
	float graphNorth = y_componentOffset;
	float graphEast = (getWidth() - xOffsetComponent) + 0.5;
	float graphSouth = (getHeight() - 240) * 0.95;

	cursorX = event.getMouseDownX();
	float cursorY = event.getMouseDownY();
	//invalid bounds
	if (cursorX < graphWest || cursorX > graphEast || cursorY < graphNorth || cursorY > graphSouth || screenToGraph(cursorX) < 1) {
		isGraph = false;
		labelCursorValue.setText("(0.0, 0.00)", juce::dontSendNotification);
	}
	//valid bounds
	else {
		isGraph = true;
		cursorX = screenToGraph(cursorX);
		if (plotIndexSelection == 1 && isVisiblePlot2) {
			labelCursorValue.setText("(" + floatToStringPrecision(cursorX, 1) + ", " + floatToStringPrecision(getYCoord(1, setToLog, (int)cursorX), 2) + ")", juce::dontSendNotification);
		}
		if (plotIndexSelection == 0 && isVisiblePlot1) {
			labelCursorValue.setText("(" + floatToStringPrecision(cursorX, 1) + ", " + floatToStringPrecision(getYCoord(0, setToLog, (int)cursorX), 2) + ")", juce::dontSendNotification);
		}
	}
	repaint();
}

float FFTSpectrumAnalyzerAudioProcessorEditor::findPeak()
{
	int p = plotIndexSelection;

	float maxValue = getYCoord(p, setToLog, 0);
	for (size_t i = 1; i < binMag[p].size(); ++i) {
		if (getYCoord(p, setToLog, i) > maxValue) {
			maxValue = getYCoord(p, setToLog, i);
			cursorPeak = i;
		}
	}
	//rob sucks
	//rian swallows
	return maxValue;
}

float FFTSpectrumAnalyzerAudioProcessorEditor::getYCoord(int plotNumber, bool log, int index) {
	//if (log) {
	return 40 * std::log10(binMag[plotNumber][index]);
	//}
	//else {
	//	return binMag[plotNumber][index];
	//}
}

float FFTSpectrumAnalyzerAudioProcessorEditor::screenToGraph(float screenCoord) {
	float graphWest = (getWidth() * 0.295) - 1;
	float graphNorth = y_componentOffset;
	float graphEast = (getWidth() - xOffsetComponent) + 0.5;
	float graphSouth = (getHeight() - 240) * 0.95;

	screenCoord -= graphWest;
	screenCoord = (screenCoord * (100)) / (graphEast - graphWest);
	return screenCoord;
}

float FFTSpectrumAnalyzerAudioProcessorEditor::graphToScreen(int graphCoord) {
	float graphWest = (getWidth() * 0.295) - 1;
	float graphNorth = y_componentOffset;
	float graphEast = (getWidth() - xOffsetComponent) + 0.5;
	float graphSouth = (getHeight() - 240) * 0.95;

	graphCoord = (graphCoord * (graphEast - graphWest)) / (100);
	graphCoord += graphWest;
	return graphCoord;
}

std::string FFTSpectrumAnalyzerAudioProcessorEditor::floatToStringPrecision(float f, int p)
{
	std::ostringstream oss;
	oss << std::fixed << std::setprecision(p) << f;
	return oss.str();
}