/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
GraphicsSampleTestAudioProcessorEditor::GraphicsSampleTestAudioProcessorEditor (GraphicsSampleTestAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (500, 500);
}

GraphicsSampleTestAudioProcessorEditor::~GraphicsSampleTestAudioProcessorEditor()
{
}

//==============================================================================
void GraphicsSampleTestAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colours::lightblue);

    g.setColour(juce::Colours::darkblue);
    g.setFont(20.0f);
    g.setFont(juce::Font("Times New Roman", 20.0f, juce::Font::italic));
    g.drawText("Hello, World!", 20, 40, 200, 40, juce::Justification::centred, true);
    g.setColour(juce::Colours::green);
    g.drawLine(10, 300, 590, 300, 5);



    juce::Rectangle<float> house(300, 120, 200, 170);
    g.fillCheckerBoard(house, 30, 10, juce::Colours::sandybrown, juce::Colours::saddlebrown);
    g.setColour(juce::Colours::yellow);
    g.drawEllipse(530, 10, 60, 60, 3);

  
    

  
}

void GraphicsSampleTestAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
