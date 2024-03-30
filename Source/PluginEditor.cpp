/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <string>


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

}

void FFTSpectrumAnalyzerAudioProcessorEditor::timerCallback()
{
}

void FFTSpectrumAnalyzerAudioProcessorEditor::resized()
{	
	// This is generally where you'll want to lay out the positions of any
	// This is generally where you'll want to lay out the positions of any
	// subcomponents in your editor..
}