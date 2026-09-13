#pragma once
#include <JuceHeader.h>

/** Small, tidy rotary knobs with a soft glow arc - fits the settings panel. */
class NeonLookAndFeel : public juce::LookAndFeel_V4
{
public:
    NeonLookAndFeel();

    void drawRotarySlider (juce::Graphics&, int x, int y, int width, int height,
                            float sliderPosProportional, float rotaryStartAngle,
                            float rotaryEndAngle, juce::Slider&) override;

    juce::Font getLabelFont (juce::Label&) override;

    void drawComboBox (juce::Graphics&, int width, int height, bool isButtonDown,
                        int buttonX, int buttonY, int buttonW, int buttonH,
                        juce::ComboBox&) override;
};
