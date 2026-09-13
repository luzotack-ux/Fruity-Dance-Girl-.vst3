#include "SpriteView.h"
#include "BinaryData.h"

SpriteView::SpriteView (FruityDanceGirlProcessor& proc)
    : processor (proc)
{
    spriteSheet = juce::ImageCache::getFromMemory (BinaryData::spritesheet_png,
                                                     BinaryData::spritesheet_pngSize);
    setOpaque (false);
    setInterceptsMouseClicks (true, false);
    startTimerHz (30);
}

SpriteView::~SpriteView()
{
    stopTimer();
}

juce::Point<int> SpriteView::getIdealSize() const
{
    auto* sizeParam = processor.apvts.getRawParameterValue (FruityDanceGirlProcessor::paramSize);
    auto* blurParam = processor.apvts.getRawParameterValue (FruityDanceGirlProcessor::paramBlur);

    const float targetWidth = sizeParam != nullptr ? sizeParam->load() : 200.0f;
    const float aspect = (float) cellHeight / (float) cellWidth;
    const float targetHeight = targetWidth * aspect;

    const float blur = blurParam != nullptr ? blurParam->load() : 0.0f;
    const int margin = (int) std::ceil (blur) * 3 + 4;

    return { (int) targetWidth + margin * 2, (int) targetHeight + margin * 2 };
}

juce::Colour SpriteView::getNeonColour (int choiceIndex) const
{
    switch (choiceIndex)
    {
        case 1: return juce::Colour::fromRGB (255, 40, 40);   // Red
        case 2: return juce::Colour::fromRGB (255, 45, 110);  // Raspberry
        case 3: return juce::Colour::fromRGB (255, 0, 200);   // Magenta
        case 4: return juce::Colour::fromRGB (170, 40, 255);  // Purple
        case 5: return juce::Colour::fromRGB (40, 120, 255);  // Blue
        case 6: return juce::Colour::fromRGB (40, 230, 255);  // Cyan
        case 7: return juce::Colour::fromRGB (60, 255, 90);   // Green
        case 8: return juce::Colour::fromRGB (255, 235, 40);  // Yellow
        case 9: return juce::Colours::white;                  // White
        default: return juce::Colours::transparentBlack;      // Off
    }
}

const juce::Image& SpriteView::getScaledFrame (AnimationEngine::Cell cell, int w, int h)
{
    if (w != frameCacheW || h != frameCacheH)
    {
        frameCache.clear();
        frameCacheW = w;
        frameCacheH = h;
    }

    const int key = cellKey (cell);
    auto it = frameCache.find (key);
    if (it != frameCache.end())
        return it->second;

    juce::Image frame (juce::Image::ARGB, w, h, true);
    {
        juce::Graphics fg (frame);
        fg.drawImage (spriteSheet, 0, 0, w, h,
                      cell.column * cellWidth, cell.row * cellHeight, cellWidth, cellHeight);
    }

    return frameCache.emplace (key, std::move (frame)).first->second;
}

const juce::Image& SpriteView::getGlow (AnimationEngine::Cell cell, const juce::Image& frame,
                                          float blurAmount, int neonChoice, int w, int h)
{
    if (w != glowCacheW || h != glowCacheH || blurAmount != glowCacheBlur || neonChoice != glowCacheNeon)
    {
        glowCache.clear();
        glowCacheW = w;
        glowCacheH = h;
        glowCacheBlur = blurAmount;
        glowCacheNeon = neonChoice;
    }

    const int key = cellKey (cell);
    auto it = glowCache.find (key);
    if (it != glowCache.end())
        return it->second;

    // Solid-colour silhouette, using the frame's own alpha shape as a clip mask
    // (much cheaper than a manual per-pixel loop).
    juce::Image silhouette (juce::Image::ARGB, w, h, true);
    {
        juce::Graphics sg (silhouette);
        sg.reduceClipRegion (frame, {});
        sg.fillAll (getNeonColour (neonChoice));
    }

    const int kernelRadius = juce::jlimit (1, 31, (int) std::ceil (blurAmount));
    const int kernelSize = kernelRadius * 2 + 1;
    juce::ImageConvolutionKernel kernel (kernelSize);
    kernel.createGaussianBlur ((float) kernelRadius);

    juce::Image blurred (juce::Image::ARGB, w, h, true);
    kernel.applyToImage (blurred, silhouette, silhouette.getBounds());

    return glowCache.emplace (key, std::move (blurred)).first->second;
}

const juce::Image& SpriteView::getBlurredFrame (AnimationEngine::Cell cell, const juce::Image& frame,
                                                  float blurAmount, int w, int h)
{
    if (w != blurFrameCacheW || h != blurFrameCacheH || blurAmount != blurFrameCacheBlur)
    {
        blurFrameCache.clear();
        blurFrameCacheW = w;
        blurFrameCacheH = h;
        blurFrameCacheBlur = blurAmount;
    }

    const int key = cellKey (cell);
    auto it = blurFrameCache.find (key);
    if (it != blurFrameCache.end())
        return it->second;

    const int kernelRadius = juce::jlimit (1, 31, (int) std::ceil (blurAmount));
    const int kernelSize = kernelRadius * 2 + 1;
    juce::ImageConvolutionKernel kernel (kernelSize);
    kernel.createGaussianBlur ((float) kernelRadius);

    juce::Image blurred (juce::Image::ARGB, w, h, true);
    kernel.applyToImage (blurred, frame, frame.getBounds());

    return blurFrameCache.emplace (key, std::move (blurred)).first->second;
}

void SpriteView::paint (juce::Graphics& g)
{
    if (spriteSheet.isNull())
        return;

    auto cell = processor.animationEngine.getCurrentCell();

    if (cell != lastCell)
    {
        trailHistory.push_front (lastCell.row >= 0 ? lastCell : cell);
        if ((int) trailHistory.size() > maxTrailLength)
            trailHistory.pop_back();
        lastCell = cell;
    }

    auto* sizeParam = processor.apvts.getRawParameterValue (FruityDanceGirlProcessor::paramSize);
    auto* blurParam = processor.apvts.getRawParameterValue (FruityDanceGirlProcessor::paramBlur);
    auto* neonParam = processor.apvts.getRawParameterValue (FruityDanceGirlProcessor::paramNeon);
    auto* styleParam = processor.apvts.getRawParameterValue (FruityDanceGirlProcessor::paramBlurStyle);

    const float targetWidth  = sizeParam != nullptr ? sizeParam->load() : 200.0f;
    const float aspect       = (float) cellHeight / (float) cellWidth;
    const float targetHeight = targetWidth * aspect;
    const float blurAmount   = blurParam != nullptr ? blurParam->load() : 0.0f;
    const int   neonChoice   = neonParam != nullptr ? (int) neonParam->load() : 0;
    const int   blurStyle    = styleParam != nullptr ? (int) styleParam->load() : 0; // 0 = Glow/Blur, 1 = Trail

    const int w = (int) targetWidth;
    const int h = (int) targetHeight;

    const auto bounds = getLocalBounds().toFloat();
    juce::Rectangle<float> spriteArea (0, 0, targetWidth, targetHeight);
    spriteArea.setCentre (bounds.getCentre());

    const auto& frame = getScaledFrame (cell, w, h);

    if (blurStyle == 1)
    {
        // --- Motion trail: fading afterimages of her last few poses ---
        const int steps = (int) trailHistory.size();
        for (int i = steps - 1; i >= 0; --i)
        {
            const float t = (float) (steps - i) / (float) (maxTrailLength + 1);
            const float alpha = juce::jlimit (0.0f, 0.55f, 0.55f * (1.0f - t));
            if (alpha <= 0.01f)
                continue;

            const auto& ghostFrame = getScaledFrame (trailHistory[(size_t) i], w, h);
            g.setOpacity (alpha);

            if (neonChoice > 0)
            {
                const auto& ghostGlow = getGlow (trailHistory[(size_t) i], ghostFrame,
                                                   juce::jmax (2.0f, blurAmount), neonChoice, w, h);
                g.drawImage (ghostGlow, spriteArea);
            }
            else
            {
                g.drawImage (ghostFrame, spriteArea);
            }
        }
        g.setOpacity (1.0f);
        g.drawImage (frame, spriteArea);
        return;
    }

    // --- Glow / Blur style ---
    const bool wantsGlow = neonChoice > 0 && blurAmount > 0.01f;
    const bool wantsPlainBlur = neonChoice == 0 && blurAmount > 0.01f;

    if (wantsGlow)
    {
        const auto& glow = getGlow (cell, frame, blurAmount, neonChoice, w, h);
        g.setOpacity (0.9f);
        g.drawImage (glow, spriteArea.expanded (blurAmount * 0.5f));
        g.setOpacity (0.6f);
        g.drawImage (glow, spriteArea.expanded (blurAmount));
        g.setOpacity (1.0f);
    }
    else if (wantsPlainBlur)
    {
        const auto& softened = getBlurredFrame (cell, frame, blurAmount, w, h);
        g.drawImage (softened, spriteArea);
        return;
    }

    g.drawImage (frame, spriteArea);
}

void SpriteView::mouseDown (const juce::MouseEvent& e)
{
    if (auto* top = getTopLevelComponent())
        dragger.startDraggingComponent (top, e);
}

void SpriteView::mouseDrag (const juce::MouseEvent& e)
{
    if (auto* top = getTopLevelComponent())
        dragger.dragComponent (top, e, nullptr);
}

void SpriteView::mouseDoubleClick (const juce::MouseEvent&)
{
    if (onDoubleClick != nullptr)
        onDoubleClick();
}

void SpriteView::timerCallback()
{
    // Belt-and-braces: keep the engine's animation/speed choice in sync with
    // the parameters even if, on some hosts, processBlock isn't being called
    // while the transport is stopped.
    if (auto* animParam = processor.apvts.getRawParameterValue (FruityDanceGirlProcessor::paramAnimation))
        processor.animationEngine.setAnimationChoice ((int) animParam->load());

    if (auto* speedParam = processor.apvts.getRawParameterValue (FruityDanceGirlProcessor::paramSpeed))
        processor.animationEngine.setSpeedMultiplier (AnimationEngine::speedChoiceToMultiplier ((int) speedParam->load()));

    const auto ideal = getIdealSize();

    if (auto* top = getTopLevelComponent())
    {
        if (top->getWidth() != ideal.x || top->getHeight() != ideal.y)
            top->setSize (ideal.x, ideal.y);
    }

    repaint();
}
