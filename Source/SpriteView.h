#pragma once
#include <JuceHeader.h>
#include <unordered_map>
#include <deque>
#include "PluginProcessor.h"

/**
    Draws the currently-active sprite-sheet cell, scaled to the user's chosen
    size, with an optional coloured neon glow/blur or a "motion trail" of
    her last few poses behind her.

    Everything expensive (cropping, tinting, Gaussian blur) is cached per
    (row, column, size, blur, neon colour) combination and only recomputed
    when one of those actually changes - dragging the window around the
    screen, or just letting the same pose sit there, costs almost nothing.

    Also handles:
      - click + drag  -> moves the parent (floating) window
      - double click   -> notifies onDoubleClick, used to open the settings panel
*/
class SpriteView : public juce::Component, private juce::Timer
{
public:
    explicit SpriteView (FruityDanceGirlProcessor& proc);
    ~SpriteView() override;

    void paint (juce::Graphics& g) override;

    void mouseDown (const juce::MouseEvent& e) override;
    void mouseDrag (const juce::MouseEvent& e) override;
    void mouseDoubleClick (const juce::MouseEvent& e) override;

    /** Returns the ideal component size in pixels for the current "Girl Size" parameter. */
    juce::Point<int> getIdealSize() const;

    std::function<void()> onDoubleClick;

private:
    void timerCallback() override;
    juce::Colour getNeonColour (int choiceIndex) const;

    // Cheap: crop + rescale only, no per-pixel work.
    const juce::Image& getScaledFrame (AnimationEngine::Cell cell, int w, int h);

    // Expensive: tinted + Gaussian-blurred silhouette. Cached per cell so it
    // is only paid for once per pose, not once per repaint.
    const juce::Image& getGlow (AnimationEngine::Cell cell, const juce::Image& frame,
                                 float blurAmount, int neonChoice, int w, int h);

    // Plain (colourless) blur of the actual sprite - separate cache since it
    // blurs the real artwork, not a tinted silhouette.
    const juce::Image& getBlurredFrame (AnimationEngine::Cell cell, const juce::Image& frame,
                                          float blurAmount, int w, int h);

    FruityDanceGirlProcessor& processor;
    juce::Image spriteSheet;

    juce::ComponentDragger dragger;

    // --- caches ---
    std::unordered_map<int, juce::Image> frameCache;
    int frameCacheW = -1, frameCacheH = -1;

    std::unordered_map<int, juce::Image> glowCache;
    int glowCacheW = -1, glowCacheH = -1;
    float glowCacheBlur = -1.0f;
    int glowCacheNeon = -1;

    std::unordered_map<int, juce::Image> blurFrameCache;
    int blurFrameCacheW = -1, blurFrameCacheH = -1;
    float blurFrameCacheBlur = -1.0f;

    // --- motion trail history (poses she has actually been in recently) ---
    std::deque<AnimationEngine::Cell> trailHistory;
    AnimationEngine::Cell lastCell { -1, -1 };
    static constexpr int maxTrailLength = 5;

    static constexpr int cellWidth  = 220;
    static constexpr int cellHeight = 256;

    static int cellKey (AnimationEngine::Cell c) { return c.row * 100 + c.column; }
};
