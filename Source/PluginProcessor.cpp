/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "RingBuffer.h"

//==============================================================================
FFTSpectrumAnalyzerAudioProcessor::FFTSpectrumAnalyzerAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
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

void FFTSpectrumAnalyzerAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String FFTSpectrumAnalyzerAudioProcessor::getProgramName (int index)
{
    return {};
}

void FFTSpectrumAnalyzerAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void FFTSpectrumAnalyzerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
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
bool FFTSpectrumAnalyzerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
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


void FFTSpectrumAnalyzerAudioProcessor::setFFTSize(int newFFTSize) {
    fftSize = newFFTSize;
    stepSize = fftSize / 2;
    numBins = fftSize / 2 + 1;
    numFreqBins = fftSize / 2;
    fftDataSize = 2 * fftSize;

    bufferLeft.resize(fftSize, 0.0f);
    bufferRight.resize(fftSize, 0.0f);
    windowBufferRight.resize(fftDataSize, 0.0f);
    windowBufferLeft.resize(fftSize, 0.0f);
    bins.resize(numBins,0.0f);
    forwardFFT = juce::dsp::FFT(std::log2(fftSize));
}

void FFTSpectrumAnalyzerAudioProcessor::setWindow(juce::dsp::WindowingFunction<float>::WindowingMethod type) {
    juce::dsp::WindowingFunction<float> window(fftSize, type);
    window.fillWindowingTables(fftSize, type);
}

//PROCESS BLOCK
void FFTSpectrumAnalyzerAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    sampleRate = getSampleRate();  //get the Sample Rate of Buffer

    int channel = 0;          //set channel
    int sampleOutIndex = 0;   //accumulate counter

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    auto* channelData = buffer.getWritePointer(channel);

    ringBuffer.write(channelData, buffer.getNumSamples());
  
    //juce::dsp::WindowingFunction<float> window(fftSize, juce::dsp::WindowingFunction<float>::hann);   //declare the window object
    //window.fillWindowingTables(fftSize, juce::dsp::WindowingFunction<float>::hann);                   //fills the content of the object array with a given windowing method
                       
    while (ringBuffer.size() >= stepSize){

        std::copy(bufferLeft.begin(), bufferLeft.begin()+stepSize, bufferLeft.begin()+stepSize);
        std::copy(bufferRight.begin()+stepSize, bufferRight.end(), bufferLeft.begin());
        std::copy(bufferRight.begin(), bufferRight.begin()+stepSize, bufferRight.begin() + stepSize);
       
        ringBuffer.read(bufferRight.data(), stepSize);

        std::copy(bufferRight.begin(), bufferRight.end(), windowBufferRight.begin());

        windowBufferLeft = bufferLeft;

		window.multiplyWithWindowingTable(windowBufferRight.data(), fftSize);

		window.multiplyWithWindowingTable(windowBufferLeft.data(), fftSize);

		forwardFFT.performRealOnlyForwardTransform(windowBufferRight.data(), true);

        fftCounter++;

		for (int i = 0; i < numBins-1; i++) { 
            float a= sqrt(pow(windowBufferRight[2 * i], 2) + pow(windowBufferRight[2 * i + 1], 2)) / numFreqBins;
            channelData[sampleOutIndex] = a;
            bins[i] += a;
            sampleOutIndex++;
		}
	} 
    procBlockCalled = true;
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

int FFTSpectrumAnalyzerAudioProcessor::getStepSize() const
{
    return stepSize;
}

int FFTSpectrumAnalyzerAudioProcessor::getFFTCounter() const
{
    return fftCounter;
}

const float* FFTSpectrumAnalyzerAudioProcessor::getBins() const
{
    return bins.data();
}


//==============================================================================
bool FFTSpectrumAnalyzerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* FFTSpectrumAnalyzerAudioProcessor::createEditor()
{
    return new FFTSpectrumAnalyzerAudioProcessorEditor (*this);
}

//==============================================================================
void FFTSpectrumAnalyzerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data. 
}

void FFTSpectrumAnalyzerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
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

//zero the static variables
int FFTSpectrumAnalyzerAudioProcessor::sampleRate = 0;
int FFTSpectrumAnalyzerAudioProcessor::fftCounter = 0;

int FFTSpectrumAnalyzerAudioProcessor::fftSize=0;
int FFTSpectrumAnalyzerAudioProcessor::stepSize=0;
int FFTSpectrumAnalyzerAudioProcessor::numBins=0;
int FFTSpectrumAnalyzerAudioProcessor::numFreqBins=0;
int FFTSpectrumAnalyzerAudioProcessor::fftDataSize = 0;

juce::dsp::FFT FFTSpectrumAnalyzerAudioProcessor::forwardFFT(0);

// Define static member variables
std::vector<float> FFTSpectrumAnalyzerAudioProcessor::bufferRight = { 0 };
std::vector<float> FFTSpectrumAnalyzerAudioProcessor::bufferLeft = { 0 };
std::vector<float> FFTSpectrumAnalyzerAudioProcessor::windowBufferRight = { 0 };
std::vector<float> FFTSpectrumAnalyzerAudioProcessor::windowBufferLeft = { 0 };
std::vector<float> FFTSpectrumAnalyzerAudioProcessor::bins = { 0 };

juce::dsp::WindowingFunction<float> FFTSpectrumAnalyzerAudioProcessor::window(0, juce::dsp::WindowingFunction<float>::blackman);

float FFTSpectrumAnalyzerAudioProcessor::ringTest[] = { 0 };