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
class FFTSpectrumAnalyzerAudioProcessor : public juce::AudioProcessor
{


public:
    //==============================================================================
    FFTSpectrumAnalyzerAudioProcessor();
    ~FFTSpectrumAnalyzerAudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    bool getProcBlockCalled();
    void resetProcBlockCalled();
    //void resetScopeDataIndex();
    //void setPlotIndex(int rowIndex);
    //int getPlotIndex();
    //int getPlotSize();

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    void zeroSelection(int selectionIndex, int selectionSize);
    void zeroAllSelections(int newFFTSize, int selectionSize);
    void removeSelection(int selectionIndex);
    void clearAllSelections();
    void prepSelection(int binMagSize, int selectionSize, int selectionIndex);
    void prepBuffers(int fftSize);
    void setInitialBlock();
    void setRowIndex(int plotIndex);
    void setFFTSize(int newFFTSize);
    void setWindow(juce::dsp::WindowingFunction<float>::WindowingMethod type);
    int getStepSize() const;
    int getFFTCounter() const;
    int getBlockSampleRate() const;
    std::vector<std::vector<float>> getBinMag() const;



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
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    static bool minBlockSize;

private:
    static juce::dsp::FFT forwardFFT;                      // [4]      //THIS IS IT THE FFT class

    //declare the ringBuffer and set its size to 10000
    RingBuffer<float> ringBuffer{ 10000 };

    static juce::dsp::WindowingFunction<float> window;


    static int fftCounter;

    static int sampleRate;

    static int fftSize;
    static int stepSize;
    static int numBins;
    static int numFreqBins;
    static int fftDataSize;
    static int rowIndex;
    static bool initialBlock;
    static int channel;

    static float ringTest[10000];

    static std::vector<float> bufferRight;
    static std::vector<float> bufferLeft;
    static std::vector<float> windowBufferRight;
    static std::vector<float> windowBufferLeft;
    static std::vector<std::vector<float>> binMag;

    bool procBlockCalled = false;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FFTSpectrumAnalyzerAudioProcessor)
};