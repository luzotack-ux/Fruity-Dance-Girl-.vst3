#include "PluginEditor.h"

FruityDanceGirlEditor::FruityDanceGirlEditor (FruityDanceGirlProcessor& p)
    : AudioProcessorEditor (&p)
    , processorRef (p)
    , panel (p)
{
    addAndMakeVisible (panel);
    setSize (panel.getWidth(), panel.getHeight());
    setResizable (false, false);
}

void FruityDanceGirlEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour::fromRGB (28, 28, 32));
}

void FruityDanceGirlEditor::resized()
{
    panel.setBounds (getLocalBounds());
}
