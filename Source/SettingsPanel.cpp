#include "SettingsPanel.h"

SettingsPanel::SettingsPanel (FruityDanceGirlProcessor& proc)
    : processor (proc)
    , animationCombo (proc.apvts, FruityDanceGirlProcessor::paramAnimation, "Animation")
    , speedCombo     (proc.apvts, FruityDanceGirlProcessor::paramSpeed,     "Speed")
    , neonCombo      (proc.apvts, FruityDanceGirlProcessor::paramNeon,      "Neon Glow")
    , styleCombo     (proc.apvts, FruityDanceGirlProcessor::paramBlurStyle, "Effect")
    , blurKnob       (proc.apvts, FruityDanceGirlProcessor::paramBlur,      "Blur")
    , sizeKnob       (proc.apvts, FruityDanceGirlProcessor::paramSize,      "Size")
{
    setLookAndFeel (&lookAndFeel);

    titleLabel.setText ("Fruity Dance Girl", juce::dontSendNotification);
    titleLabel.setFont (juce::Font (16.0f, juce::Font::bold));
    titleLabel.setJustificationType (juce::Justification::centred);
    titleLabel.setColour (juce::Label::textColourId, juce::Colour::fromRGB (255, 170, 90));
    addAndMakeVisible (titleLabel);

    addAndMakeVisible (animationCombo);
    addAndMakeVisible (speedCombo);
    addAndMakeVisible (neonCombo);
    addAndMakeVisible (styleCombo);
    addAndMakeVisible (blurKnob);
    addAndMakeVisible (sizeKnob);

    setSize (300, 288);
}

SettingsPanel::~SettingsPanel()
{
    setLookAndFeel (nullptr);
}

void SettingsPanel::paint (juce::Graphics& g)
{
    auto b = getLocalBounds().toFloat();
    g.setGradientFill (juce::ColourGradient (
        juce::Colour::fromRGB (32, 32, 37), b.getTopLeft(),
        juce::Colour::fromRGB (22, 22, 26), b.getBottomLeft(), false));
    g.fillAll();

    g.setColour (juce::Colour::fromRGB (90, 200, 130).withAlpha (0.5f));
    g.drawHorizontalLine (34, 16.0f, b.getWidth() - 16.0f);
}

void SettingsPanel::resized()
{
    auto b = getLocalBounds().reduced (16);

    titleLabel.setBounds (b.removeFromTop (26));
    b.removeFromTop (14);

    auto row = [&b] (int height) { auto r = b.removeFromTop (height); b.removeFromTop (8); return r; };

    animationCombo.setBounds (row (24));
    speedCombo.setBounds (row (24));
    neonCombo.setBounds (row (24));
    styleCombo.setBounds (row (24));

    b.removeFromTop (6);

    auto knobRow = b.removeFromTop (110);
    const int half = knobRow.getWidth() / 2;
    blurKnob.setBounds (knobRow.removeFromLeft (half).reduced (10, 0));
    sizeKnob.setBounds (knobRow.reduced (10, 0));
}

//==============================================================================
SettingsWindow::SettingsWindow (FruityDanceGirlProcessor& processor)
    : DocumentWindow ("Fruity Dance Girl", juce::Colour::fromRGB (22, 22, 26),
                       DocumentWindow::closeButton, true)
    , panel (processor)
{
    setUsingNativeTitleBar (true);
    setContentNonOwned (&panel, true);
    setResizable (false, false);
    centreWithSize (panel.getWidth(), panel.getHeight());
    setAlwaysOnTop (true);
    setVisible (true);
}

void SettingsWindow::closeButtonPressed()
{
    setVisible (false);
}
