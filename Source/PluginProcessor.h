#pragma once
#include <JuceHeader.h>
#include "AnimationEngine.h"

class OverlayWindow;

/**
    Fruity Dance Girl — audio passes through completely untouched.
    This is a visual "toy" plugin: drop it on the master channel and a
    little sprite window floats on top of everything, dancing in time
    with the host tempo.
*/
class FruityDanceGirlProcessor : public juce::AudioProcessor
{
public:
    FruityDanceGirlProcessor();
    ~FruityDanceGirlProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }

    bool acceptsMidi() const override  { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // Parameter IDs
    static constexpr auto paramAnimation = "animation";
    static constexpr auto paramBlur      = "blur";
    static constexpr auto paramSize      = "size";
    static constexpr auto paramSpeed     = "speed";
    static constexpr auto paramNeon      = "neon";
    static constexpr auto paramBlurStyle = "blurStyle"; // "Glow / Blur" or "Motion Trail"

    juce::AudioProcessorValueTreeState apvts;

    AnimationEngine animationEngine;

    // Floating window position, persisted in state but not host-automatable.
    std::atomic<float> windowX { -1.0f }; // -1 = "not set yet, centre on screen"
    std::atomic<float> windowY { -1.0f };

    // The always-on-top sprite window. Owned by the processor so it lives
    // for as long as the plugin instance exists, independent of whether the
    // host has the plugin's editor open.
    std::unique_ptr<OverlayWindow> overlayWindow;

    void ensureOverlayWindowExists();

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FruityDanceGirlProcessor)
};
