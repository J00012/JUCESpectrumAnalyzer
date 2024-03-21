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
    //!modify the buffer so it outputs on the audacity window! (optional apply gain)
    //! 
    //! apply window on the full buffer on both
    //! apply a window everytime then add to a seperate buffer using the right and the left together
    //two 1024 window buffers (each 512)
    //add the result of the right and left 
    //make final result a scope variable and will be used to modify the buffer info

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();


    //just for mono
    int channel = 0;

    //change this back to Read if you dont want to modify the data
    auto* channelData = buffer.getWritePointer(channel);

    //ringBuffer.write(channelData, buffer.getNumSamples());
    //ringBuffer.read(ringTest, buffer.getNumSamples());
    //if(ringBuffer.size() < 512)
    //if(ringBuffer.size() >= 512)

    //buffer that is start at 0 (loop backwards) for loop that counts down ) 512 from the end 3*512
    //do the window for the first 1024
    //do the window on the first 1024 (copy over in another buffer) (copy second in another buffer)  and the last 1024
    //memcpy

   //scuffed looping to shift the data over
    for (int sample = 0; sample < 512; ++sample) {
        bufferLeft[sample + 512] = bufferLeft[sample];
    }
    for (int sample = 0; sample < 512; ++sample) {
        bufferLeft[sample] = bufferRight[sample + 512];
    }
    for (int sample = 0; sample < 512; ++sample) {
        bufferRight[sample + 512] = bufferRight[sample];
    }
    for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
        bufferRight[sample] = channelData[sample];
    }

    memcpy(windowBufferRight, bufferRight, 1024*sizeof(float));
    memcpy(windowBufferLeft, bufferLeft, 1024 * sizeof(float));


    //apply windowing
      //make a new dataType for the enum in JUCE and string for selection
    struct WindowToName {
        juce::dsp::WindowingFunction<float>::WindowingMethod window;
        std::string name;
    };

    //make an array of window type with the enum value and the string value
    WindowToName windowToName[] = {
        {juce::dsp::WindowingFunction<float>::hann,"Hann"},
        {juce::dsp::WindowingFunction<float>::blackman,"Blackman"},
        {juce::dsp::WindowingFunction<float>::rectangular,"Rectangular"},
        {juce::dsp::WindowingFunction<float>::blackmanHarris,"BlackmanHarris"},
        {juce::dsp::WindowingFunction<float>::hamming,"Hamming"},
        {juce::dsp::WindowingFunction<float>::triangular,"Triangular"},
        {juce::dsp::WindowingFunction<float>::flatTop,"FlatTop"},
        {juce::dsp::WindowingFunction<float>::kaiser,"Kaiser"},
    };


    WindowToName& selectedWindow = windowToName[0];                               //set selectedWindow to a variable Name
    juce::dsp::WindowingFunction<float> window(1024, selectedWindow.window);   //declare the window object
    window.fillWindowingTables(1024, selectedWindow.window);                   //fills the content of the object array with a given windowing method
    //window.multiplyWithWindowingTable(ringTest, scopeSize);                        //applies the windowing fucntion to the audio data stored in fftData


    window.multiplyWithWindowingTable(windowBufferRight, 1024);
    window.multiplyWithWindowingTable(windowBufferLeft, 1024);

    //add the right and the left of the two buffer together and add them to the selection
    for (int i = 0; i < buffer.getNumSamples(); ++i) {
        channelData[i] = windowBufferRight[i + 512] + windowBufferLeft[i];
    }
    
    //forwardFFT.performFrequencyOnlyForwardTransform(scopeData);


/*
    procBlockIsRunning = true;
    
    memcpy(fftData, fftArray, sizeof(fftArray));


    // then render our FFT data..
    forwardFFT.performFrequencyOnlyForwardTransform(fftData);  // [2]

    auto mindB = -100.0f;
    auto maxdB = 0.0f;

    for (int i = 0; i < scopeSize; ++i)                         // [3]
    {
        auto skewedProportionX = 1.0f - std::exp(std::log(1.0f - (float)i / (float)scopeSize) * 0.2f);
        auto fftDataIndex = juce::jlimit(0, fftSize / 2, (int)(skewedProportionX * (float)fftSize * 0.5f));
        /*auto level = juce::jmap(juce::jlimit(mindB, maxdB, juce::Decibels::gainToDecibels(fftData[fftDataIndex])
            - juce::Decibels::gainToDecibels((float)fftSize)),
            mindB, maxdB, 0.0f, 1.0f);*/

        //scopeData[i] = skewedProportionX;
        //scopeData[i] = ;
        //scopeData[i] = level;                                   // [4]
        //scopeData[i] = 1.234;
   // }
    
   
}


//void FFTSpectrumAnalyzerAudioProcessor::applyWindow() {

//}

int FFTSpectrumAnalyzerAudioProcessor::getScopeSize() const
{
    return scopeSize;
}


const float* FFTSpectrumAnalyzerAudioProcessor::getScopeData() const
{
    return scopeData;
}

const float* FFTSpectrumAnalyzerAudioProcessor::getFFT() const
{
    return fftData;
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
float FFTSpectrumAnalyzerAudioProcessor::ringTest[] = { 0 };

float FFTSpectrumAnalyzerAudioProcessor::scopeData[] = { 0 };

float FFTSpectrumAnalyzerAudioProcessor::bufferRight[] = { 0 };
float FFTSpectrumAnalyzerAudioProcessor::bufferLeft[] = { 0 };
float FFTSpectrumAnalyzerAudioProcessor::windowBufferRight[] = { 0 };
float FFTSpectrumAnalyzerAudioProcessor::windowBufferLeft[] = { 0 };
float FFTSpectrumAnalyzerAudioProcessor::windowBufferResult[] = { 0 };
