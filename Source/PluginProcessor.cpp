/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "RingBuffer.h"


//zero the static variables
int FFTSpectrumAnalyzerAudioProcessor::sampleRate = 0;
int FFTSpectrumAnalyzerAudioProcessor::fftCounter = 0;

int FFTSpectrumAnalyzerAudioProcessor::channel = 0;
int FFTSpectrumAnalyzerAudioProcessor::rowIndex = 0;
int FFTSpectrumAnalyzerAudioProcessor::rowSize = 0;
int FFTSpectrumAnalyzerAudioProcessor::fftSize = 1024;
int FFTSpectrumAnalyzerAudioProcessor::stepSize = 512;
int FFTSpectrumAnalyzerAudioProcessor::numBins = 0;
int FFTSpectrumAnalyzerAudioProcessor::numFreqBins = 0;
int FFTSpectrumAnalyzerAudioProcessor::fftDataSize = 0;

bool FFTSpectrumAnalyzerAudioProcessor::initialBlock = true;
bool FFTSpectrumAnalyzerAudioProcessor::minBlockSize = false;

//juce::dsp::FFT FFTSpectrumAnalyzerAudioProcessor::forwardFFT(0);

// Define static member variables
//std::vector<float> FFTSpectrumAnalyzerAudioProcessor::bufferRight = { 0 };
//std::vector<float> FFTSpectrumAnalyzerAudioProcessor::bufferLeft = { 0 };
//std::vector<float> FFTSpectrumAnalyzerAudioProcessor::windowBufferRight = { 0 };
//std::vector<float> FFTSpectrumAnalyzerAudioProcessor::windowBufferLeft = { 0 };
//std::vector<std::vector<float>> FFTSpectrumAnalyzerAudioProcessor::binMag;
std::vector<float> FFTSpectrumAnalyzerAudioProcessor::accumulationBuffer;

juce::dsp::WindowingFunction<float> FFTSpectrumAnalyzerAudioProcessor::window(0, juce::dsp::WindowingFunction<float>::blackman);

//float FFTSpectrumAnalyzerAudioProcessor::ringTest[] = { 0 };

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

//void FFTSpectrumAnalyzerAudioProcessor::zeroSelection(int selectionIndex) {
//    std::fill(binMag[selectionIndex].begin(), binMag[selectionIndex].end(), 0.0f);
//    fftCounter = 0;
//}
//
////based on index value
//void FFTSpectrumAnalyzerAudioProcessor::removeSelection(int selectionIndex) {
//    binMag.erase(binMag.begin() + selectionIndex);
//}
//
//void FFTSpectrumAnalyzerAudioProcessor::clearAllSelections() {
//    binMag.clear();
//}
//
//void FFTSpectrumAnalyzerAudioProcessor::zeroAllSelections(int binMagSize, int selectionSize) {
//    binMag.resize(selectionSize, std::vector<float>(binMagSize, 0));
//}
//
//void FFTSpectrumAnalyzerAudioProcessor::prepSelection(int binMagSize, int selectionSize, int selectionIndex) {
//    binMag.resize(selectionSize, std::vector<float>(binMagSize));
//    zeroSelection(selectionIndex);
//    bufferLeft.resize(fftSize, 0.0f);
//    bufferRight.resize(fftSize, 0.0f);
//    windowBufferRight.resize(fftDataSize, 0.0f);
//    windowBufferLeft.resize(fftSize, 0.0f);
//}
//
//void FFTSpectrumAnalyzerAudioProcessor::prepBuffers(int fftSize) {
//    bufferLeft.resize(fftSize);
//    std::fill(bufferLeft.begin(), bufferLeft.end(), 0.0f);
//    bufferRight.resize(fftSize);
//    std::fill(bufferRight.begin(), bufferRight.end(), 0.0f);
//    windowBufferRight.resize(fftSize * 2);
//    std::fill(windowBufferRight.begin(), windowBufferRight.end(), 0.0f);
//    windowBufferLeft.resize(fftSize);
//    std::fill(windowBufferLeft.begin(), windowBufferLeft.end(), 0.0f);
//}

void FFTSpectrumAnalyzerAudioProcessor::clearAccumulationBuffer() {
    accumulationBuffer.clear();
}


void FFTSpectrumAnalyzerAudioProcessor::setRowIndex(int plotIndex) {
    rowIndex = plotIndex;
}

void FFTSpectrumAnalyzerAudioProcessor::setFFTSize(int newFFTSize) {
    fftSize = newFFTSize;
    stepSize = fftSize / 2;
    numBins = fftSize / 2 + 1;
    numFreqBins = fftSize / 2;
    fftDataSize = 2 * fftSize;
    //forwardFFT = juce::dsp::FFT(std::log2(fftSize));
}

void FFTSpectrumAnalyzerAudioProcessor::setStepSize(int stepS) {
    stepSize = stepS;
}

void FFTSpectrumAnalyzerAudioProcessor::setWindow(juce::dsp::WindowingFunction<float>::WindowingMethod type) {
    juce::dsp::WindowingFunction<float> window(fftSize, type);
    window.fillWindowingTables(fftSize, type);
}

//================================================PROCESS BLOCK====================================================================//
void FFTSpectrumAnalyzerAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    if (initialBlock == true && buffer.getNumSamples() < stepSize) {
        minBlockSize = false;
    }
    else {
        minBlockSize = true;
        sampleRate = getSampleRate();  //get the Sample Rate of Buffer

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

RingBuffer<float> FFTSpectrumAnalyzerAudioProcessor::getSampleBuffer() const
{
    return ringBuffer;
}

void FFTSpectrumAnalyzerAudioProcessor::setInitialBlock() {
    initialBlock = true;
}

void FFTSpectrumAnalyzerAudioProcessor::resetProcBlockCalled()
{
    procBlockCalled = false;
}

void FFTSpectrumAnalyzerAudioProcessor::clearRingBuffer() {
    ringBuffer.clear();
}

bool FFTSpectrumAnalyzerAudioProcessor::getProcBlockCalled()
{
    return procBlockCalled;
}

int FFTSpectrumAnalyzerAudioProcessor::getBlockSampleRate() const
{
    return sampleRate;
}

int FFTSpectrumAnalyzerAudioProcessor::getStepSize() const
{
    return stepSize;
}

int FFTSpectrumAnalyzerAudioProcessor::getFFTCounter() const
{
    return fftCounter;
}

//std::vector<float> FFTSpectrumAnalyzerAudioProcessor::getBinMag() const
//{
//    return binMag[rowIndex];
//}
//
//std::vector<std::vector<float>> FFTSpectrumAnalyzerAudioProcessor::getBinSet() const
//{
//    return binMag;
//}


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