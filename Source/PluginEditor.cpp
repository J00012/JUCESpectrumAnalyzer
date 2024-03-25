/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <string>

bool FFTSpectrumAnalyzerAudioProcessorEditor::isRunning = false;
int FFTSpectrumAnalyzerAudioProcessorEditor::xMinPrev = 0;
int FFTSpectrumAnalyzerAudioProcessorEditor::xMin = 0;
int FFTSpectrumAnalyzerAudioProcessorEditor::xMaxPrev = 100;
int FFTSpectrumAnalyzerAudioProcessorEditor::xMax = 100;
int FFTSpectrumAnalyzerAudioProcessorEditor::yMinPrev = -10;
int FFTSpectrumAnalyzerAudioProcessorEditor::yMin = -10;
int FFTSpectrumAnalyzerAudioProcessorEditor::yMaxPrev = 10;
int FFTSpectrumAnalyzerAudioProcessorEditor::yMax = 10;
int FFTSpectrumAnalyzerAudioProcessorEditor::plotIndexSelection = 0;
bool FFTSpectrumAnalyzerAudioProcessorEditor::isVisiblePlot1 = true;
bool FFTSpectrumAnalyzerAudioProcessorEditor::isVisiblePlot2 = true;

int FFTSpectrumAnalyzerAudioProcessorEditor::windowWidth = 1300;
int FFTSpectrumAnalyzerAudioProcessorEditor::windowHeight = 1000+2;
int FFTSpectrumAnalyzerAudioProcessorEditor::xBuffer = 30;
int FFTSpectrumAnalyzerAudioProcessorEditor::yBuffer = 30;
int FFTSpectrumAnalyzerAudioProcessorEditor::lengthXAxis = 900;  //pixels = unit
int FFTSpectrumAnalyzerAudioProcessorEditor::lengthYAxis = 900;  //pixels = unit
int FFTSpectrumAnalyzerAudioProcessorEditor::yStartXYAxis = yBuffer + lengthYAxis;
int FFTSpectrumAnalyzerAudioProcessorEditor::xStartXYAxis = 100;
int FFTSpectrumAnalyzerAudioProcessorEditor::yStartPlot = yBuffer + lengthYAxis / 2;

//==============================================================================
FFTSpectrumAnalyzerAudioProcessorEditor::FFTSpectrumAnalyzerAudioProcessorEditor(FFTSpectrumAnalyzerAudioProcessor& p)
	: AudioProcessorEditor(&p), audioProcessor(p)
{
	setOpaque(true);
	startTimer(500);
	setSize(windowWidth, windowHeight);

	addAndMakeVisible(buttonPlot1);
	buttonPlot1.setClickingTogglesState(true);
	buttonPlot1.onClick = [&]()
		{
			setPlotIndex(0);
		};
	addAndMakeVisible(buttonPlot2);
	buttonPlot2.setClickingTogglesState(true);
	buttonPlot2.onClick = [&]()
		{
			setPlotIndex(1);
		};

	// toggle button for plot 1
	addAndMakeVisible(toggleButtonPlot1);
	if (isVisiblePlot1 == true) 
	{
		toggleButtonPlot1.setToggleState(true, true);
	}
	toggleButtonPlot1.onClick = [this] { updateToggleState(1); };
	toggleButtonPlot1.setClickingTogglesState(true);

	// toggle button for plot 2
	addAndMakeVisible(toggleButtonPlot2);
	if (isVisiblePlot2 == true) 
	{
		toggleButtonPlot2.setToggleState(true, true);
	}
	toggleButtonPlot2.onClick = [this] { updateToggleState(2); };
	toggleButtonPlot2.setClickingTogglesState(true);

	addAndMakeVisible(inputXmin);
	addAndMakeVisible(inputXmax);
	addAndMakeVisible(inputYmin);
	addAndMakeVisible(inputYmax);
	addAndMakeVisible(labelPlot1);
	addAndMakeVisible(labelPlot2);
	inputXmin.setEditable(true);
	inputXmax.setEditable(true);
	inputYmin.setEditable(true);
	inputYmax.setEditable(true);
	labelPlot1.setEditable(false);
	labelPlot2.setEditable(false);
	inputXmin.setText(std::to_string(xMin), juce::dontSendNotification);
	inputXmax.setText(std::to_string(xMax), juce::dontSendNotification);
	inputYmin.setText(std::to_string(yMin), juce::dontSendNotification);
	inputYmax.setText(std::to_string(yMax), juce::dontSendNotification);
	labelPlot1.setText("Plot 1", juce::dontSendNotification);
	labelPlot2.setText("Plot 2", juce::dontSendNotification);
	inputXmin.setColour(juce::Label::backgroundColourId, juce::Colours::black);
	inputXmax.setColour(juce::Label::backgroundColourId, juce::Colours::black);
	inputYmin.setColour(juce::Label::backgroundColourId, juce::Colours::black);
	inputYmax.setColour(juce::Label::backgroundColourId, juce::Colours::black);
	inputXmin.onTextChange = [this] { getBounds(); };
	inputXmax.onTextChange = [this] { getBounds(); };
	inputYmin.onTextChange = [this] { getBounds(); };
	inputYmax.onTextChange = [this] { getBounds(); };
}

FFTSpectrumAnalyzerAudioProcessorEditor::~FFTSpectrumAnalyzerAudioProcessorEditor()
{
}

//==============================================================================
void FFTSpectrumAnalyzerAudioProcessorEditor::paint(juce::Graphics& g)
{
	g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
	g.setOpacity(1.0f);
	g.setColour(juce::Colours::white);

	const int scopeSize = audioProcessor.getScopeSize();
	const int plotSize = audioProcessor.getPlotSize();
	const int plotIndex = audioProcessor.getPlotIndex();
	const float* scopeData = audioProcessor.getScopeData();
	const float* fft = audioProcessor.getFFT();

	juce::Path plot1;
	juce::Path plot2;
	juce::Path xAxis;
	juce::Path xAxisMarkers;
	juce::Path yAxis;
	juce::Path yAxisMarkersUp;
	juce::Path yAxisMarkersDown;
	juce::Path zeroTick;


	int sampleSize = 100;  // Adjust the number of samples being displayed as needed



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
	plot2.startNewSubPath(xStartXYAxis + xShift, yStartPlot + *(scopeData + 1 * scopeSize) * scaleY + yShift);
	plot1.startNewSubPath(xStartXYAxis + xShift, yStartPlot + scopeData[0] * scaleY + yShift);  // Xmin needs to be the new startXPlot; this will be reset by the bounds read in to xMin textEntry box
	for (int i = 1; i < sampleSize; i++)
	{
		if (isVisiblePlot2 == true) {
			plot2.lineTo(i * scaleX + xStartXYAxis + xShift, *((scopeData + i) + 1 * scopeSize) * scaleY + plotYShift);
		}
		if (isVisiblePlot1 == true) {
			plot1.lineTo(i * scaleX + xStartXYAxis + xShift, *((scopeData + i) + 0 * scopeSize) * scaleY + plotYShift);
		}
	}
	
	g.setColour(juce::Colours::lightgreen);
	g.strokePath(plot2, juce::PathStrokeType(3.0f));
	g.setColour(juce::Colours::cornflowerblue);
	g.strokePath(plot1, juce::PathStrokeType(3.0f));


	// Axis variables
	int numXMarkers = xDiff;
	int numYMarkers = yDiff;

	// Plot X Axis Markers
	for (int i = 1; i <= numXMarkers; i++) {
		xAxisMarkers.startNewSubPath(xStartXYAxis + (i * scaleX), yStartXYAxis - 5);
		xAxisMarkers.lineTo(xStartXYAxis + (i * scaleX), yStartXYAxis + 5);
	}
	g.setColour(juce::Colours::white);
	g.strokePath(xAxisMarkers, juce::PathStrokeType(2.0f));

	// Plot Y Axis Markers
	for (int i = 1; i <= numYMarkers; i++) {
		yAxisMarkersUp.startNewSubPath(xStartXYAxis - 5, yStartPlot + (scaleY * i) + yShift);
		yAxisMarkersUp.lineTo(xStartXYAxis + 5, yStartPlot + (scaleY * i) + yShift);  // drawing line markers moving up from midpoint
		yAxisMarkersDown.startNewSubPath(xStartXYAxis - 5, yStartPlot - (scaleY * i) + yShift);
		yAxisMarkersDown.lineTo(xStartXYAxis + 5, yStartPlot - (scaleY * i) + yShift);  // drawing line markers moving up from midpoint
	}
	g.setColour(juce::Colours::white);
	g.strokePath(yAxisMarkersUp, juce::PathStrokeType(2.0f));
	g.strokePath(yAxisMarkersDown, juce::PathStrokeType(2.0f));


	//Plot zero on Y-axis
	zeroTick.startNewSubPath(xStartXYAxis - 15, yStartPlot + yShift);
	zeroTick.lineTo(xStartXYAxis + 15, yStartPlot + yShift);
	g.strokePath(zeroTick, juce::PathStrokeType(3.0f));
	
	// Draw background boxes
	//box 1
	g.setColour(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
	g.fillRect(0, 0, 100, 950);
	//box 2
	g.setColour(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
	g.fillRect(xBuffer + lengthXAxis, 0, 600, 950);
	//box 3
	g.setColour(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
	g.fillRect(0, 0, 1200, 50);
	//box 4
	g.setColour(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
	g.fillRect(0, yBuffer + lengthYAxis, 1200, 400);

	// Plot x-axis
	xAxis.startNewSubPath(xStartXYAxis, yStartXYAxis);
	xAxis.lineTo(xStartXYAxis + lengthXAxis, yStartXYAxis);
	g.setColour(juce::Colours::white);
	g.strokePath(xAxis, juce::PathStrokeType(2.0f));

	// Plot y-axis
	yAxis.startNewSubPath(xStartXYAxis, yStartXYAxis);
	yAxis.lineTo(xStartXYAxis, yStartXYAxis - lengthYAxis);
	g.setColour(juce::Colours::white);
	g.strokePath(yAxis, juce::PathStrokeType(2.0f));

}

void FFTSpectrumAnalyzerAudioProcessorEditor::timerCallback()
{
	if (!isRunning && audioProcessor.getProcBlockCalled()) {
		isRunning = true;
		audioProcessor.resetProcBlockCalled();
	}
	else if (isRunning && !audioProcessor.getProcBlockCalled()) {
		isRunning = false;
		repaint();
		audioProcessor.resetScopeDataIndex();
	}
}

void FFTSpectrumAnalyzerAudioProcessorEditor::resized()
{
	// This is generally where you'll want to lay out the positions of any
	// subcomponents in your editor..
	int widgetOffsetVertical = 10;
	int widgetOffsetHorizontal = 10;

	int widthLabel = 50;
	int widthPlotLabel = 50;
	int widthToggleButton = 30;
	int widthButton = 90;

	int leftMarginXmin = xStartXYAxis;
	int leftMarginXmax = lengthXAxis + widthLabel;
	int leftMarginYmin = xBuffer;
	int leftMarginYmax = xBuffer;
	int topMarginXMinMax = lengthYAxis + yBuffer * 2;
	int topMarginYmin = lengthYAxis + yBuffer;
	int topMarginYmax = yBuffer;

	int heightControlWidget = 24;
	int heightPlotLabel = heightControlWidget;
	int heightToggleButton = heightControlWidget;
	int heightButton = heightControlWidget;

	int leftMarginToggleButton = xBuffer * 4 + lengthXAxis;
	int leftMarginPlotLabel = leftMarginToggleButton + widthToggleButton + widgetOffsetHorizontal;
	int leftMarginButton = leftMarginPlotLabel + widthPlotLabel + widgetOffsetHorizontal;
	
	int topMarginControlWidget = yBuffer * 2;
	int topMarginToggleButton1 = topMarginControlWidget;
	int topMarginToggleButton2 = topMarginControlWidget + heightToggleButton + widgetOffsetVertical;
	int topMarginPlot1Label = topMarginControlWidget;
	int topMarginPlot2Label = topMarginControlWidget + heightPlotLabel + widgetOffsetVertical;
	int topMarginButton1 = topMarginControlWidget;
	int topMarginButton2 = topMarginControlWidget + heightButton + widgetOffsetVertical;

	inputXmin.setBounds(leftMarginXmin, topMarginXMinMax, widthLabel, heightControlWidget);
	inputXmax.setBounds(leftMarginXmax, topMarginXMinMax, widthLabel, heightControlWidget);
	inputYmin.setBounds(leftMarginYmin, topMarginYmin, widthLabel, heightControlWidget);
	inputYmax.setBounds(leftMarginYmax, topMarginYmax, widthLabel, heightControlWidget);
	labelPlot1.setBounds(leftMarginPlotLabel, topMarginPlot1Label, widthPlotLabel, heightPlotLabel);
	labelPlot2.setBounds(leftMarginPlotLabel, topMarginPlot2Label, widthPlotLabel, heightPlotLabel);
	toggleButtonPlot1.setBounds(leftMarginToggleButton, topMarginToggleButton1, widthToggleButton, heightToggleButton);
	toggleButtonPlot2.setBounds(leftMarginToggleButton, topMarginToggleButton2, widthToggleButton, heightToggleButton);
	buttonPlot1.setBounds(leftMarginButton, topMarginButton1, widthButton, heightButton);
	buttonPlot2.setBounds(leftMarginButton, topMarginButton2, widthButton, heightButton);
}


void FFTSpectrumAnalyzerAudioProcessorEditor::getBounds()
{
	int minVal = -1000;
	int maxVal = 1000;
	juce::String temp = inputXmin.getText(false);
	int val = std::atoi(temp.toStdString().c_str());
	if (val >= minVal && val <= maxVal)
	{
		xMin = val;
	}
	else{inputXmin.setText(std::to_string(xMin), juce::dontSendNotification);}

	temp = inputXmax.getText(false);
	val = std::atoi(temp.toStdString().c_str());
	if (val >= minVal && val <= maxVal)
	{
		xMax = val;
	}
	else{inputXmax.setText(std::to_string(xMax), juce::dontSendNotification);}

	temp = inputYmin.getText(false);
	val = std::atoi(temp.toStdString().c_str());
	if (val >= minVal && val <= maxVal)
	{
		yMin = val;
	}
	else{inputYmin.setText(std::to_string(yMin), juce::dontSendNotification);}

	temp = inputYmax.getText(false);
	val = std::atoi(temp.toStdString().c_str());
	if (val >= minVal && val <= maxVal)
	{
		yMax = val;
	}
	else{inputYmax.setText(std::to_string(yMax), juce::dontSendNotification);}
	repaint();
}


void FFTSpectrumAnalyzerAudioProcessorEditor::setPlotIndex(int plotIndex)
{
	plotIndexSelection = plotIndex;
	audioProcessor.setPlotIndex(plotIndexSelection);
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

