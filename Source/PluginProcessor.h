/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class FFTSpectrumAnalyzerAudioProcessor  : public juce::AudioProcessor
{

    enum
    {
        fftOrder = 11,             // [1]
        fftSize = 1 << fftOrder,  // [2]
        scopeSize = 512             // [3]	   //this will probably need to change since it is the "SIZE"
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

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    void drawNextFrameOfSpectrum(float* channelData, int numSample);
    


    int getScopeSize() const;
    const float* getScopeData() const;
    const double* getArray() const;
    const float* getFFT() const;


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
    juce::dsp::WindowingFunction<float> window;     // [5]	//HERE IS THE WINDOW DECLARATION

    float fftArray[fftSize] = {0};
    float fftData[2 * fftSize] = { 0 };                    // [7]	//NEED

    int fftArrayIndex = 0;
   
    bool nextFFTBlockReady = false;                 // [9]	//DONT NEED
    static float scopeData[scopeSize];                  // [10]	
   // double array[6] = { 0,0,0,0,0,0};
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FFTSpectrumAnalyzerAudioProcessor)
};
