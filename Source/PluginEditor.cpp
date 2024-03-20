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
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setOpaque(true);
    setSize (1200, 950);
    startTimer(500);
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

    int lineHeight = 10;  // Adjust line height as needed
    int startXText = 10;  // Start position from the left
    int startYText = 10;  // Start position from the top
    int textOffset = 100;  // Space between lines
    int startXPlot = 250;  // Offset X position
    int startYPlot1 = 450;
    int startYPlot2 = 450;
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
    //juce::Path yAxisMarkersDown;

    for (int j = 0; j < scopeSize; ++j) // Print the value of the samples
    {
        // Convert each float value to a string
        auto row1 = std::to_string(*((scopeData + j) + 0 * scopeSize));
        auto row2 = std::to_string(*((scopeData + j) + 1 * scopeSize));
        // Draw the string at appropriate positions
        g.drawText(row1, startXText, startYText + j * lineHeight, getWidth(), lineHeight, juce::Justification::left);
        g.drawText(row2, startXText + textOffset, startYText + j * lineHeight, getWidth(), lineHeight, juce::Justification::left);
    }

    // Graph plots
    plot1.startNewSubPath(startXPlot, startYPlot1 + scopeData[0] * scaleY);
    plot2.startNewSubPath(startXPlot, startYPlot2 + *(scopeData + 1 * scopeSize) * scaleY);
    for (int i = 1; i < sampleSize; i++)
    {
        plot1.lineTo(i * scaleX + startXPlot, *((scopeData + i) + 0 * scopeSize) * scaleY + startYPlot1);
        plot2.lineTo(i * scaleX + startXPlot, *((scopeData + i) + 1 * scopeSize) * scaleY + startYPlot2);
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
        //yAxisMarkersDownPlot1.startNewSubPath(offsetX - 5, startYAxis1 + (scaleYMarker * i));
        //yAxisMarkersDownPlot1.lineTo(offsetX + 5, startYAxis1 + (scaleYMarker * i));  // drawing line markers moving down from midpoint
    }
    g.setColour(juce::Colours::white);
    g.strokePath(yAxisMarkersUp, juce::PathStrokeType(2.0f));
    //g.strokePath(yAxisMarkersDown, juce::PathStrokeType(2.0f));
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
}

