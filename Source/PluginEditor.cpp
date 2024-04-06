/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <string>

int FFTSpectrumAnalyzerAudioProcessorEditor::cursorX1 = 0; //mouse
float FFTSpectrumAnalyzerAudioProcessorEditor::cursorY1 = 0.00; //mouse
int FFTSpectrumAnalyzerAudioProcessorEditor::cursorX2 = 0; //mouse
float FFTSpectrumAnalyzerAudioProcessorEditor::cursorY2 = 0.00; //mouse
bool FFTSpectrumAnalyzerAudioProcessorEditor::isRunning = false;
bool FFTSpectrumAnalyzerAudioProcessorEditor::isVisiblePlot1 = true;
bool FFTSpectrumAnalyzerAudioProcessorEditor::isVisiblePlot2 = true;
int FFTSpectrumAnalyzerAudioProcessorEditor::xMinPrev = 0;
int FFTSpectrumAnalyzerAudioProcessorEditor::xMin = 0;
int FFTSpectrumAnalyzerAudioProcessorEditor::xMaxPrev = 100;
int FFTSpectrumAnalyzerAudioProcessorEditor::xMax = 100;
int FFTSpectrumAnalyzerAudioProcessorEditor::yMinPrev = -1;
int FFTSpectrumAnalyzerAudioProcessorEditor::yMin = -1;
int FFTSpectrumAnalyzerAudioProcessorEditor::yMaxPrev = 1;
int FFTSpectrumAnalyzerAudioProcessorEditor::yMax = 1;
int FFTSpectrumAnalyzerAudioProcessorEditor::plotIndexSelection = 0;

//ROW INDEX STUFF!!!
int FFTSpectrumAnalyzerAudioProcessorEditor::rowSize=2;
int FFTSpectrumAnalyzerAudioProcessorEditor::rowIndex=0;

//Processor statics
int FFTSpectrumAnalyzerAudioProcessorEditor::fftSize = 1024;
int FFTSpectrumAnalyzerAudioProcessorEditor::fftS = 0;
int FFTSpectrumAnalyzerAudioProcessorEditor::numBins = 0;
int FFTSpectrumAnalyzerAudioProcessorEditor::maxFreq = 0;
int FFTSpectrumAnalyzerAudioProcessorEditor::numFreqBins = 0;

//Processor vectors
std::vector<float> FFTSpectrumAnalyzerAudioProcessorEditor::indexToFreqMap = { 0 };
std::vector< std::vector<float>> FFTSpectrumAnalyzerAudioProcessorEditor::binMag;

//==============================================================================
FFTSpectrumAnalyzerAudioProcessorEditor::FFTSpectrumAnalyzerAudioProcessorEditor(FFTSpectrumAnalyzerAudioProcessor& p)
	: AudioProcessorEditor(&p), audioProcessor(p)
{
	setOpaque(true);
	startTimer(500);
	setSize(1300, 1000);
	setResizable(true, true);

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

	addAndMakeVisible(cursorPlot1); //mouse
	addAndMakeVisible(cursorPlot2); //mouse
	addAndMakeVisible(cursorLabel0); //mouse
	addAndMakeVisible(cursorLabel1); //mouse
	addAndMakeVisible(cursorLabel2); //mouse
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
	cursorPlot1.setEditable(false); //mouse
	cursorPlot2.setEditable(false); //mouse
	cursorLabel0.setEditable(false); //mouse
	cursorLabel1.setEditable(false); //mouse
	cursorLabel2.setEditable(false); //mouse
	inputXmin.setText(std::to_string(xMin), juce::dontSendNotification);
	inputXmax.setText(std::to_string(xMax), juce::dontSendNotification);
	inputYmin.setText(std::to_string(yMin), juce::dontSendNotification);
	inputYmax.setText(std::to_string(yMax), juce::dontSendNotification);
	labelPlot1.setText("Plot 1", juce::dontSendNotification);
	labelPlot2.setText("Plot 2", juce::dontSendNotification);
	cursorPlot1.setText("(" + floatToStringPrecision(cursorX1, 1) + ", " + floatToStringPrecision(cursorY1, 2) + ")", juce::dontSendNotification);
	cursorPlot2.setText("(" + floatToStringPrecision(cursorX2, 1) + ", " + floatToStringPrecision(cursorY2, 2) + ")", juce::dontSendNotification);
	cursorLabel0.setText("Cursor:", juce::dontSendNotification);
	cursorLabel1.setText("Plot 1", juce::dontSendNotification);
	cursorLabel2.setText("Plot 2", juce::dontSendNotification);
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

	//const int scopeSize = audioProcessor.getScopeSize();
	//const int plotSize = audioProcessor.getPlotSize();
	//const int plotIndex = audioProcessor.getPlotIndex();
	//const float* scopeData = audioProcessor.getScopeData();
	//const float* fft = audioProcessor.getFFT();


	//PROCESSOR CLASS CODE!!!!!!!!!
	fftS = 1024;
	rowIndex = 0;
	rowSize = 2;

	int sampleRate = audioProcessor.getBlockSampleRate();
	setFreqData(fftS, sampleRate);
	audioProcessor.setFFTSize(fftS);
	audioProcessor.zeroAllSelections(numBins, rowSize);      //this is used for the hardcoded 2 selections
	audioProcessor.prepSelection(numBins, rowSize,rowIndex); 


	//std::string rate = std::to_string(sampleRate);
	setFreqData(fftS, sampleRate);

	juce::dsp::WindowingFunction<float>::WindowingMethod windowType = juce::dsp::WindowingFunction<float>::WindowingMethod::hann;
	audioProcessor.setWindow(windowType);

	//x variable for labeling
	for (int i = 0; i < numBins; i++) {
		indexToFreqMap[i] = i * ((float)maxFreq / (float)numFreqBins);
	}

	int fftCounter = audioProcessor.getFFTCounter();
	//std::string counter = std::to_string(fftCounter);

	binMag = audioProcessor.getBinMag();


	if (fftCounter != 0) {
		for (int i = 0; i < numBins; i++) {
			binMag[rowIndex][i] /= fftCounter;
		}
	}
	
	juce::Path plot1;
	juce::Path plot2;
	juce::Path xAxis;
	juce::Path xAxisMarkers;
	juce::Path yAxis;
	juce::Path yAxisMarkersUp;
	juce::Path yAxisMarkersDown;
	juce::Path zeroTick;

	// Paint values for plotting
	float xBuffer = getWidth() * 0.10;
	float yBuffer = getHeight() * 0.05;
	float lengthXAxis = getWidth() * 0.80;
	float lengthYAxis = getHeight() * 0.80;
	float yStartXYAxis = yBuffer + lengthYAxis;
	float xStartXYAxis = xBuffer;
	float yStartPlot = yBuffer + lengthYAxis / 2;

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

	plot2.startNewSubPath(xStartXYAxis + xShift, yStartPlot + (binMag[1][0])* scaleY + yShift);
	//plot2.startNewSubPath(xStartXYAxis + xShift, yStartPlot + *(scopeData + 1 * scopeSize) * scaleY + yShift);
	plot1.startNewSubPath(xStartXYAxis + xShift, yStartPlot + binMag[0][0] * scaleY + yShift);  // Xmin needs to be the new startXPlot; this will be reset by the bounds read in to xMin textEntry box
	for (int i = 1; i <= sampleSize; i++)
	{
		if (isVisiblePlot2 == true) {
			plot2.lineTo(i * scaleX + xStartXYAxis + xShift, (binMag[1][0]) * scaleY + plotYShift);
			//plot2.lineTo(i * scaleX + xStartXYAxis + xShift, *((scopeData + i) + 1 * scopeSize) * scaleY + plotYShift);
		}
		if (isVisiblePlot1 == true) {
			plot1.lineTo(i * scaleX + xStartXYAxis + xShift, (binMag[0][0] + 0 * numBins) * scaleY + plotYShift);
			//plot1.lineTo(i * scaleX + xStartXYAxis + xShift, *((scopeData + i) + 0 * scopeSize) * scaleY + plotYShift);
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
	int leftMarginLeftPanel = origin;
	int leftMarginRightPanel = xBuffer + lengthXAxis;
	int leftMarginTopPanel = origin;
	int leftMarginBottomPanel = origin;
	int topMarginLeftPanel = origin;
	int topMarginRightPanel = origin;
	int topMarginTopPanel = origin;
	int topMarginBottomPanel = yBuffer + lengthYAxis - 2;
	int widthLeftPanel = xBuffer;
	int widthRightPanel = getWidth() * 0.25;
	int widthTopPanel = getWidth();
	int widthBottomPanel = getWidth();
	int heightLeftPanel = getHeight();
	int heightRightPanel = getHeight();
	int heightTopPanel = yBuffer;
	int heightBottomPanel = getHeight() * 0.25;

	juce::Rectangle<int> leftPanel(leftMarginLeftPanel, topMarginLeftPanel, widthLeftPanel, heightLeftPanel);
	juce::Rectangle<int> rightPanel(leftMarginRightPanel, topMarginRightPanel, widthRightPanel, heightRightPanel);
	juce::Rectangle<int> topPanel(leftMarginTopPanel, topMarginTopPanel, widthTopPanel, heightTopPanel);
	juce::Rectangle<int> bottomPanel(leftMarginBottomPanel, topMarginBottomPanel, widthBottomPanel, heightBottomPanel);
	g.setColour(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
	g.fillRect(leftPanel);
	g.fillRect(rightPanel);
	g.fillRect(topPanel);
	g.fillRect(bottomPanel);

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
		//audioProcessor.resetScopeDataIndex();
	}
}

void FFTSpectrumAnalyzerAudioProcessorEditor::resized()
{
	// Paint values for plotting
	float xBuffer = getWidth() * 0.10;
	float yBuffer = getHeight() * 0.05;
	float lengthXAxis = getWidth() * 0.80;  //pixels = unit
	float lengthYAxis = getHeight() * 0.80;  //pixels = unit
	float yStartXYAxis = yBuffer + lengthYAxis;
	float xStartXYAxis = xBuffer;
	float yStartPlot = yBuffer + lengthYAxis / 2;

	int leftMarginXmin = xStartXYAxis;
	int leftMarginXmax = lengthXAxis + xBuffer - widthLabel;
	int leftMarginYmin = xBuffer - (widthLabel + widgetOffsetHorizontal);
	int leftMarginYmax = xBuffer - (widthLabel + widgetOffsetHorizontal);
	int topMarginXMinMax = lengthYAxis + (yBuffer + widgetOffsetVertical);
	int topMarginYmin = lengthYAxis + yBuffer - widgetHeight;
	int topMarginYmax = yBuffer;
	
	int heightControlWidget = widgetHeight;
	int heightCursorLabel = widgetHeight;
	int heightPlotLabel = heightControlWidget;
	int heightToggleButton = heightControlWidget;
	int heightButton = heightControlWidget;

	int leftMarginCursorLabel0 = lengthXAxis / 2;
	int leftMarginCursorLabel1 = lengthXAxis / 2;
	int leftMarginCursorLabel2 = leftMarginCursorLabel0;
	int leftMarginCursorPlot1 = leftMarginCursorLabel1 + widthPlotLabel + widgetOffsetHorizontal;
	int leftMarginCursorPlot2 = leftMarginCursorLabel2 + widthPlotLabel + widgetOffsetHorizontal;
	int leftMarginToggleButton = leftMarginCursorLabel0 + xBuffer +  widthToggleButton + widgetOffsetHorizontal;
	int leftMarginPlotLabel = leftMarginToggleButton + widthToggleButton + widgetOffsetHorizontal;
	int leftMarginButton = leftMarginPlotLabel + widthPlotLabel + widgetOffsetHorizontal;

	int topMarginCursorLabel0 = yBuffer * 2 + lengthYAxis;
	int topMarginCursorPlot1 = topMarginCursorLabel0 + heightCursorLabel + widgetOffsetVertical;
	int topMarginCursorPlot2 = topMarginCursorPlot1 + heightCursorLabel + widgetOffsetVertical;
	int topMarginCursorLabel1 = topMarginCursorLabel0 + heightCursorLabel + widgetOffsetVertical;
	int topMarginCursorLabel2 = topMarginCursorLabel1 + heightCursorLabel + widgetOffsetVertical;
	int topMarginControlWidget = topMarginCursorPlot1;
	int topMarginToggleButton1 = topMarginCursorPlot1;
	int topMarginToggleButton2 = topMarginControlWidget + heightToggleButton + widgetOffsetVertical;
	int topMarginPlot1Label = topMarginControlWidget;
	int topMarginPlot2Label = topMarginControlWidget + heightPlotLabel + widgetOffsetVertical;
	int topMarginButton1 = topMarginControlWidget;
	int topMarginButton2 = topMarginControlWidget + heightButton + widgetOffsetVertical;

	cursorPlot1.setBounds(leftMarginCursorPlot1, topMarginCursorPlot1, widthButton, widgetHeight); //mouse
	cursorPlot2.setBounds(leftMarginCursorPlot2, topMarginCursorPlot2, widthButton, widgetHeight); //mouse
	cursorLabel0.setBounds(leftMarginCursorLabel0, topMarginCursorLabel0, widthButton, widgetHeight); //mouse
	cursorLabel1.setBounds(leftMarginCursorLabel1, topMarginCursorLabel1, widthButton, widgetHeight); //mouse
	cursorLabel2.setBounds(leftMarginCursorLabel2, topMarginCursorLabel2, widthButton, widgetHeight); //mouse

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

void FFTSpectrumAnalyzerAudioProcessorEditor::setFreqData(int fftData,int sampleRate) {
	fftS = fftData;
	numBins = fftS / 2 + 1;
	maxFreq = sampleRate / 2;
	numFreqBins = fftS / 2;
	indexToFreqMap.resize(numBins);
	binMag.resize(1, std::vector<float>(numBins, 0));
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
	//audioProcessor.setPlotIndex(plotIndexSelection);
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

void FFTSpectrumAnalyzerAudioProcessorEditor::mouseMove(const juce::MouseEvent& event)
{
	float xBuffer = getWidth() * 0.10; 
	float yBuffer = getWidth() * 0.10; 
	float xStartXYAxis = xBuffer;
	float lengthXAxis = getWidth() * 0.80;
	float lengthYAxis = getWidth() * 0.80;

	cursorX1 = event.getMouseDownX();
	cursorY1 = event.getMouseDownY();
	//invalid bounds
	if (cursorX1 < xBuffer || cursorX1 > (xBuffer + lengthXAxis) || cursorY1 < yBuffer || cursorY1 > (yBuffer + lengthYAxis)) {
		cursorX1 = cursorX2 = 0.0;
		cursorY1 = cursorY2 = 0.00;
		cursorPlot1.setText("(" + floatToStringPrecision(cursorX1, 1) + ", " + floatToStringPrecision(cursorY1, 2) + ")", juce::dontSendNotification); //mouse
		cursorPlot2.setText("(" + floatToStringPrecision(cursorX2, 1) + ", " + floatToStringPrecision(cursorY2, 2) + ")", juce::dontSendNotification); //mouse
	}
	else {
		//offset xCoord [xCoord / (ratio of x-axis length to bounds)]
		int xScale = lengthXAxis / (xMax - xMin);

		cursorX1 += (xMin * xScale);
		cursorX1 -= xStartXYAxis;
		cursorX1 /= xScale;
		cursorX2 = cursorX1;

		//plot 1
		if (rowIndex == 0 && isVisiblePlot1) {
			const std::vector<std::vector<float>> scopeData = audioProcessor.getBinMag();
			cursorPlot1.setText("(" + floatToStringPrecision(cursorX1, 1) + ", " + floatToStringPrecision(scopeData[rowIndex][cursorX1], 2) + ")", juce::dontSendNotification);
		}
		//plot 2
		else {
			const std::vector<std::vector<float>> scopeData2 = audioProcessor.getBinMag();
			//const float* scopeData2 = audioProcessor.getScopeData() + audioProcessor.getScopeSize();
			cursorPlot2.setText("(" + floatToStringPrecision(cursorX2, 1) + ", " + floatToStringPrecision(scopeData2[rowIndex][cursorX2], 2) + ")", juce::dontSendNotification);
		}
	}
	repaint();
}

std::string FFTSpectrumAnalyzerAudioProcessorEditor::floatToStringPrecision(float f, int p)
{
	std::ostringstream oss;
	oss << std::fixed << std::setprecision(p) << f;
	return oss.str();
}


