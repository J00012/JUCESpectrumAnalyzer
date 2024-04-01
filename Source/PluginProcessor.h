/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "RingBuffer.h"
//==============================================================================
/**
*/
class FFTSpectrumAnalyzerAudioProcessor  : public juce::AudioProcessor
{
    

public:
    //==============================================================================
    FFTSpectrumAnalyzerAudioProcessor();
    ~FFTSpectrumAnalyzerAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    bool getProcBlockCalled();
    void resetProcBlockCalled();
    void resetScopeDataIndex();
    void setPlotIndex(int rowIndex);
    int getPlotIndex();
    int getPlotSize();

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    void setFFTSize(int newFFTsize);
    int getStepSize() const;
    int getFFTCounter() const;
    int getBlockSampleRate() const;
    const float* getBins() const;
    const float* getRingTest() const;
   


    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override; 

private:
    juce::dsp::FFT forwardFFT;                      // [4]      //THIS IS IT THE FFT class

    //declare the ringBuffer and set its size to 10000
    RingBuffer<float> ringBuffer{ 10000 };  

    //juce::dsp::WindowingFunction<float>::WindowingMethod window;

  
    static int fftCounter;

    static int sampleRate;

    static int fftSize;
    static int stepSize;
    static int numBins;
    static int numFreqBins;
    static int fftDataSize;
   
    static float ringTest[10000];

    static std::vector<float> bufferRight;
    static std::vector<float> bufferLeft;
    static std::vector<float> windowBufferRight;
    static std::vector<float> windowBufferLeft;
    static std::vector<float> bins;
  
    bool nextFFTBlockReady = false;
    bool procBlockCalled = false;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FFTSpectrumAnalyzerAudioProcessor)
};
