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
                       ),
    forwardFFT(fftOrder) // Initialize forwardFFT with fftOrder
    
#endif
{
}

FFTSpectrumAnalyzerAudioProcessor::~FFTSpectrumAnalyzerAudioProcessor()
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

void FFTSpectrumAnalyzerAudioProcessor::resetProcBlockIsRunning() 
{
    procBlockIsRunning = false;
}

bool FFTSpectrumAnalyzerAudioProcessor::getProcBlockIsRunning()
{
    return procBlockIsRunning;
}


//buffer- two dimenstional array where rows represent different channels and columns represent individual samples
//(A multi-channel buffer containing floating point audio samples)
//
//midiMessages
void FFTSpectrumAnalyzerAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    int sampleRate = getSampleRate();  //get the Sample Rate of Buffer

    int channel = 0;          //set channel
    int sampleOutIndex = 0;   //accumulate counter

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    auto* channelData = buffer.getWritePointer(channel);

    ringBuffer.write(channelData, buffer.getNumSamples());
  
    juce::dsp::WindowingFunction<float> window(fftSize, juce::dsp::WindowingFunction<float>::hann);   //declare the window object
    window.fillWindowingTables(fftSize, juce::dsp::WindowingFunction<float>::hann);                   //fills the content of the object array with a given windowing method
                       
    //buffer that is start at 0 (loop backwards) for loop that counts down ) 512 from the end 3*512
    //do the window for the first 1024
    //do the window on the first 1024 (copy over in another buffer) (copy second in another buffer)  and the last 1024
    //memcpy

    while (ringBuffer.size() >= stepSize){

		for (int sample = 0; sample < stepSize; ++sample) {
			bufferLeft[sample + stepSize] = bufferLeft[sample];
		}
		for (int sample = 0; sample < stepSize; ++sample) {
			bufferLeft[sample] = bufferRight[sample + stepSize];
		}
		for (int sample = 0; sample < stepSize; ++sample) {
			bufferRight[sample + stepSize] = bufferRight[sample];
		}
        ringBuffer.read(bufferRight, stepSize);

		//copy for windowing
		for (int sample = 0; sample < fftSize; ++sample) {
			windowBufferRight[sample] = bufferRight[sample];
		}
		for (int sample = 0; sample < fftSize; ++sample) {
			windowBufferLeft[sample] = bufferLeft[sample];
		}

		window.multiplyWithWindowingTable(windowBufferRight, fftSize);

		window.multiplyWithWindowingTable(windowBufferLeft, fftSize);

		forwardFFT.performRealOnlyForwardTransform(windowBufferRight,true);

        fftCounter++;

		for (int i = 0; i < numBins-1; i++) {
            float a= sqrt(pow(windowBufferRight[2 * i], 2) + pow(windowBufferRight[2 * i + 1], 2)) / numFreqBins;
            channelData[sampleOutIndex] = a;
            bins[i] += a;
			sampleOutIndex++;
		}
	} 
}


int FFTSpectrumAnalyzerAudioProcessor::getStepSize() const
{
    return stepSize;
}


const float* FFTSpectrumAnalyzerAudioProcessor::getScopeData() const
{
    return scopeData;
}


const float* FFTSpectrumAnalyzerAudioProcessor::getRingTest() const
{
    return ringTest;
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

//zero the static float arrays
float FFTSpectrumAnalyzerAudioProcessor::ringTest[] = { 0 };
float FFTSpectrumAnalyzerAudioProcessor::readBuffer[] = { 0 };
float FFTSpectrumAnalyzerAudioProcessor::scopeData[] = { 0 };
float FFTSpectrumAnalyzerAudioProcessor::bufferRight[] = { 0 };
float FFTSpectrumAnalyzerAudioProcessor::bufferLeft[] = { 0 };
float FFTSpectrumAnalyzerAudioProcessor::windowBufferRight[] = { 0 };
float FFTSpectrumAnalyzerAudioProcessor::windowBufferLeft[] = { 0 };
float FFTSpectrumAnalyzerAudioProcessor::indexFreqMap[] = { 0 };
float FFTSpectrumAnalyzerAudioProcessor::bins[] = { 0 };
