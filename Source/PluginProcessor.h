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

    enum
    {
        fftOrder = 10,             // [1]
        fftSize = 1 << fftOrder,  // [2]
        stepSize = 512,            // [3]	   //this will probably need to change since it is the "SIZE"
        numBins = fftSize / 2 +1,
        numFreqBins = fftSize / 2
    };


public:
    //==============================================================================
    FFTSpectrumAnalyzerAudioProcessor();
    ~FFTSpectrumAnalyzerAudioProcessor() override
;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    bool getProcBlockIsRunning();
    void resetProcBlockIsRunning();
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    int getStepSize() const;
    int getFFTCounter() const;
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

    //declare the ringBuffer and set its size to 1000
    RingBuffer<float> ringBuffer{ 10000 };  

    //juce::dsp::WindowingFunction<float>::WindowingMethod window;

    //counters
    int fftCounter = 0;
   
    //static float arrays
    static float bufferRight[fftSize];
    static float bufferLeft[fftSize];
    static float windowBufferRight[fftSize*2];
    static float windowBufferLeft[fftSize];
    static float indexFreqMap[numBins];
    static float ringTest[stepSize];
    static float scopeData[stepSize];                  // [10]	
    static float bins[fftSize / 2 + 1];
  

    //UI Teams code
    bool nextFFTBlockReady = false;                 // [9]	//DONT NEED
    bool procBlockIsRunning = false;
    

    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FFTSpectrumAnalyzerAudioProcessor)
};
