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

//==============================================================================
FFTSpectrumAnalyzerAudioProcessorEditor::FFTSpectrumAnalyzerAudioProcessorEditor(FFTSpectrumAnalyzerAudioProcessor& p)
	: AudioProcessorEditor(&p), audioProcessor(p)
{
	setOpaque(true);
	startTimer(500);
	setSize(1300, 1000);

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

	int xStartPlot = 100;  // Offset X position
	int yStartPlot = 450;
	int sampleSize = 100;  // Adjust the number of samples being displayed as needed
	int yBottom = -1;  // Setting for when the sine wave goes from 1 to -1

	// Axis variables
	int lengthXAxis = 1000;  //pixels = unit
	int lengthYAxis = 800;  //pixels = unit
	int scaleXMarker = 50;
	int scaleYMarker = 50;
	int numXMarkers = 20;
	int numYMarkers = 15;
	int yStartXAxis = 850;
	int yStartYAxis = 850;
	int yAxisMidpoint = 450;

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

	juce::Path plot1;
	juce::Path plot2;
	juce::Path xAxis;
	juce::Path xAxisMarkers;
	juce::Path yAxis;
	juce::Path yAxisMarkersUp;
	juce::Path zeroTick;

	// Graph plots
	plot2.startNewSubPath(xStartPlot + xShift, yStartPlot + *(scopeData + 1 * scopeSize) * scaleY + yShift);
	plot1.startNewSubPath(xStartPlot + xShift, yStartPlot + scopeData[0] * scaleY + yShift);  // Xmin needs to be the new startXPlot; this will be reset by the bounds read in to xMin textEntry box
	for (int i = 1; i < sampleSize; i++)
	{
		if (isVisiblePlot2 == true) {
			plot2.lineTo(i * scaleX + xStartPlot + xShift, *((scopeData + i) + 1 * scopeSize) * scaleY + yStartPlot + yShift);
		}
		if (isVisiblePlot1 == true) {
			plot1.lineTo(i * scaleX + xStartPlot + xShift, *((scopeData + i) + 0 * scopeSize) * scaleY + yStartPlot + yShift);
		}
	}
	
	g.setColour(juce::Colours::lightgreen);
	g.strokePath(plot2, juce::PathStrokeType(5.0f));
	g.setColour(juce::Colours::cornflowerblue);
	g.strokePath(plot1, juce::PathStrokeType(5.0f));
	
	// Draw boxes
	//box 1
	g.setColour(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
	g.fillRect(0, 0, 100, 950);
	//box 2
	g.setColour(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
	g.fillRect(1100, 0, 600, 950);
	//box 3
	g.setColour(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
	g.fillRect(0, 0, 1200, 50);
	//box 4
	g.setColour(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
	g.fillRect(0, 850, 1200, 400);


	// Plot x-axis
	xAxis.startNewSubPath(xStartPlot, yStartXAxis);
	xAxis.lineTo(xStartPlot + lengthXAxis, yStartXAxis);
	g.setColour(juce::Colours::white);
	g.strokePath(xAxis, juce::PathStrokeType(2.0f));

	// Plot y-axis
	yAxis.startNewSubPath(xStartPlot, yStartYAxis);
	yAxis.lineTo(xStartPlot, yStartYAxis - lengthYAxis);
	g.setColour(juce::Colours::white);
	g.strokePath(yAxis, juce::PathStrokeType(2.0f));

	//Plot zero on Y-axis
	zeroTick.startNewSubPath(xStartPlot - 15, yAxisMidpoint);
	zeroTick.lineTo(xStartPlot + 15, yAxisMidpoint);
	g.strokePath(zeroTick, juce::PathStrokeType(4.0f));

	// Plot X Axis Markers
	for (int i = 1; i < numXMarkers; i++) {
		xAxisMarkers.startNewSubPath(xStartPlot + (i * scaleXMarker), yStartXAxis - 5);
		xAxisMarkers.lineTo(xStartPlot + (i * scaleXMarker), yStartXAxis + 5);
	}
	g.setColour(juce::Colours::white);
	g.strokePath(xAxisMarkers, juce::PathStrokeType(2.0f));

	// Plot Y Axis Markers
	for (int i = 1; i <= numYMarkers; i++) {
		yAxisMarkersUp.startNewSubPath(xStartPlot - 5, yStartYAxis - (scaleYMarker * i));
		yAxisMarkersUp.lineTo(xStartPlot + 5, yStartYAxis - (scaleYMarker * i));  // drawing line markers moving up from midpoint
	}
	g.setColour(juce::Colours::white);
	g.strokePath(yAxisMarkersUp, juce::PathStrokeType(2.0f));
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

	int leftMarginXmin = 100;
	int leftMarginXmax = 1045;
	int leftMarginYmin = 20;
	int leftMarginYmax = 20;
	int topMarginXmin = 870;
	int topMarginXmax = 870;
	int topMarginYmin = 820;
	int topMarginYmax = 45;

	int widgetOffsetVertical = 10;
	int widgetOffsetHorizontal = 10;

	int widthLabel = 60;
	int widthPlotLabel = 50;
	int widthToggleButton = 30;
	int widthButton = 90;

	int heightControlWidget = 24;
	int heightPlotLabel = heightControlWidget;
	int heightToggleButton = heightControlWidget;
	int heightButton = heightControlWidget;

	int leftMarginToggleButton = 1100;
	int leftMarginPlotLabel = leftMarginToggleButton + widthToggleButton + widgetOffsetHorizontal;
	int leftMarginButton = leftMarginPlotLabel + widthPlotLabel + widgetOffsetHorizontal;
	
	int topMarginControlWidget = 100;
	int topMarginToggleButton1 = topMarginControlWidget;
	int topMarginToggleButton2 = topMarginControlWidget + heightToggleButton + widgetOffsetVertical;
	int topMarginPlot1Label = topMarginControlWidget;
	int topMarginPlot2Label = topMarginControlWidget + heightPlotLabel + widgetOffsetVertical;
	int topMarginButton1 = topMarginControlWidget;
	int topMarginButton2 = topMarginControlWidget + heightButton + widgetOffsetVertical;

	inputXmin.setBounds(leftMarginXmin, topMarginXmin, widthLabel, heightControlWidget);
	inputXmax.setBounds(leftMarginXmax, topMarginXmax, widthLabel, heightControlWidget);
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
	else
	{
		inputXmin.setText(std::to_string(xMin), juce::dontSendNotification);
	}

	temp = inputXmax.getText(false);
	val = std::atoi(temp.toStdString().c_str());
	if (val >= minVal && val <= maxVal)
	{
		xMax = val;
	}
	else
	{
		inputXmax.setText(std::to_string(xMax), juce::dontSendNotification);
	}

	temp = inputYmin.getText(false);
	val = std::atoi(temp.toStdString().c_str());
	if (val >= minVal && val <= maxVal)
	{
		yMin = val;
	}
	else
	{
		inputYmin.setText(std::to_string(yMin), juce::dontSendNotification);
	}

	temp = inputYmax.getText(false);
	val = std::atoi(temp.toStdString().c_str());
	if (val >= minVal && val <= maxVal)
	{
		yMax = val;
	}
	else
	{
		inputYmax.setText(std::to_string(yMax), juce::dontSendNotification);
	}
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

