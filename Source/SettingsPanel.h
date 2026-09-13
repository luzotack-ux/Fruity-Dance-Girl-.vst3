#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "NeonLookAndFeel.h"

/** One rotary knob + its caption, wired to an AudioProcessorValueTreeState parameter. */
struct LabelledKnob : public juce::Component
{
    LabelledKnob (juce::AudioProcessorValueTreeState& apvts, const juce::String& paramID, const juce::String& caption)
        : attachment (apvts, paramID, slider)
    {
        slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 60, 16);
        addAndMakeVisible (slider);

        label.setText (caption, juce::dontSendNotification);
        label.setJustificationType (juce::Justification::centred);
        label.setFont (juce::Font (12.5f));
        addAndMakeVisible (label);
    }

    void resized() override
    {
        auto b = getLocalBounds();
        label.setBounds (b.removeFromTop (16));
        slider.setBounds (b);
    }

    juce::Slider slider;
    juce::Label label;
    juce::AudioProcessorValueTreeState::SliderAttachment attachment;
};

/** One labelled row: caption on the left, a combo box filling the rest. */
struct LabelledCombo : public juce::Component
{
    LabelledCombo (juce::AudioProcessorValueTreeState& apvts, const juce::String& paramID, const juce::String& caption)
        : attachment (apvts, paramID, box)
    {
        if (auto* p = dynamic_cast<juce::AudioParameterChoice*> (apvts.getParameter (paramID)))
            box.addItemList (p->choices, 1);

        addAndMakeVisible (box);

        label.setText (caption, juce::dontSendNotification);
        label.setJustificationType (juce::Justification::centredLeft);
        label.setFont (juce::Font (13.0f));
        addAndMakeVisible (label);
    }

    void resized() override
    {
        auto b = getLocalBounds();
        label.setBounds (b.removeFromLeft (78));
        box.setBounds (b);
    }

    juce::Label label;
    juce::ComboBox box;
    juce::AudioProcessorValueTreeState::ComboBoxAttachment attachment;
};

/** The knob panel itself: Animation, Speed & Neon dropdowns, Blur Style,
    and Blur / Size knobs. Designed to be small and tidy. */
class SettingsPanel : public juce::Component
{
public:
    explicit SettingsPanel (FruityDanceGirlProcessor& processor);
    ~SettingsPanel() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    FruityDanceGirlProcessor& processor;
    NeonLookAndFeel lookAndFeel;

    juce::Label titleLabel;

    LabelledCombo animationCombo, speedCombo, neonCombo, styleCombo;
    LabelledKnob blurKnob, sizeKnob;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SettingsPanel)
};

/** Floating utility window that hosts the SettingsPanel. Purely cosmetic
    chrome (title bar + close button); closing it just hides it. */
class SettingsWindow : public juce::DocumentWindow
{
public:
    explicit SettingsWindow (FruityDanceGirlProcessor& processor);

    void closeButtonPressed() override;

private:
    SettingsPanel panel;
};
