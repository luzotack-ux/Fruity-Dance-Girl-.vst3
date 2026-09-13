#include "OverlayWindow.h"
#include "PluginProcessor.h"
#include "SettingsPanel.h"

OverlayWindow::OverlayWindow (FruityDanceGirlProcessor& proc)
    : processor (proc)
    , spriteView (proc)
{
    setOpaque (false);
    addAndMakeVisible (spriteView);

    spriteView.onDoubleClick = [this] { toggleSettingsWindow(); };

    auto ideal = spriteView.getIdealSize();
    setSize (ideal.x, ideal.y);

    // Borderless, click-through-off, temporary (no taskbar entry) window,
    // placed directly on the desktop so it floats above the host and every
    // other application.
    addToDesktop (juce::ComponentPeer::windowIsTemporary | juce::ComponentPeer::windowIgnoresKeyPresses);
    setAlwaysOnTop (true);

    restoreSavedPosition();
    setVisible (true);
    toFront (false);
}

OverlayWindow::~OverlayWindow()
{
    settingsWindow.reset();
    removeFromDesktop();
}

void OverlayWindow::resized()
{
    spriteView.setBounds (getLocalBounds());
}

void OverlayWindow::moved()
{
    processor.windowX.store ((float) getX());
    processor.windowY.store ((float) getY());
}

void OverlayWindow::restoreSavedPosition()
{
    const float savedX = processor.windowX.load();
    const float savedY = processor.windowY.load();

    if (savedX >= 0.0f && savedY >= 0.0f)
    {
        setTopLeftPosition ((int) savedX, (int) savedY);
        return;
    }

    // First run: put her near the top-right corner of the main display.
    if (auto* display = juce::Desktop::getInstance().getDisplays().getPrimaryDisplay())
    {
        auto area = display->userArea;
        setTopLeftPosition (area.getRight() - getWidth() - 60, area.getY() + 60);
    }
}

void OverlayWindow::toggleSettingsWindow()
{
    if (settingsWindow == nullptr)
        settingsWindow = std::make_unique<SettingsWindow> (processor);
    else
        settingsWindow->setVisible (! settingsWindow->isVisible());

    if (settingsWindow->isVisible())
        settingsWindow->toFront (true);
}
