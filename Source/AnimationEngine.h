#pragma once
#include <JuceHeader.h>
#include <random>

/**
    Works out which sprite-sheet cell (row, column) should be on screen
    right now.

      - Transport STOPPED : she never freezes - row 0 (the first animation)
        keeps looping continuously, timed off the wall clock at the
        project's BPM (so it still "breathes" with the song's tempo even
        when nothing is playing).
      - Transport PLAYING : steps through the chosen row's 8 frames locked
        to the host's beat position (so it scrubs/stays in sync if you move
        the playhead), or, in "Random" mode, jumps to a random cell every step.
*/
class AnimationEngine
{
public:
    static constexpr int numColumns = 8;
    static constexpr int numRows    = 10;
    static constexpr int randomModeIndex = numRows; // 10th index = "Random"

    // The 7 fixed speed positions shown in the Speed dropdown, as a
    // multiplier of the base rate (2 sprite-frames per beat).
    static constexpr float speedChoices[7] = { 0.25f, 0.5f, 1.0f, 2.0f, 3.0f, 4.0f, 8.0f };
    static constexpr int numSpeedChoices = 7;

    static juce::StringArray getSpeedChoiceLabels()
    {
        return { "0.25x", "0.5x", "1x", "2x", "3x", "4x", "8x" };
    }

    static float speedChoiceToMultiplier (int index)
    {
        return speedChoices[(size_t) juce::jlimit (0, numSpeedChoices - 1, index)];
    }

    /** Call once per audio block with fresh transport info from the host. */
    void updateTransport (bool hostIsPlaying, double bpm, double ppqPosition)
    {
        isPlaying.store (hostIsPlaying, std::memory_order_relaxed);
        currentBpm.store (bpm > 0.0 ? bpm : 120.0, std::memory_order_relaxed);
        currentPpq.store (ppqPosition, std::memory_order_relaxed);
    }

    void setAnimationChoice (int index)   { animationChoice.store (index, std::memory_order_relaxed); }
    void setSpeedMultiplier (float speed) { speedMultiplier.store (speed, std::memory_order_relaxed); }

    struct Cell
    {
        int row = 0;
        int column = 0;
        bool operator== (const Cell& o) const { return row == o.row && column == o.column; }
        bool operator!= (const Cell& o) const { return ! (*this == o); }
    };

    /** Call regularly from a UI timer (message thread only - not realtime-safe). */
    Cell getCurrentCell()
    {
        const double now = juce::Time::getMillisecondCounterHiRes();
        double dt = lastUpdateMs > 0.0 ? (now - lastUpdateMs) / 1000.0 : 0.0;
        dt = juce::jlimit (0.0, 0.25, dt); // guard against big jumps (window minimised etc.)
        lastUpdateMs = now;

        const bool playing = isPlaying.load (std::memory_order_relaxed);
        const double bpm = currentBpm.load (std::memory_order_relaxed);
        const double speed = (double) speedMultiplier.load (std::memory_order_relaxed);
        const double framesPerBeat = 2.0 * speed;

        if (! playing)
        {
            const double stepsPerSecond = (bpm / 60.0) * framesPerBeat;
            idlePhase += dt * stepsPerSecond;
            const int stepIndex = (int) std::floor (idlePhase);
            lastRandomStep = -1;
            return { 0, wrap (stepIndex) };
        }

        const int choice = animationChoice.load (std::memory_order_relaxed);
        const double ppq = currentPpq.load (std::memory_order_relaxed);
        const double totalSteps = ppq * framesPerBeat;
        const int stepIndex = (int) std::floor (totalSteps);
        idlePhase = totalSteps; // stay continuous if playback stops right after this

        if (choice == randomModeIndex)
        {
            if (stepIndex != lastRandomStep)
            {
                lastRandomStep = stepIndex;
                std::mt19937 rng ((unsigned) (stepIndex * 2654435761u + 1u));
                std::uniform_int_distribution<int> rowDist (0, numRows - 1);
                std::uniform_int_distribution<int> colDist (0, numColumns - 1);
                cachedRandomCell = { rowDist (rng), colDist (rng) };
            }
            return cachedRandomCell;
        }

        lastRandomStep = -1;
        const int row = juce::jlimit (0, numRows - 1, choice);
        return { row, wrap (stepIndex) };
    }

private:
    static int wrap (int stepIndex) { return ((stepIndex % numColumns) + numColumns) % numColumns; }

    std::atomic<bool> isPlaying { false };
    std::atomic<double> currentBpm { 120.0 };
    std::atomic<double> currentPpq { 0.0 };
    std::atomic<int> animationChoice { 0 };
    std::atomic<float> speedMultiplier { 1.0f };

    double idlePhase = 0.0;
    double lastUpdateMs = 0.0;

    int lastRandomStep = -1;
    Cell cachedRandomCell {};
};
