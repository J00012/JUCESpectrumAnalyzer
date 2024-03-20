/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <string>

bool FFTSpectrumAnalyzerAudioProcessorEditor::isRunning = false;

//==============================================================================
FFTSpectrumAnalyzerAudioProcessorEditor::FFTSpectrumAnalyzerAudioProcessorEditor (FFTSpectrumAnalyzerAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setOpaque(true);
    startTimer(500);
    setSize (1200, 1100);


    addAndMakeVisible(inputXmin);
    inputXmin.setEditable(true);
    inputXmin.setColour(juce::Label::backgroundColourId, juce::Colours::black);
    addAndMakeVisible(inputXmax);
    inputXmax.setEditable(true);
    inputXmax.setColour(juce::Label::backgroundColourId, juce::Colours::black);
    addAndMakeVisible(inputYmin);
    inputYmin.setEditable(true);
    inputYmin.setColour(juce::Label::backgroundColourId, juce::Colours::black);
    addAndMakeVisible(inputYmax);
    inputYmax.setEditable(true);
    inputYmax.setColour(juce::Label::backgroundColourId, juce::Colours::black);

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

    int startXPlot = 100;  // Offset X position
    int startYPlot = 450;
    int scaleX = 10;  // Scaling X increments
    int scaleY = -50;  // Scaling Y increments + vertical flip
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

    juce::Path plot1;
    juce::Path plot2;
    juce::Path xAxis;
    juce::Path xAxisMarkers;
    juce::Path yAxis;
    juce::Path yAxisMarkersUp;
    juce::Path zeroTick;
    //juce::Path yAxisMarkersDown;

    // Graph plots
    plot1.startNewSubPath(startXPlot, startYPlot + scopeData[0] * scaleY);
    plot2.startNewSubPath(startXPlot, startYPlot + *(scopeData + 1 * scopeSize) * scaleY);
    for (int i = 1; i < sampleSize; i++)
    {
        plot1.lineTo(i * scaleX + startXPlot, *((scopeData + i) + 0 * scopeSize) * scaleY + startYPlot);
        plot2.lineTo(i * scaleX + startXPlot, *((scopeData + i) + 1 * scopeSize) * scaleY + startYPlot);
    }
    g.setColour(juce::Colours::yellow);
    g.strokePath(plot1, juce::PathStrokeType(5.0f));
    g.setColour(juce::Colours::red);
    g.strokePath(plot2, juce::PathStrokeType(5.0f));

    // Plot x-axis
    xAxis.startNewSubPath(startXPlot, yStartXAxis);
    xAxis.lineTo(startXPlot + lengthXAxis, yStartXAxis);
    g.setColour(juce::Colours::white);
    g.strokePath(xAxis, juce::PathStrokeType(2.0f));

    // Plot y-axis
    yAxis.startNewSubPath(startXPlot, yStartYAxis);
    yAxis.lineTo(startXPlot, yStartYAxis - lengthYAxis);
    g.setColour(juce::Colours::white);
    g.strokePath(yAxis, juce::PathStrokeType(2.0f));

    //Plot zero on Y-axis
    zeroTick.startNewSubPath(startXPlot - 15, startYPlot);
    zeroTick.lineTo(startXPlot + 15, startYPlot);
    g.strokePath(zeroTick, juce::PathStrokeType(4.0f));

    // Plot X Axis Markers
    for (int i = 1; i < numXMarkers; i++) {
        xAxisMarkers.startNewSubPath(startXPlot + (i * scaleXMarker), yStartXAxis - 5);
        xAxisMarkers.lineTo(startXPlot + (i * scaleXMarker), yStartXAxis + 5);
    }
    g.setColour(juce::Colours::white);
    g.strokePath(xAxisMarkers, juce::PathStrokeType(2.0f));
    
   
    // Plot Y Axis Markers
    for (int i = 1; i <= numYMarkers; i++) {
        yAxisMarkersUp.startNewSubPath(startXPlot - 5, yStartYAxis - (scaleYMarker * i));
        yAxisMarkersUp.lineTo(startXPlot + 5, yStartYAxis - (scaleYMarker * i));  // drawing line markers moving up from midpoint
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

