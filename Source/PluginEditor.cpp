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
FFTSpectrumAnalyzerAudioProcessorEditor::FFTSpectrumAnalyzerAudioProcessorEditor(FFTSpectrumAnalyzerAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

    setOpaque(true);
    setSize(1000, 600);
    startTimer(500);
}

FFTSpectrumAnalyzerAudioProcessorEditor::~FFTSpectrumAnalyzerAudioProcessorEditor()
{
}

//==============================================================================

void FFTSpectrumAnalyzerAudioProcessorEditor::paint(juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    //g.fillAll(juce::Colours::black);


    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    //g.setColour(juce::Colours::white);
    //g.setFont(15.0f);

    g.setOpacity(1.0f);
    g.setColour(juce::Colours::white);

    int lineHeight = 10; // Adjust line height as needed
    int yPosition = 10; // Start position from the top
    int xPosition = 10; // Start position from the left

    //g.drawFittedText(str, getLocalBounds(), juce::Justification::centred, 1);
    const int scopeSize = audioProcessor.getScopeSize();
    const float* scopeData = audioProcessor.getScopeData();

    const float* fft = audioProcessor.getFFT();
    /*
    for (int i = 0; i < scopeSize; ++i)
    {

        // Convert each float value to a string
        auto valueString = std::to_string(scopeData[i]); // Change '4' to the desired number of decimal places

        // Draw the string at appropriate positions
        g.drawText(valueString, xPosition, yPosition + i * lineHeight, getWidth(), lineHeight, juce::Justification::left);
    }
     */
    
    juce::Path graph;

    //x-axis variables
    float graphOffsetX_xAxis = 20;
    float graphXOffsetY_xAxis = 300;
    float graphXLength_xAxis = 950;

    //y-axis variables
    float graphOffsetX_yAxis = 20;
    float graphXOffsetY_yAxis = 10;
    float graphXLength_yAxis = 291;
    float graphMidpoint_yAxis = graphXOffsetY_yAxis + graphXLength_yAxis;

    //x-axis markers
    float graphOffscaleY_xMarker = 100;

    //y-axis markers
    float graphOffscaleY_yMarker = 100;

    // graph x-axis
    graph.startNewSubPath(graphOffsetX_xAxis, graphXOffsetY_xAxis);
    graph.lineTo(graphOffsetX_xAxis + graphXLength_xAxis, graphXOffsetY_xAxis);
    g.strokePath(graph, juce::PathStrokeType(2.0f));

    // graph y-axis
    graph.startNewSubPath(graphOffsetX_yAxis, graphXOffsetY_yAxis);
    graph.lineTo(graphOffsetX_yAxis, graphXOffsetY_yAxis + graphXLength_yAxis);
    graph.lineTo(graphOffsetX_yAxis, graphMidpoint_yAxis + graphXLength_yAxis);
    g.strokePath(graph, juce::PathStrokeType(2.0f));

    // graph x-axis marker
    graph.startNewSubPath(graphOffsetX_xAxis + graphOffscaleY_xMarker, graphXOffsetY_xAxis - 5);
    graph.lineTo(graphOffsetX_xAxis + graphOffscaleY_xMarker, graphXOffsetY_xAxis + 5);
    g.strokePath(graph, juce::PathStrokeType(2.0f));

    for (int i = 1; i < 13; i++) {
        graph.startNewSubPath(graphOffsetX_xAxis + graphOffscaleY_xMarker + (i * graphOffscaleY_xMarker), graphXOffsetY_xAxis - 5);
        graph.lineTo(graphOffsetX_xAxis + graphOffscaleY_xMarker + (i * graphOffscaleY_xMarker), graphXOffsetY_xAxis + 5);
    }
    g.strokePath(graph, juce::PathStrokeType(2.0f));

    // graph y-axis marker
    for (int i = 1; i < 3; i++) {
        graph.startNewSubPath(graphOffsetX_yAxis - 5, graphXOffsetY_xAxis - graphOffscaleY_yMarker * i);
        graph.lineTo(graphOffsetX_xAxis + 5, graphXOffsetY_xAxis - graphOffscaleY_yMarker * i);
    }
    g.strokePath(graph, juce::PathStrokeType(2.0f));

    for (int i = 1; i < 3; i++) {
        graph.startNewSubPath(graphOffsetX_yAxis - 5, graphXOffsetY_xAxis + graphOffscaleY_yMarker * i);
        graph.lineTo(graphOffsetX_xAxis + 5, graphXOffsetY_xAxis + graphOffscaleY_yMarker * i);
    }
    g.strokePath(graph, juce::PathStrokeType(2.0f));



    ///*
    juce::Path myPath;
    float offsetX = 20;
    float offsetY = 300;
    float scaleX = 10;
    float scaleY = 100;
    float sampleSize = 512;
    float FlipYAxisValue = -1;

    myPath.startNewSubPath(offsetX, offsetY + scopeData[0]); //observe closely
    for (int i = 1; i < sampleSize; i++)
    {
        myPath.lineTo(i * scaleX + offsetX, FlipYAxisValue * scopeData[i] * scaleY + offsetY);
    }

    g.strokePath(myPath, juce::PathStrokeType(1.0f));
    //*/
}

void FFTSpectrumAnalyzerAudioProcessorEditor::timerCallback()
{
    if (!isRunning && audioProcessor.getProcBlockIsCalled()) {      // isRunning: false, procBlockCalled: true -> procBlockCalled: false, isRunning: true
        audioProcessor.resetProcBlockIsCalled();
        isRunning = true;
    } else {                                                        // **isRunning: true**                                              
        if (!audioProcessor.getProcBlockIsCalled()) {               // isRunning: true -> procBlockCalled: false, isRunning: false
            isRunning = false;
            repaint();                                              // block is finished processing, call repaint // timerCallback recalled later
        }
    }
}

void FFTSpectrumAnalyzerAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
