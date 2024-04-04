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

int FFTSpectrumAnalyzerAudioProcessorEditor::windowWidth = 950;
int FFTSpectrumAnalyzerAudioProcessorEditor::windowHeight = 550 + 2;
int FFTSpectrumAnalyzerAudioProcessorEditor::windowMaxWidth = 2160; 
int FFTSpectrumAnalyzerAudioProcessorEditor::windowMaxHeight = 1080;

// height and width for primary category labels (Import Audio, Zoom, Export, etc.) 
const int width_primaryCategoryLabel = 275; 
const int height_primaryCategoryLabel = 25;  
// height and width for secondary labels ("Selected Traces", Upper/Lower, etc.)
const int width_secondaryLabel = 150;
const int height_secondaryLabel = 25;
// space between primary labels and secondary labels
const int yOffsetPrimary_secondaryLabel = 8;
// space between secondary components (e.g. white box for plot selection) and physical boundaries
const int x_componentOffset = 6;
const int y_componentOffset = 6;
// dimensions of white box for plot selection
const int yOffset_selectionBox = 2;
const int width_selectionBox = 263;
const int height_selectionBox = 90;

//==============================================================================
FFTSpectrumAnalyzerAudioProcessorEditor::FFTSpectrumAnalyzerAudioProcessorEditor(FFTSpectrumAnalyzerAudioProcessor& p)
	: AudioProcessorEditor(&p), audioProcessor(p)
{
	setOpaque(true);
	setSize(1600, 1000);
	startTimer(500);
	setSize(windowWidth, windowHeight);
	setResizable(true, true);
	setResizeLimits(windowWidth, windowHeight, windowMaxWidth, windowMaxHeight);

	setPlotIndex(plotIndexSelection);

	// new gui elements start
	addAndMakeVisible(gui_importAudio);
	gui_importAudio.setFont(juce::Font("Arial", 18.0f, juce::Font::bold));
	gui_importAudio.setText("Import Audio", juce::dontSendNotification);
	gui_importAudio.setColour(juce::Label::backgroundColourId, juce::Colours::darkgrey);

	addAndMakeVisible(gui_selectTrace);
	gui_selectTrace.setText("Selected Traces", juce::dontSendNotification);
	gui_selectTrace.setFont(juce::Font(17.0f));
	
	addAndMakeVisible(gui_zoom);
	gui_zoom.setFont(juce::Font("Arial", 18.0f, juce::Font::bold));
	gui_zoom.setText("Zoom", juce::dontSendNotification);
	gui_zoom.setColour(juce::Label::backgroundColourId, juce::Colours::darkgrey);

	addAndMakeVisible(gui_upper);
	gui_upper.setText("Upper", juce::dontSendNotification);
	gui_upper.setFont(juce::Font(17.0f));

	addAndMakeVisible(gui_lower);
	gui_lower.setText("Lower", juce::dontSendNotification);
	gui_lower.setFont(juce::Font(17.0f));

	addAndMakeVisible(gui_xUpper);
	gui_xUpper.setText("X", juce::dontSendNotification);
	gui_xUpper.setFont(juce::Font(17.0f));

	addAndMakeVisible(gui_yUpper);
	gui_yUpper.setText("Y", juce::dontSendNotification);
	gui_yUpper.setFont(juce::Font(17.0f));

	addAndMakeVisible(gui_xLower);
	gui_xLower.setText("X", juce::dontSendNotification);
	gui_xLower.setFont(juce::Font(17.0f));

	addAndMakeVisible(gui_yLower);
	gui_yLower.setText("Y", juce::dontSendNotification);
	gui_yLower.setFont(juce::Font(17.0f));

	addAndMakeVisible(gui_export);
	gui_export.setText("Export", juce::dontSendNotification);
	gui_export.setFont(juce::Font("Arial", 18.0f, juce::Font::bold));
	gui_export.setColour(juce::Label::backgroundColourId, juce::Colours::darkgrey);

	addAndMakeVisible(gui_exportButton);
	gui_exportButton.setColour(juce::TextButton::buttonColourId, juce::Colours::white);
	gui_exportButton.setColour(juce::TextButton::textColourOnId, juce::Colours::black);
	gui_exportButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);

	addAndMakeVisible(cursorLabel); 
	cursorLabel.setText("Cursor", juce::dontSendNotification);
	cursorLabel.setFont(juce::Font("Arial", 14.0f, juce::Font::bold));
	cursorLabel.setColour(juce::Label::textColourId, juce::Colours::white);

	addAndMakeVisible(peakLabel);
	peakLabel.setText("Peak", juce::dontSendNotification);
	peakLabel.setFont(juce::Font("Arial", 14.0f, juce::Font::bold));
	peakLabel.setColour(juce::Label::textColourId, juce::Colours::white);

	addAndMakeVisible(windowLabel);
	windowLabel.setText("Function", juce::dontSendNotification);
	windowLabel.setFont(juce::Font("Arial", 14.0f, juce::Font::bold));
	windowLabel.setColour(juce::Label::textColourId, juce::Colours::white);

	addAndMakeVisible(axisLabel);
	axisLabel.setText("Axis", juce::dontSendNotification);
	axisLabel.setFont(juce::Font("Arial", 14.0f, juce::Font::bold));
	axisLabel.setColour(juce::Label::textColourId, juce::Colours::white);

	addAndMakeVisible(sizeLabel);
	sizeLabel.setText("Size", juce::dontSendNotification);
	sizeLabel.setFont(juce::Font("Arial", 14.0f, juce::Font::bold));
	sizeLabel.setColour(juce::Label::textColourId, juce::Colours::white);

	addAndMakeVisible(windowFunction);
	windowFunction.addItem("Hann window", 1);
	windowFunction.setSelectedId(1);
	windowFunction.setColour(juce::ComboBox::backgroundColourId, juce::Colours::white); 
	windowFunction.setColour(juce::ComboBox::textColourId, juce::Colours::black);
	windowFunction.setColour(juce::ComboBox::arrowColourId, juce::Colours::darkgrey); 

	addAndMakeVisible(axis);
	axis.addItem("Log Frequency", 1);
	axis.setSelectedId(1);
	axis.setColour(juce::ComboBox::backgroundColourId, juce::Colours::white);
	axis.setColour(juce::ComboBox::textColourId, juce::Colours::black);
	axis.setColour(juce::ComboBox::arrowColourId, juce::Colours::darkgrey);

	addAndMakeVisible(size);
	size.addItem("1024", 1);
	size.setSelectedId(1);
	size.setColour(juce::ComboBox::backgroundColourId, juce::Colours::white);
	size.setColour(juce::ComboBox::textColourId, juce::Colours::black);
	size.setColour(juce::ComboBox::arrowColourId, juce::Colours::darkgrey);



	// new gui elements end

	// buttons to select which of two graphs to plot
	addAndMakeVisible(buttonPlot1);
	buttonPlot1.setClickingTogglesState(true);
	buttonPlot1.onClick = [&]()
		{
			plotIndexSelection = 0;
			setPlotIndex(0);
		};
	addAndMakeVisible(buttonPlot2);
	buttonPlot2.setClickingTogglesState(true);
	buttonPlot2.onClick = [&]()
		{
			plotIndexSelection = 1;
			setPlotIndex(1);
		};

	// toggle button for plot 1
	addAndMakeVisible(toggleButtonPlot1);
	toggleButtonPlot1.setColour(juce::ToggleButton::tickColourId, juce::Colours::white);
	toggleButtonPlot1.setColour(juce::ToggleButton::tickDisabledColourId, juce::Colours::lightgrey);
	if (isVisiblePlot1 == true)
	{
		toggleButtonPlot1.setToggleState(true, true);
	}
	toggleButtonPlot1.onClick = [this] { updateToggleState(1); };
	toggleButtonPlot1.setClickingTogglesState(true);

	// toggle button for plot 2
	addAndMakeVisible(toggleButtonPlot2);
	toggleButtonPlot2.setColour(juce::ToggleButton::tickColourId, juce::Colours::white);
	toggleButtonPlot2.setColour(juce::ToggleButton::tickDisabledColourId, juce::Colours::lightgrey);
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
	cursorPlot1.setEditable(false); //mouse
	cursorPlot2.setEditable(false); //mouse
	cursorLabel0.setEditable(false); //mouse
	cursorLabel1.setEditable(false); //mouse
	cursorLabel2.setEditable(false); //mouse

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

	/*
	cursorPlot1.setText("(" + floatToStringPrecision(cursorX1, 1) + ", " + floatToStringPrecision(cursorY1, 2) + ")", juce::dontSendNotification);
	cursorPlot2.setText("(" + floatToStringPrecision(cursorX2, 1) + ", " + floatToStringPrecision(cursorY2, 2) + ")", juce::dontSendNotification);
	cursorLabel0.setText("Cursor:", juce::dontSendNotification);
	cursorLabel1.setText("Plot 1", juce::dontSendNotification);
	cursorLabel2.setText("Plot 2", juce::dontSendNotification);
	*/

	inputXmin.setText(std::to_string(xMin), juce::dontSendNotification);
	inputXmax.setText(std::to_string(xMax), juce::dontSendNotification);
	inputYmin.setText(std::to_string(yMin), juce::dontSendNotification);
	inputYmax.setText(std::to_string(yMax), juce::dontSendNotification);

	labelPlot1.setText("Plot 1", juce::dontSendNotification);
	labelPlot1.setColour(juce::Label::textColourId, juce::Colours::darkgrey);
	labelPlot2.setText("Plot 2", juce::dontSendNotification);
	labelPlot2.setColour(juce::Label::textColourId, juce::Colours::darkgrey);

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
	//g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
	g.fillAll(juce::Colours::black);
	g.setOpacity(1.0f);
	g.setColour(juce::Colours::white);
	
	
	

	const int scopeSize = audioProcessor.getScopeSize();
	const int plotSize = audioProcessor.getPlotSize();
	const int plotIndex = audioProcessor.getPlotIndex();
	const float* scopeData = audioProcessor.getScopeData();
	const float* fft = audioProcessor.getFFT();
	int fftS = 1024;
	int row = 0;
	juce::Path plot1;
	juce::Path plot2;
	juce::Path xAxis;
	juce::Path xAxisMarkers;
	juce::Path yAxis;
	juce::Path yAxisMarkersUp;
	juce::Path yAxisMarkersDown;
	juce::Path zeroTick;

	//** graph scaling variables **//
	float border_xBuffer = getWidth() * 0.295;
	float border_yBuffer = y_componentOffset;
	float widthBorder = getWidth() - x_componentOffset;
	float heightBorder = getHeight() - 240;
	float xBuffer = border_xBuffer + 2;
	float yBuffer = border_yBuffer + 12;
	float lengthXAxis = widthBorder;
	float lengthYAxis = heightBorder * .95;
	float yStartXYAxis = yBuffer + lengthYAxis - 1; 
	float xStartXYAxis = xBuffer - 3; 
	float yStartPlot = (yBuffer + lengthYAxis) / 2;

	audioProcessor.setFFTSize(fftS,row);
	int sampleRate = audioProcessor.getBlockSampleRate();
	//std::string rate = std::to_string(sampleRate);
	setFreqData(fftS, sampleRate);

	juce::dsp::WindowingFunction<float>::WindowingMethod windowType = juce::dsp::WindowingFunction<float>::WindowingMethod::hann;
	audioProcessor.setWindow(windowType);	float xDiff = xMax - xMin;
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
	//x variable for labeling
	for (int i = 0; i < numBins; i++) {
		indexToFreqMap[i] = i * ((float)maxFreq / (float)numFreqBins);
float yDiff = yMax - yMin;
	if (yDiff <= 0)  // handles divide by zero errors
	{
		yMax = yMaxPrev;
		yMin = yMinPrev;
		yDiff = yMaxPrev - yMinPrev;
		inputYmin.setText(std::to_string(yMinPrev), juce::dontSendNotification);
		inputYmax.setText(std::to_string(yMaxPrev), juce::dontSendNotification);
	}
	binMag = audioProcessor.getBinMag();
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
	for (int i = 1; i <= sampleSize; i++)
	{
		if (isVisiblePlot2 == true) {
			plot2.lineTo(i * scaleX + xStartXYAxis + xShift, *((scopeData + i) + 1 * scopeSize) * scaleY + plotYShift);
		}
		if (isVisiblePlot1 == true) {
			plot1.lineTo(i * scaleX + xStartXYAxis + xShift, *((scopeData + i) + 0 * scopeSize) * scaleY + plotYShift);
		}
	}

	bins = audioProcessor.getBins();

	int fftCounter = audioProcessor.getFFTCounter();
	//std::string counter = std::to_string(fftCounter);

	
	//g.drawText(counter, getLocalBounds(),juce::Justification::centred,true);

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
	// This is generally where you'll want to lay out the positions of any
	//** graph scaling variables **//
	float border_xBuffer = getWidth() * 0.295;
	float border_yBuffer = y_componentOffset;
	float widthBorder = getWidth() - x_componentOffset;
	float heightBorder = getHeight() - 240;
	float xBuffer = border_xBuffer + 2;
	float yBuffer = border_yBuffer + 12;
	float lengthXAxis = widthBorder;
	float lengthYAxis = heightBorder * .95;
	float yStartXYAxis = yBuffer + lengthYAxis - 1;
	float xStartXYAxis = xBuffer - 3;
	float yStartPlot = (yBuffer + lengthYAxis) / 2;

	//** margins for primary labels **//
	int yMargin_selectTraceLabel = height_primaryCategoryLabel + yOffsetPrimary_secondaryLabel;
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

	
	//** peak **//
	// label
	int xMargin_peakLabel = xMargin_cursorLabel + 205;
	int yMargin_peaklabel = yMargin_cursorLabel;

	
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
	cursorLabel.setBounds(xMargin_cursorLabel, yMargin_cursorLabel, width_secondaryLabel, height_secondaryLabel);
	peakLabel.setBounds(xMargin_peakLabel, yMargin_peaklabel, width_secondaryLabel, height_secondaryLabel);
	windowLabel.setBounds(xMargin_windowLabel, yMargin_windowLabel, width_secondaryLabel, height_secondaryLabel);
	axisLabel.setBounds(xMargin_axisLabel, yMargin_axisLabel, width_secondaryLabel, height_secondaryLabel);
	sizeLabel.setBounds(xMargin_sizeLabel, yMargin_sizeLabel, width_secondaryLabel, height_secondaryLabel);

	windowFunction.setBounds(xMargin_winCombo, yMargin_winCombo, width_comboBox, height_comboBox);
	axis.setBounds(xMargin_axisCombo, yMargin_axisCombo, width_comboBox, height_comboBox);
	size.setBounds(xMargin_sizeCombo, yMargin_sizeCombo, width_comboBox, height_comboBox);
	

	//** plot 1 **//
	// toggle button 1
	int xMargin_toggleButton1 = 2 * x_componentOffset;
	int yMargin_toggleButton1 = height_primaryCategoryLabel + yOffsetPrimary_secondaryLabel + height_secondaryLabel + (6 * yOffset_selectionBox);
	// plot label 1
	int xMargin_plotLabel1 = 4 * xMargin_toggleButton1;
	int yMargin_plotLabel1 = yMargin_toggleButton1; 
	// selection button 1
	int xMargin_selectButton1 = 3.5 * xMargin_plotLabel1;
	int yMargin_selectButton1 = yMargin_toggleButton1;

	//** plot 2 **//
	// toggle button 2
	int xMargin_toggleButton2 = xMargin_toggleButton1;
	int yMargin_toggleBotton2 = yMargin_toggleButton1 + (23 * yOffset_selectionBox);
	// plot label 2
	int xMargin_plotLabel2 = xMargin_plotLabel1;
	int yMargin_plotLabel2 = yMargin_toggleBotton2;
	// selection button 2
	int xMargin_selectButton2 = xMargin_selectButton1;
	int yMargin_selectButton2 = yMargin_toggleBotton2;

	//** upper bounds **//
	// upper label
	int yMargin_upperLabel = yMargin_zoomLabel + height_primaryCategoryLabel + yOffsetPrimary_secondaryLabel;

	// xMax input
	int xMargin_xMax = 10 * x_componentOffset;
	int yMargin_xMax = yMargin_upperLabel + 2;

	// x label
	int xMargin_xMaxLabel = 20.5 * x_componentOffset;
	int yMargin_xMaxLabel = yMargin_upperLabel;

	// yMax input
	int xMargin_yMax = 30 * x_componentOffset;
	int yMargin_yMax = yMargin_upperLabel + 2; 

	// y label
	int xMargin_yMaxLabel = 40.5 * x_componentOffset;;
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
	int xMargin_exportButton = x_componentOffset; 
	int yMargin_exportButton = yMargin_exportLabel + height_primaryCategoryLabel + (1.5 * yOffsetPrimary_secondaryLabel);

	//** set bounds for GUI elements **//
	gui_importAudio.setBounds(0, 0, width_primaryCategoryLabel, height_primaryCategoryLabel); 
	gui_selectTrace.setBounds(0, yMargin_selectTraceLabel, width_secondaryLabel, height_secondaryLabel);
	gui_zoom.setBounds(0, yMargin_zoomLabel, width_primaryCategoryLabel, height_primaryCategoryLabel);
	gui_export.setBounds(0, yMargin_exportLabel, width_primaryCategoryLabel, height_primaryCategoryLabel);

	//** set bounds for secondary GUI elements **//
	toggleButtonPlot1.setBounds(xMargin_toggleButton1, yMargin_toggleButton1, width_toggleButton, height_toggleButton);
	toggleButtonPlot2.setBounds(xMargin_toggleButton2, yMargin_toggleBotton2, width_toggleButton, height_toggleButton);

	labelPlot1.setBounds(xMargin_plotLabel1, yMargin_plotLabel1, width_plotLabel, height_plotLabel);
	labelPlot2.setBounds(xMargin_plotLabel2, yMargin_plotLabel2, width_plotLabel, height_plotLabel);

	buttonPlot1.setBounds(xMargin_selectButton1, yMargin_selectButton1, width_selectButton, height_selectButton);
	buttonPlot2.setBounds(xMargin_selectButton2, yMargin_selectButton2, width_selectButton, height_selectButton);

	gui_upper.setBounds(0, yMargin_upperLabel, width_secondaryLabel, height_secondaryLabel);
	inputXmax.setBounds(xMargin_xMax, yMargin_xMax, width_inputTextbox, height_inputTextbox);
	gui_xUpper.setBounds(xMargin_xMaxLabel, yMargin_xMaxLabel, width_secondaryLabel, height_secondaryLabel);
	inputYmax.setBounds(xMargin_yMax, yMargin_yMax, width_inputTextbox, height_inputTextbox);
	gui_yUpper.setBounds(xMargin_yMaxLabel, yMargin_yMaxLabel, width_inputTextbox, height_inputTextbox);

	gui_lower.setBounds(0, yMargin_lowerLabel, width_secondaryLabel, height_secondaryLabel);
	inputXmin.setBounds(xMargin_xMin, yMargin_xMin, width_inputTextbox, height_inputTextbox);
	gui_xLower.setBounds(xMargin_xMinLabel, yMargin_xMinLabel, width_secondaryLabel, height_secondaryLabel);
	inputYmin.setBounds(xMargin_yMax, yMargin_yMin, width_inputTextbox, height_inputTextbox);
	gui_yLower.setBounds(xMargin_yMinLabel, yMargin_yMinLabel, width_secondaryLabel, height_secondaryLabel);

	gui_exportButton.setBounds(xMargin_exportButton, yMargin_exportButton, width_exportButton, height_exportButton);

	/*
	cursorPlot1.setBounds(500, topMarginXMinMax - 20, 70, 24); //mouse
	cursorPlot2.setBounds(500, topMarginXMinMax, 70, 24); //mouse
	cursorLabel0.setBounds(450, topMarginXMinMax - 20, 60, 24); //mouse
	cursorLabel1.setBounds(570, topMarginXMinMax - 20, 60, 24); //mouse
	cursorLabel2.setBounds(570, topMarginXMinMax, 60, 24); //mouse
	*/
}}

void FFTSpectrumAnalyzerAudioProcessorEditor::setFreqData(int fftData,int sampleRate) {
	fftS = fftData;
	numBins = fftS / 2 + 1;
	maxFreq = sampleRate / 2;
	numFreqBins = fftS / 2;

	indexToFreqMap.resize(numBins);

	//bins.resize(numBins);
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
	else { inputXmin.setText(std::to_string(xMin), juce::dontSendNotification); }
	
	temp = inputXmax.getText(false);
	val = std::atoi(temp.toStdString().c_str());
	if (val >= minVal && val <= maxVal)
	{
		xMax = val;
	}
	else { inputXmax.setText(std::to_string(xMax), juce::dontSendNotification); }

	temp = inputYmin.getText(false);
	val = std::atoi(temp.toStdString().c_str());
	if (val >= minVal && val <= maxVal)
	{
		yMin = val;
	}
	else { inputYmin.setText(std::to_string(yMin), juce::dontSendNotification); }

	temp = inputYmax.getText(false);
	val = std::atoi(temp.toStdString().c_str());
	if (val >= minVal && val <= maxVal)
	{
		yMax = val;
	}
	else { inputYmax.setText(std::to_string(yMax), juce::dontSendNotification); }
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


int FFTSpectrumAnalyzerAudioProcessorEditor::fftSize = 1024;
int FFTSpectrumAnalyzerAudioProcessorEditor::fftS = 0;
int FFTSpectrumAnalyzerAudioProcessorEditor::numBins = 0;
int FFTSpectrumAnalyzerAudioProcessorEditor::maxFreq = 0;
int FFTSpectrumAnalyzerAudioProcessorEditor::numFreqBins = 0;
/*
void FFTSpectrumAnalyzerAudioProcessorEditor::mouseMove(const juce::MouseEvent& event)
{
	cursorX1 = event.getMouseDownX();
	cursorY1 = event.getMouseDownY();
	//invalid bounds
	if (cursorX1 < 100 || cursorX1 > 1000 || cursorY1 < 30 || cursorY1 > 930) {
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
		if (audioProcessor.getPlotIndex() == 0 && isVisiblePlot1) {
			const float* scopeData = audioProcessor.getScopeData();
			cursorPlot1.setText("(" + floatToStringPrecision(cursorX1, 1) + ", " + floatToStringPrecision(scopeData[cursorX1], 2) + ")", juce::dontSendNotification);
		}
		//plot 2
		else if (audioProcessor.getPlotIndex() == 1 && isVisiblePlot2) {
			const float* scopeData2 = audioProcessor.getScopeData() + audioProcessor.getScopeSize();
			cursorPlot2.setText("(" + floatToStringPrecision(cursorX2, 1) + ", " + floatToStringPrecision(scopeData2[cursorX2], 2) + ")", juce::dontSendNotification);
		}
	}
	repaint();
}
*/

std::string FFTSpectrumAnalyzerAudioProcessorEditor::floatToStringPrecision(float f, int p)
{
	std::ostringstream oss;
	oss << std::fixed << std::setprecision(p) << f;
	return oss.str();
}

std::vector<float> FFTSpectrumAnalyzerAudioProcessorEditor::indexToFreqMap = { 0 };
std::vector< std::vector<float>> FFTSpectrumAnalyzerAudioProcessorEditor::binMag;
