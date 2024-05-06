/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"



//zero the static variables
int FFTSpectrumAnalyzerAudioProcessor::sampleRate = 0;
int FFTSpectrumAnalyzerAudioProcessor::channel = 0;
int FFTSpectrumAnalyzerAudioProcessor::fftSize = 1024;
int FFTSpectrumAnalyzerAudioProcessor::stepSize = 512;
bool FFTSpectrumAnalyzerAudioProcessor::initialBlock = true;
bool FFTSpectrumAnalyzerAudioProcessor::minBlockSize = false;

std::vector<float> FFTSpectrumAnalyzerAudioProcessor::accumulationBuffer;

//==============================================================================
FFTSpectrumAnalyzerAudioProcessor::FFTSpectrumAnalyzerAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    )
    // Initialize forwardFFT with fftOrder
#endif
{
}

FFTSpectrumAnalyzerAudioProcessor::~FFTSpectrumAnalyzerAudioProcessor() // This is the destructor
{
}

//==============================================================================
const juce::String FFTSpectrumAnalyzerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool FFTSpectrumAnalyzerAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool FFTSpectrumAnalyzerAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool FFTSpectrumAnalyzerAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double FFTSpectrumAnalyzerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int FFTSpectrumAnalyzerAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int FFTSpectrumAnalyzerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void FFTSpectrumAnalyzerAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String FFTSpectrumAnalyzerAudioProcessor::getProgramName(int index)
{
    return {};
}

void FFTSpectrumAnalyzerAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void FFTSpectrumAnalyzerAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void FFTSpectrumAnalyzerAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool FFTSpectrumAnalyzerAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

//================================================PROCESS BLOCK====================================================================//
void FFTSpectrumAnalyzerAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    if (initialBlock == true && buffer.getNumSamples() < stepSize) {
        minBlockSize = false;
    }
    else {
        minBlockSize = true;
        sampleRate = getSampleRate();  

        juce::ScopedNoDenormals noDenormals;
        auto totalNumInputChannels = getTotalNumInputChannels();
        auto totalNumOutputChannels = getTotalNumOutputChannels();

        auto* channelData = buffer.getWritePointer(channel);

        for (int i = 0; i < buffer.getNumSamples(); i++) {
            accumulationBuffer.push_back(channelData[i]);
        }
        procBlockCalled = true;
        initialBlock = false;
    }
}

std::vector<float> FFTSpectrumAnalyzerAudioProcessor::getAccumulationBuffer() const
{
    return accumulationBuffer;
}

void FFTSpectrumAnalyzerAudioProcessor::setInitialAccBuffer() const {
    accumulationBuffer.resize(fftSize, 0.0f);
}

void FFTSpectrumAnalyzerAudioProcessor::clearAccumulationBuffer() {
    accumulationBuffer.clear();
}

void FFTSpectrumAnalyzerAudioProcessor::setInitialBlock() {
    initialBlock = true;
}

void FFTSpectrumAnalyzerAudioProcessor::resetProcBlockCalled()
{
    procBlockCalled = false;
}

bool FFTSpectrumAnalyzerAudioProcessor::getProcBlockCalled()
{
    return procBlockCalled;
}

int FFTSpectrumAnalyzerAudioProcessor::getBlockSampleRate() const
{
    return sampleRate;
}


void FFTSpectrumAnalyzerAudioProcessor::setFFTSize(int newFFTSize) {
    fftSize = newFFTSize;
    stepSize = fftSize / 2;
}

void FFTSpectrumAnalyzerAudioProcessor::setStepSize(int stepS) {
    stepSize = stepS;
}

//==============================================================================
bool FFTSpectrumAnalyzerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* FFTSpectrumAnalyzerAudioProcessor::createEditor()
{
    return new FFTSpectrumAnalyzerAudioProcessorEditor(*this);
}

//==============================================================================
void FFTSpectrumAnalyzerAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data. 
}

void FFTSpectrumAnalyzerAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FFTSpectrumAnalyzerAudioProcessor();
}