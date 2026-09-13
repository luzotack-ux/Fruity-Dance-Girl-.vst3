#pragma once
#include <JuceHeader.h>
#include "SpriteView.h"

class FruityDanceGirlProcessor;
class SettingsWindow;

/**
    A borderless, always-on-top, click-and-drag window that sits directly on
    the OS desktop (not embedded in the host's plugin-editor window), so the
    dancing sprite stays visible above the DAW and any other application.

    Double-clicking the sprite opens/raises the SettingsWindow with all the
    controls (animation, blur, size, speed, neon colour).
*/
class OverlayWindow : public juce::Component
{
public:
    explicit OverlayWindow (FruityDanceGirlProcessor& processor);
    ~OverlayWindow() override;

    void resized() override;
    void moved() override;

    void toggleSettingsWindow();

private:
    FruityDanceGirlProcessor& processor;
    SpriteView spriteView;
    std::unique_ptr<SettingsWindow> settingsWindow;

    void restoreSavedPosition();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OverlayWindow)
};
