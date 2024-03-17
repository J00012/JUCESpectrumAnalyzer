/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <string>


//==============================================================================
FFTSpectrumAnalyzerAudioProcessorEditor::FFTSpectrumAnalyzerAudioProcessorEditor(FFTSpectrumAnalyzerAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

    setOpaque(true);
    setSize(1000, 600);
    startTimer(50);
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

    juce::Path myPath;
    float offsetX = 1;
    float offsetY = 300;
    float scaleX = 10;
    float scaleY = 50;
    float sampleSize = 512;
    float FlipYAxisValue = -1;

    myPath.startNewSubPath(offsetX, offsetY + scopeData[0]); //observe closely
    for (int i = 1; i < sampleSize; i++)
    {
        myPath.lineTo(i * scaleX + offsetX, FlipYAxisValue * scopeData[i] * scaleY + offsetY);
    }

    g.strokePath(myPath, juce::PathStrokeType(1.0f));
}

void FFTSpectrumAnalyzerAudioProcessorEditor::timerCallback()
{
    if (!audioProcessor.getProcBlockIsRunning()) {                  // isRunning: false
        if (!audioProcessor.getProcBlockIsCalled()) {               // isRunning: false -> procBlockCalled: false
                                                                    // do nothing // timerCallback recalled later
        }
        else {                                                      // isRunning: false -> **procBlockCalled: true**
            audioProcessor.resetProcBlockIsCalled();                // isRunning: false -> procBlockCalled: true, isRunning: true // timerCallback recalled later
        }
    } else {                                                        // **isRunning: true**                                              
        if (!audioProcessor.getProcBlockIsCalled()) {               // isRunning: true -> procBlockCalled: false
            audioProcessor.resetProcBlockIsRunning();               // isRunning: true -> procBlockCalled: false, isRunning: false
            repaint();                                              // block is finished processing, call repaint // timerCallback recalled later
        }
        else {                                                      // isRunning: true -> **procBlockCalled: true** // timerCallback recalled later
                                                                    // do nothing // timerCallback recalled later
        }
    }
}

void FFTSpectrumAnalyzerAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
