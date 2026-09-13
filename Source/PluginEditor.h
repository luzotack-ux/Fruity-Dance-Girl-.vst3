#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "SettingsPanel.h"

/**
    This is what Bitwig shows if you open the plugin's own panel (e.g. via
    the device chain). It's the same knob set as the floating settings
    window (opened by double-clicking the sprite), just embedded in the
    host's own window instead of a separate OS window - handy if the
    floating window ever ends up off-screen or hidden.
*/
class FruityDanceGirlEditor : public juce::AudioProcessorEditor
{
public:
    explicit FruityDanceGirlEditor (FruityDanceGirlProcessor&);

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    FruityDanceGirlProcessor& processorRef;
    SettingsPanel panel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FruityDanceGirlEditor)
};
