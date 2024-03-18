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
    setSize (1200, 1000);
    startTimer(500); // Timer callback in milliseconds  
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

    int lineHeight = 10; // Adjust line height as needed
    int yPosition = 10; // Start position from the top
    int xPosition = 10; // Start position from the left
    int textOffset = 100;
    int graph1StartY = 200;
    int graph2StartY = 600;
    float offsetX = 70; // Offset X position
    float scaleX = 10; // Scaling X increments
    float scaleY = -50; // Scaling Y increments
    float sampleSize = 100; // Adjust the number of samples being displayed as needed
    juce::Path myPath1;
    juce::Path myPath2;
    
    for (int j = 0; j < scopeSize; ++j) // Print the value of the samples
    {
        // Convert each float value to a string
        auto row1 = std::to_string(*((scopeData + j) + 0 * scopeSize));
        auto row2 = std::to_string(*((scopeData + j) + 1 * scopeSize));
        // Draw the string at appropriate positions
        g.drawText(row1, xPosition, yPosition + j * lineHeight, getWidth(), lineHeight, juce::Justification::left);
        g.drawText(row2, xPosition + textOffset, yPosition + j * lineHeight, getWidth(), lineHeight, juce::Justification::left);
    }
    

    // Draws the waveform; loops through the samples that have been read in
    myPath1.startNewSubPath(offsetX, graph1StartY + scopeData[0]);
    myPath2.startNewSubPath(offsetX, graph2StartY + *(scopeData + 1 * scopeSize));
    for (int i = 1; i < sampleSize; i++)
    {
        myPath1.lineTo(i * scaleX + offsetX, *((scopeData + i) + 0 * scopeSize) * scaleY + graph1StartY);
        myPath2.lineTo(i * scaleX + offsetX, *((scopeData + i) + 1 * scopeSize) * scaleY + graph2StartY);
    }
    g.strokePath(myPath1, juce::PathStrokeType(5.0f)); 
    g.strokePath(myPath2, juce::PathStrokeType(5.0f)); 
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

