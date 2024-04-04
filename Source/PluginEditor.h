/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
class FFTSpectrumAnalyzerAudioProcessorEditor  : public juce::AudioProcessorEditor, juce::Timer
{
public:
    FFTSpectrumAnalyzerAudioProcessorEditor (FFTSpectrumAnalyzerAudioProcessor&);
    ~FFTSpectrumAnalyzerAudioProcessorEditor() override;

    //==============================================================================
    
    void paint (juce::Graphics&) override;
    void drawFrame(juce::Graphics& g); 
    void setFreqData(int fftData,int sampleR);
    void resized() override;
    void timerCallback() override;
   
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    FFTSpectrumAnalyzerAudioProcessor& audioProcessor;
    static bool isRunning;

    static int fftSize;

    static int fftS;
    static int numBins;
    static int sampleRate;
    static int maxFreq;
    static int numFreqBins;

    static int row;
    

    static std::vector<float> indexToFreqMap;
    static std::vector<std::vector<float>> binMag;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FFTSpectrumAnalyzerAudioProcessorEditor)
};
