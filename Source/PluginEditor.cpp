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
int FFTSpectrumAnalyzerAudioProcessorEditor::yMinPrev = 0;
int FFTSpectrumAnalyzerAudioProcessorEditor::yMin = 0; 
int FFTSpectrumAnalyzerAudioProcessorEditor::yMaxPrev = 10;
int FFTSpectrumAnalyzerAudioProcessorEditor::yMax = 10;

//==============================================================================
FFTSpectrumAnalyzerAudioProcessorEditor::FFTSpectrumAnalyzerAudioProcessorEditor (FFTSpectrumAnalyzerAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setOpaque(true);
    startTimer(500);
    setSize (1200, 1100);


    addAndMakeVisible(inputXmin);
    inputXmin.setEditable(true);
    inputXmin.setText(std::to_string(xMin), juce::dontSendNotification);
    inputXmin.setColour(juce::Label::backgroundColourId, juce::Colours::black);
    inputXmin.onTextChange = [this] { getXMin(); };
    addAndMakeVisible(inputXmax);
    inputXmax.setEditable(true);
    inputXmax.setText(std::to_string(xMax), juce::dontSendNotification);
    inputXmax.setColour(juce::Label::backgroundColourId, juce::Colours::black);
    inputXmax.onTextChange = [this] { getXMax(); };
    addAndMakeVisible(inputYmin);
    inputYmin.setEditable(true);
    inputYmin.setText(std::to_string(yMin), juce::dontSendNotification);
    inputYmin.setColour(juce::Label::backgroundColourId, juce::Colours::black);
    inputYmin.onTextChange = [this] { getYMin(); };
    addAndMakeVisible(inputYmax);
    inputYmax.setEditable(true);
    inputYmax.setText(std::to_string(yMax), juce::dontSendNotification);
    inputYmax.setColour(juce::Label::backgroundColourId, juce::Colours::black);
    inputYmax.onTextChange = [this] { getYMax(); };

}

FFTSpectrumAnalyzerAudioProcessorEditor::~FFTSpectrumAnalyzerAudioProcessorEditor()
{
}

//==============================================================================
void FFTSpectrumAnalyzerAudioProcessorEditor::paint (juce::Graphics& g)
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

    // Axis variables
    int lengthXAxis = 1000;
    int lengthYAxis = 800;
    int scaleXMarker = 50;
    int scaleYMarker = 50;
    int numXMarkers = 20;
    int numYMarkers = 15;   
    int yStartXAxis = 850;
    int yStartYAxis = 850;

    int xDiff = xMax - xMin;
    if (xDiff == 0)  // handles divide by zero errors
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
    int scaleX = lengthXAxis / xDiff;  // Scaling X increments; pixels shown per sample
    int xShift = -xMin * scaleX;

    int yDiff = yMax - yMin;
    if (yDiff == 0)  // handles divide by zero errors
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
    int scaleY = -lengthYAxis / yDiff;  // Scaling X increments; pixels shown per sample
    int yShift = -yMin * scaleY; 

    juce::Path plot1;
    juce::Path plot2;
    juce::Path xAxis;
    juce::Path xAxisMarkers;
    juce::Path yAxis;
    juce::Path yAxisMarkersUp;
    juce::Path zeroTick;

    // Graph plots
    plot1.startNewSubPath(xStartPlot + xShift, yStartPlot + scopeData[0] * scaleY + yShift);  // Xmin needs to be the new startXPlot; this will be reset by the bounds read in to xMin textEntry box
    plot2.startNewSubPath(xStartPlot + xShift, yStartPlot + *(scopeData + 1 * scopeSize) * scaleY + yShift);
    for (int i = 1; i < sampleSize; i++)
    {
        plot1.lineTo(i * scaleX + xStartPlot + xShift, *((scopeData + i) + 0 * scopeSize) * scaleY + yStartPlot + yShift);
        plot2.lineTo(i * scaleX + xStartPlot + xShift, *((scopeData + i) + 1 * scopeSize) * scaleY + yStartPlot + yShift);
    }
    g.setColour(juce::Colours::yellow);
    g.strokePath(plot1, juce::PathStrokeType(5.0f));
    g.setColour(juce::Colours::red);
    g.strokePath(plot2, juce::PathStrokeType(5.0f));

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
    g.fillRect(0, 850, 1200, 100);


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
    zeroTick.startNewSubPath(xStartPlot - 15, yStartPlot);
    zeroTick.lineTo(xStartPlot + 15, yStartPlot);
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
    if (!isRunning && audioProcessor.getProcBlockCalled()){
        isRunning = true;
        audioProcessor.resetProcBlockCalled();
    }
    else if (isRunning && !audioProcessor.getProcBlockCalled()){
        isRunning = false;
        repaint();
        audioProcessor.resetScopeDataIndex();
        audioProcessor.incrementPlotIndex();
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

    int textEntryWidth = 60;
    int textEntryHeight = 24;

    inputXmin.setBounds(leftMarginXmin, topMarginXmin, textEntryWidth, textEntryHeight);
    inputXmax.setBounds(leftMarginXmax, topMarginXmax, textEntryWidth, textEntryHeight);
    inputYmin.setBounds(leftMarginYmin, topMarginYmin, textEntryWidth, textEntryHeight);
    inputYmax.setBounds(leftMarginYmax, topMarginYmax, textEntryWidth, textEntryHeight);
}

void FFTSpectrumAnalyzerAudioProcessorEditor::getXMin()
{
    bool isValid = true;

    juce::String temp = inputXmin.getText(false);
    for (int i = 0; i < temp.length(); i++) {
        if (temp[i] < '0' || temp[i] > '9') {
            isValid = false;
        }
    }
    if (isValid == true) {
        xMin = temp.getIntValue();
        repaint();
    }
}
void FFTSpectrumAnalyzerAudioProcessorEditor::getXMax()
{
    bool isValid = true;

    juce::String temp = inputXmax.getText(false);
    for (int i = 0; i < temp.length(); i++) {
        if (temp[i] < '0' || temp[i] > '9') {
            isValid = false;
        }
    }
    if (isValid == true) {
        xMax = temp.getIntValue();
        repaint();
    }
}
void FFTSpectrumAnalyzerAudioProcessorEditor::getYMin()
{
    bool isValid = true;

    juce::String temp = inputYmin.getText(false);
    for (int i = 0; i < temp.length(); i++) {
        if (temp[i] < '0' || temp[i] > '9') {
            isValid = false;
        }
    }
    if (isValid == true) {
        yMin = temp.getIntValue();
        repaint();
    }
}
void FFTSpectrumAnalyzerAudioProcessorEditor::getYMax()
{
    bool isValid = true;

    juce::String temp = inputYmax.getText(false);
    for (int i = 0; i < temp.length(); i++) {
        if (temp[i] < '0' || temp[i] > '9') {
            isValid = false;
        }
    }
    if (isValid == true) {
        yMax = temp.getIntValue();
        repaint();
    }
}



