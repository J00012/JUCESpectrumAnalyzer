/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <string>

bool FFTSpectrumAnalyzerAudioProcessorEditor::isRunning = false;
//==============================================================================
FFTSpectrumAnalyzerAudioProcessorEditor::FFTSpectrumAnalyzerAudioProcessorEditor(FFTSpectrumAnalyzerAudioProcessor& p)
	: AudioProcessorEditor(&p), audioProcessor(p)
{
	setOpaque(true);
	setSize(1600, 1000);
	startTimer(500);
}

FFTSpectrumAnalyzerAudioProcessorEditor::~FFTSpectrumAnalyzerAudioProcessorEditor()
{
}

//==============================================================================
void FFTSpectrumAnalyzerAudioProcessorEditor::paint(juce::Graphics& g)
{
	g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
	g.setOpacity(1.0f);
	g.setColour(juce::Colours::white);

	int fftS = 1024;

	audioProcessor.setFFTSize(fftS);

	int sampleRate = audioProcessor.getBlockSampleRate();
	//std::string rate = std::to_string(sampleRate);

	int fftCounter = audioProcessor.getFFTCounter();
	//std::string counter = std::to_string(fftCounter);

	
	//g.drawText(counter, getLocalBounds(),juce::Justification::centred,true);

}

void FFTSpectrumAnalyzerAudioProcessorEditor::timerCallback()
{
	if (!isRunning && audioProcessor.getProcBlockCalled()) {
		isRunning = true;
		audioProcessor.resetProcBlockCalled();
	}
	else if (isRunning && !audioProcessor.getProcBlockCalled()) {
		isRunning = false;
		repaint();
		//audioProcessor.resetScopeDataIndex();
	}
}

void FFTSpectrumAnalyzerAudioProcessorEditor::resized()
{	
	// This is generally where you'll want to lay out the positions of any
	// This is generally where you'll want to lay out the positions of any
	// subcomponents in your editor..
}

int FFTSpectrumAnalyzerAudioProcessorEditor::fftSize = 1024;

