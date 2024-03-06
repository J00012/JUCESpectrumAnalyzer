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
    setSize (1200, 700);

    
}

FFTSpectrumAnalyzerAudioProcessorEditor::~FFTSpectrumAnalyzerAudioProcessorEditor()
{
}

//==============================================================================


void FFTSpectrumAnalyzerAudioProcessorEditor::paint (juce::Graphics& g)
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
            g.drawText(valueString, xPosition, yPosition + i * lineHeight, getWidth() , lineHeight, juce::Justification::left);
        }
    */
    juce::Path myPath;
    float offsetX = 1;
    float offsetY = 400;
    float scaleX = 1;
    float scaleY = 500;
    float sampleSize = 512;
    float FlipYAxisValue = -1;

    myPath.startNewSubPath(offsetX, offsetY + scopeData[0]); //observe closely
    for (int i = 1; i < sampleSize; i++)
    {
        myPath.lineTo(i * scaleX + offsetX, FlipYAxisValue * scopeData[i] * scaleY + offsetY);
    }

 /*   myPath.lineTo(100.0f, 200.0f);
    myPath.lineTo(200.0f, 300.0f);*/

    g.strokePath(myPath, juce::PathStrokeType(1.0f));
    
    /*
    auto str1 = std::to_string(scopeData[0]);
    std::cout << scopeData[0];
    g.drawText(str1, getLocalBounds(), juce::Justification::centred, true);

    for (int i = 1; i < scopeSize; ++i)
    {
        auto width = getLocalBounds().getWidth();
        auto height = getLocalBounds().getHeight();

        g.drawLine({ (float)juce::jmap(i - 1, 0, scopeSize - 1, 0, width),
                              juce::jmap(scopeData[i - 1], 0.0f, 1.0f, (float)height, 0.0f),
                      (float)juce::jmap(i,     0, scopeSize - 1, 0, width),
                              juce::jmap(scopeData[i],     0.0f, 1.0f, (float)height, 0.0f) });
    }
    //drawFrame(g);
    */
}


void FFTSpectrumAnalyzerAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
