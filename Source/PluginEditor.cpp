/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <string>


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
    const float* scopeData = audioProcessor.getScopeData();
    const float* fft = audioProcessor.getFFT();

    int lineHeight = 10; // Adjust line height as needed
    int yPosition = 10; // Start position from the top
    int xPosition = 10; // Start position from the left
    float offsetX = 70; // Offset X position
    float offsetY = 500; // Offset Y position
    float scaleX = 10; // Scaling X increments
    float scaleY = 50; // Scaling Y increments
    float sampleSize = 100; // Adjust the number of samples being displayed as needed
    float FlipYAxisValue = -1; // Used to flip the graph vertically to the correct orientation
    auto msg = "default"; // default print message
    juce::Path myPath; // Used when we print the graph 

    for (int i = 0; i < scopeSize; ++i) // Print the value of the samples
    {
        // Convert each float value to a string
        auto valueString = std::to_string(scopeData[i]); // Change '4' to the desired number of decimal places
        // Draw the string at appropriate positions
        g.drawText(valueString, xPosition, yPosition + i * lineHeight, getWidth() , lineHeight, juce::Justification::left);

    }
    
 
    // Use to write to the gui the status of the processBlock
    if (audioProcessor.IsProcBlockEvents())
    {
        g.drawText(audioProcessor.getProcBlockEvents(), xPosition + offsetX, yPosition * lineHeight, getWidth(), lineHeight, juce::Justification::left);
        //audioProcessor.clearProcBlockEvents();
    }

    // Draws the waveform; loops through the samples that have been read in
    myPath.startNewSubPath(offsetX, offsetY + scopeData[0]);
    for (int i = 1; i < sampleSize; i++)
    {
        myPath.lineTo(i * scaleX + offsetX, FlipYAxisValue * scopeData[i] * scaleY + offsetY);
    }
    g.strokePath(myPath, juce::PathStrokeType(5.0f)); 
}

void FFTSpectrumAnalyzerAudioProcessorEditor::timerCallback()
{
    if (audioProcessor.IsProcBlockEvents())
    {
        repaint();
        // stopTimer();
    }
}

void FFTSpectrumAnalyzerAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
