# Fruity Dance Girl (VST3)
<a href="https://github.com/luzotack-ux/Fruity-Dance-Girl-.vst3-all-daws/actions/runs/34743617829/artifacts/10312973781">
  <img src="https://img.shields.io/badge/Download-Plugin-blue?style=for-the-badge" />
</a>



A "visual toy" VST3 effect plugin: drop it on your master channel in Bitwig,
and a sprite floats above everything (the DAW *and* other apps), dancing in
time with your project's tempo and transport.

Audio passes through completely untouched — it only reads tempo/playhead
info from the host, it never processes the signal.

## What's in this repo

```
CMakeLists.txt              - fetches JUCE and builds the plugin
Assets/spritesheet.png      - your 8x10 sprite grid, embedded into the plugin
Source/
  PluginProcessor.*         - audio pass-through, parameters, reads host tempo/playhead
  AnimationEngine.*         - works out which sprite cell to show, synced to the beat
  OverlayWindow.*           - the always-on-top floating window (drag it, double-click it)
  SpriteView.*              - draws the sprite, blur & neon-glow rendering
  SettingsPanel.*           - the knob panel (also opens as its own floating window)
  PluginEditor.*            - the panel Bitwig shows if you open the plugin normally
  NeonLookAndFeel.*         - the look of the knobs
.github/workflows/build.yml - CI: builds Windows/macOS/Linux VST3s automatically
```

## How it behaves

- **Idle (transport stopped):** always shows row 1 / frame 1, no matter which
  animation is selected.
- **Playing:** steps through the frames of whichever row you picked (or, in
  "Random" mode, jumps to a random cell on the sheet every step), synced to
  the project's BPM via the **Anim Speed** knob (2 frames per beat at 1.0x).
- **Drag** the sprite anywhere on screen with the mouse — its position is
  remembered per-project (saved in plugin state).
- **Double-click** the sprite to open/close the settings panel:
  - **Animation** — Row 1..Row 10, or Random
  - **Neon Glow** — Off / Red / Raspberry / Magenta / Purple / Blue / Cyan / Green / Yellow / White
  - **Blur** — 0..20 px (also controls glow softness/size)
  - **Size** — girl's width in px (default 200, height follows the sprite's
    real proportions automatically)
  - **Anim Speed** — 0.1x..4x
- Opening the plugin's normal panel inside Bitwig shows the *same* knobs too
  (handy backup if the floating window is ever off-screen).

## Building it yourself (GitHub Actions)

Just push this repo to GitHub — `.github/workflows/build.yml` runs
automatically on every push to `main` and on manual trigger ("Run workflow"),
and builds the VST3 for Windows, macOS and Linux. Download the finished
`.vst3` from the run's **Artifacts** section.

### Building locally
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --target FruityDanceGirl_VST3 --parallel
```
The built plugin will be under `build/FruityDanceGirl_artefacts/Release/VST3/`.
Copy `Fruity Dance Girl.vst3` into your VST3 folder
(Windows: `C:\Program Files\Common Files\VST3`,
macOS: `/Library/Audio/Plug-Ins/VST3`,
Linux: `~/.vst3`), then rescan plugins in Bitwig.

## Known limitations / things worth knowing

- **"Always on top of everything"** relies on the OS honoring a topmost/
  floating window flag from a plugin process. This works well on Windows and
  Linux; on macOS, sandboxed/notarization-restricted hosts occasionally clip
  this behaviour — if the girl doesn't float above other apps on macOS, try
  disabling App Sandbox restrictions for Bitwig or check Bitwig's plugin
  sandboxing setting.
- **Unsigned builds**: CI output isn't code-signed/notarized. On macOS you
  may need to right-click → Open the first time, or run
  `xattr -dr com.apple.quarantine "Fruity Dance Girl.vst3"` if Gatekeeper
  blocks it. On Windows, unsigned plugins load fine but may show a SmartScreen
  warning on first copy from a browser download.
- The window position is saved per **plugin instance** (i.e. per project), not
  globally — if you use it on several projects, each remembers its own spot.
- Only one sprite window is ever created per plugin instance; deleting the
  plugin from the channel closes its window.

## Customizing the sprite sheet

Replace `Assets/spritesheet.png` with your own art as long as it keeps the
same grid: **8 columns × 10 rows**, each cell the same pixel size. If your
cell size differs from 220×256, update `cellWidth`/`cellHeight` in
`Source/SpriteView.h` and `Source/AnimationEngine.h` (`numRows`/`numColumns`)
to match.
