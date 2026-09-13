#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "OverlayWindow.h"

FruityDanceGirlProcessor::FruityDanceGirlProcessor()
    : AudioProcessor (BusesProperties()
                        .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                        .withOutput ("Output", juce::AudioChannelSet::stereo(), true))
    , apvts (*this, nullptr, "PARAMETERS", createParameterLayout())
{
    // Create the floating sprite window once the message loop is free.
    // (Some hosts instantiate plugins off the message thread during scanning,
    // so we defer this rather than doing it directly in the constructor.)
    juce::MessageManager::callAsync ([this] { ensureOverlayWindowExists(); });
}

FruityDanceGirlProcessor::~FruityDanceGirlProcessor()
{
    overlayWindow.reset();
}

juce::AudioProcessorValueTreeState::ParameterLayout FruityDanceGirlProcessor::createParameterLayout()
{
    juce::StringArray animationChoices {
        "Row 1", "Row 2", "Row 3", "Row 4", "Row 5",
        "Row 6", "Row 7", "Row 8", "Row 9", "Row 10",
        "Random"
    };

    juce::StringArray neonChoices {
        "Off", "Red", "Raspberry", "Magenta", "Purple",
        "Blue", "Cyan", "Green", "Yellow", "White"
    };

    juce::StringArray blurStyleChoices { "Glow / Blur", "Motion Trail" };

    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back (std::make_unique<juce::AudioParameterChoice> (
        paramAnimation, "Animation", animationChoices, 0));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        paramBlur, "Blur",
        juce::NormalisableRange<float> (0.0f, 20.0f, 0.1f), 0.0f,
        "px"));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        paramSize, "Girl Size",
        juce::NormalisableRange<float> (40.0f, 800.0f, 1.0f), 200.0f,
        "px"));

    params.push_back (std::make_unique<juce::AudioParameterChoice> (
        paramSpeed, "Speed", AnimationEngine::getSpeedChoiceLabels(), 2)); // default "1x"

    params.push_back (std::make_unique<juce::AudioParameterChoice> (
        paramNeon, "Neon Glow", neonChoices, 0));

    params.push_back (std::make_unique<juce::AudioParameterChoice> (
        paramBlurStyle, "Blur Style", blurStyleChoices, 0));

    return { params.begin(), params.end() };
}

void FruityDanceGirlProcessor::ensureOverlayWindowExists()
{
    JUCE_ASSERT_MESSAGE_MANAGER_IS_LOCKED
    if (overlayWindow == nullptr)
        overlayWindow = std::make_unique<OverlayWindow> (*this);
}

void FruityDanceGirlProcessor::prepareToPlay (double, int) {}
void FruityDanceGirlProcessor::releaseResources() {}

bool FruityDanceGirlProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    // Accept anything mono/stereo in = same out, so it behaves on any
    // channel including the master bus.
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    return true;
}

void FruityDanceGirlProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    // Pure pass-through: we never touch the audio. This plugin is a visual
    // toy, safe to drop anywhere including the master channel.
    juce::ignoreUnused (buffer);

    bool playing = false;
    double bpm = 120.0;
    double ppq = 0.0;

    if (auto* playHead = getPlayHead())
    {
        if (auto position = playHead->getPosition())
        {
            playing = position->getIsPlaying();
            if (auto bpmOpt = position->getBpm())
                bpm = *bpmOpt;
            if (auto ppqOpt = position->getPpqPosition())
                ppq = *ppqOpt;
        }
    }

    animationEngine.updateTransport (playing, bpm, ppq);

    // *** This is the bit that was missing before: actually forward the
    // Animation / Speed knob values into the engine that picks frames. ***
    if (auto* animParam = apvts.getRawParameterValue (paramAnimation))
        animationEngine.setAnimationChoice ((int) animParam->load());

    if (auto* speedParam = apvts.getRawParameterValue (paramSpeed))
        animationEngine.setSpeedMultiplier (AnimationEngine::speedChoiceToMultiplier ((int) speedParam->load()));
}

juce::AudioProcessorEditor* FruityDanceGirlProcessor::createEditor()
{
    return new FruityDanceGirlEditor (*this);
}

void FruityDanceGirlProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    auto xml = state.createXml();

    xml->setAttribute ("windowX", (double) windowX.load());
    xml->setAttribute ("windowY", (double) windowY.load());

    copyXmlToBinary (*xml, destData);
}

void FruityDanceGirlProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary (data, sizeInBytes))
    {
        if (xml->hasTagName (apvts.state.getType()))
        {
            windowX.store ((float) xml->getDoubleAttribute ("windowX", -1.0));
            windowY.store ((float) xml->getDoubleAttribute ("windowY", -1.0));

            apvts.replaceState (juce::ValueTree::fromXml (*xml));
        }
    }
}

// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FruityDanceGirlProcessor();
}
