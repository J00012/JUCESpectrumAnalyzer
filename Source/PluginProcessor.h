/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
//==============================================================================
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

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    void setStepSize(int stepS);
    void setInitialBlock();
    void setInitialAccBuffer() const;
    void setFFTSize(int newFFTSize);
    int getBlockSampleRate() const;
    void clearAccumulationBuffer();
    std::vector<float> getAccumulationBuffer() const;

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
  
    static int sampleRate;
    static int fftSize;
    static int stepSize;
    static bool initialBlock;
    static int channel;
    static std::vector<float> accumulationBuffer;
    bool procBlockCalled = false;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FFTSpectrumAnalyzerAudioProcessor)
};