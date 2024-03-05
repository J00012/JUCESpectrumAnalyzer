/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


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
    forwardFFT(fftOrder), // Initialize forwardFFT with fftOrder
    window(fftSize, juce::dsp::WindowingFunction<float>::hann) // Initialize window
    
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


//buffer- two dimenstional array where rows represent different channels and columns represent individual samples
//(A multi-channel buffer containing floating point audio samples)
//
//midiMessages
void FFTSpectrumAnalyzerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

   //loops through the output channels and clears a specified region of all the channels
    //for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        //buffer.clear (i, 0, buffer.getNumSamples());


    //TEST CODE !!!!!

    //just for mono
    int channel = 0;

    auto* channelData = buffer.getReadPointer(channel);

    /*for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
        fftArray[sample] = channelData[sample];
    }*/
    for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
        scopeData[sample] = channelData[sample];
    }
    
    //memcpy(fftData, fftArray, sizeof(fftArray));



    //// first apply a windowing function to our data
    //window.multiplyWithWindowingTable(fftData, fftSize);       // [1]

    //// then render our FFT data..
    //forwardFFT.performFrequencyOnlyForwardTransform(fftData);  // [2]

    //auto mindB = -100.0f;
    //auto maxdB = 0.0f;

    //for (int i = 0; i < scopeSize; ++i)                         // [3]
    //{
    //    auto skewedProportionX = 1.0f - std::exp(std::log(1.0f - (float)i / (float)scopeSize) * 0.2f);
    //    auto fftDataIndex = juce::jlimit(0, fftSize / 2, (int)(skewedProportionX * (float)fftSize * 0.5f));
    //    /*auto level = juce::jmap(juce::jlimit(mindB, maxdB, juce::Decibels::gainToDecibels(fftData[fftDataIndex])
    //        - juce::Decibels::gainToDecibels((float)fftSize)),
    //        mindB, maxdB, 0.0f, 1.0f);*/

    //    //scopeData[i] = skewedProportionX;
    //    //scopeData[i] = ;
    //    //scopeData[i] = level;                                   // [4]
    //    //scopeData[i] = 1.234;
    //}

}



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

float FFTSpectrumAnalyzerAudioProcessor::scopeData[] = { 0 };
