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
    setSize (700, 1000);

    
}

FFTSpectrumAnalyzerAudioProcessorEditor::~FFTSpectrumAnalyzerAudioProcessorEditor()
{
}

//==============================================================================


//draws the frame to be displayed using drawLine fucntion from g (other options are available)
void FFTSpectrumAnalyzerAudioProcessorEditor::drawFrame(juce::Graphics& g)
{
    const int scopeSize = audioProcessor.getScopeSize();
    const float* scopeData = audioProcessor.getScopeData();
   

    for (int i = 1; i < scopeSize; ++i)
    {
        auto width = getLocalBounds().getWidth();
        auto height = getLocalBounds().getHeight();

        g.drawLine({ (float)juce::jmap(i - 1, 0, scopeSize - 1, 0, width),
                              juce::jmap(scopeData[i - 1], 0.0f, 1.0f, (float)height, 0.0f),
                      (float)juce::jmap(i,     0, scopeSize - 1, 0, width),
                              juce::jmap(scopeData[i],     0.0f, 1.0f, (float)height, 0.0f) });
    }
    
}


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
    const double* array = audioProcessor.getArray(); 
    const float* fft = audioProcessor.getFFT();


    // Allocate memory for 'ar' as an array of doubles
    double* ar = new double[scopeSize];

    // Copy the contents of 'array' into 'ar'
    for (int i = 0; i < scopeSize; ++i)
    {
        ar[i] = array[i];
    }


        for (int i = 0; i < 2048; ++i)
        {
            
            // Convert each float value to a string
            auto valueString = std::to_string(fft[i]); // Change '4' to the desired number of decimal places

            // Draw the string at appropriate positions
            g.drawText(valueString, xPosition, yPosition + i * lineHeight, getWidth() , lineHeight, juce::Justification::left);
        }
    

    
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
