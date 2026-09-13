#include "NeonLookAndFeel.h"

NeonLookAndFeel::NeonLookAndFeel()
{
    setColour (juce::Slider::rotarySliderFillColourId, juce::Colour::fromRGB (255, 150, 60));
    setColour (juce::Slider::rotarySliderOutlineColourId, juce::Colour::fromRGB (60, 60, 68));
    setColour (juce::Slider::thumbColourId, juce::Colours::white);
    setColour (juce::Label::textColourId, juce::Colour::fromRGB (230, 230, 235));
    setColour (juce::ComboBox::backgroundColourId, juce::Colour::fromRGB (40, 40, 46));
    setColour (juce::ComboBox::textColourId, juce::Colours::white);
    setColour (juce::ComboBox::outlineColourId, juce::Colour::fromRGB (90, 90, 100));
    setColour (juce::DocumentWindow::backgroundColourId, juce::Colour::fromRGB (26, 26, 30));
    setColour (juce::DocumentWindow::textColourId, juce::Colours::white);
    setColour (juce::PopupMenu::backgroundColourId, juce::Colour::fromRGB (40, 40, 46));
    setColour (juce::PopupMenu::textColourId, juce::Colours::white);
}

void NeonLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                                         float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                                         juce::Slider& slider)
{
    auto bounds = juce::Rectangle<int> (x, y, width, height).toFloat().reduced (6.0f);
    auto radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) * 0.5f;
    auto centre = bounds.getCentre();
    auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

    const auto fillColour = slider.findColour (juce::Slider::rotarySliderFillColourId);

    // Soft outer glow
    g.setColour (fillColour.withAlpha (0.18f));
    g.fillEllipse (bounds.expanded (3.0f));

    // Track
    juce::Path track;
    track.addCentredArc (centre.x, centre.y, radius, radius, 0.0f,
                          rotaryStartAngle, rotaryEndAngle, true);
    g.setColour (slider.findColour (juce::Slider::rotarySliderOutlineColourId));
    g.strokePath (track, juce::PathStrokeType (3.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // Value arc
    juce::Path valueArc;
    valueArc.addCentredArc (centre.x, centre.y, radius, radius, 0.0f,
                             rotaryStartAngle, angle, true);
    g.setColour (fillColour);
    g.strokePath (valueArc, juce::PathStrokeType (3.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // Knob body
    auto knobRadius = radius * 0.62f;
    g.setColour (juce::Colour::fromRGB (52, 52, 58));
    g.fillEllipse (juce::Rectangle<float> (knobRadius * 2, knobRadius * 2).withCentre (centre));

    // Pointer
    juce::Path pointer;
    const float pointerLength = knobRadius * 0.85f;
    const float pointerThickness = 2.5f;
    pointer.addRoundedRectangle (-pointerThickness * 0.5f, -pointerLength, pointerThickness, pointerLength, 1.25f);
    pointer.applyTransform (juce::AffineTransform::rotation (angle).translated (centre));
    g.setColour (juce::Colours::white);
    g.fillPath (pointer);
}

juce::Font NeonLookAndFeel::getLabelFont (juce::Label&)
{
    return juce::Font (13.0f, juce::Font::plain);
}

void NeonLookAndFeel::drawComboBox (juce::Graphics& g, int width, int height, bool,
                                     int, int, int, int, juce::ComboBox& box)
{
    auto bounds = juce::Rectangle<float> (0, 0, (float) width, (float) height).reduced (1.0f);

    g.setColour (box.findColour (juce::ComboBox::backgroundColourId));
    g.fillRoundedRectangle (bounds, 6.0f);

    g.setColour (box.findColour (juce::ComboBox::outlineColourId));
    g.drawRoundedRectangle (bounds, 6.0f, 1.2f);

    // Little accent arrow on the right, coloured to match the knobs.
    const float arrowSize = 5.0f;
    juce::Rectangle<float> arrowZone (bounds.getRight() - 24.0f, 0.0f, 24.0f, bounds.getHeight());
    juce::Path arrow;
    arrow.addTriangle (arrowZone.getCentreX() - arrowSize, arrowZone.getCentreY() - arrowSize * 0.5f,
                        arrowZone.getCentreX() + arrowSize, arrowZone.getCentreY() - arrowSize * 0.5f,
                        arrowZone.getCentreX(), arrowZone.getCentreY() + arrowSize * 0.6f);
    g.setColour (juce::Colour::fromRGB (255, 150, 60));
    g.fillPath (arrow);
}
