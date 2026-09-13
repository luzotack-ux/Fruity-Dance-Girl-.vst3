cmake_minimum_required(VERSION 3.22)

project(FruityDanceGirl VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------------------------------------------------------------------------
# Pull in JUCE. Pinned to a known-good tag so CI builds are reproducible.
# ---------------------------------------------------------------------------
include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 7.0.12
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(JUCE)

# ---------------------------------------------------------------------------
# Embed the sprite sheet (Assets/spritesheet.png) as C++ binary data.
# Generates BinaryData.h / BinaryData.cpp with SpritesheetData::spritesheet_png
# ---------------------------------------------------------------------------
juce_add_binary_data(FruityDanceGirlAssets
    SOURCES
        Assets/spritesheet.png
)
set_target_properties(FruityDanceGirlAssets PROPERTIES POSITION_INDEPENDENT_CODE ON)

# ---------------------------------------------------------------------------
# The plugin itself.
# ---------------------------------------------------------------------------
juce_add_plugin(FruityDanceGirl
    COMPANY_NAME "YourName"
    PLUGIN_MANUFACTURER_CODE Yrnm
    PLUGIN_CODE Fdgl
    FORMATS VST3
    PRODUCT_NAME "Fruity Dance Girl"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    VST3_CATEGORIES Fx Visualizer
    COPY_PLUGIN_AFTER_BUILD FALSE
)
juce_generate_juce_header(FruityDanceGirl)
target_sources(FruityDanceGirl PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
    Source/AnimationEngine.cpp
    Source/AnimationEngine.h
    Source/SpriteView.cpp
    Source/SpriteView.h
    Source/OverlayWindow.cpp
    Source/OverlayWindow.h
    Source/SettingsPanel.cpp
    Source/SettingsPanel.h
    Source/NeonLookAndFeel.cpp
    Source/NeonLookAndFeel.h
)

target_compile_definitions(FruityDanceGirl PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_DISPLAY_SPLASH_SCREEN=0
)

target_link_libraries(FruityDanceGirl PRIVATE
    FruityDanceGirlAssets
    juce::juce_audio_utils
    juce::juce_audio_processors
    juce::juce_gui_extra
    juce::juce_dsp
    juce::juce_graphics
    juce::juce_gui_basics
PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)
