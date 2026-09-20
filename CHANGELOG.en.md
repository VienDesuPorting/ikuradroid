# IkuraDroid Changelog

English translation of the Russian changelog ([CHANGELOG.md](CHANGELOG.md)), which remains the authoritative version. The history of the old 2015–2016 Android port lives in the README and the archived VK thread.

## 2.0.5 — 2026-09-21

- Critical Point: choices are back inside the text window, as in the original. The layout still dated from when the window clung to the top edge, so the choice rows painted over the scene while the window sat empty below. Captions now sit on the same rows as the dialog text, centered; hovering inverts the row, and a tap past the rows no longer advances the line.

## 2.0.4 — 2026-09-21

- Critical Point: nameless-page text moved down to the common row. The original keeps it on the same lines as named pages, not flush against the window frame.

## 2.0.3 — 2026-09-20

- Critical Point: the speaker name is shown in the text window. The color comes from the original's table (CP scripts only ever use blue and crimson), the name prints small and centered on the window's first row.
- Critical Point: fixed the SAY opcode read - names used to lose their first two characters ("Reiko" turned into "iko"). LMM uses a different SAY format and stays untouched, same for the v1 games.
- Critical Point: dialog text is back inside the window instead of the screen's top edge - after the window moved down, the text kept its dialog-local coordinates.
- Text shadow implemented: it was an empty stub in the engine. Critical Point draws the text and the name with a shadow, like the original default.
- The window position (12,300) was checked against the MsgPos registry value shipped with the original's installer.

## 2.0.2 — 2026-09-20

- Critical Point: character sprites no longer "detach from the floor". Scripts pass each sprite's position right in the load opcode; the engine discarded the coordinates and drew the sprite from the top of its slot - at 424 px tall on a 480 px screen that left a gap of up to 56 px.
- Critical Point: the text window moved to the bottom of the screen, as in the original. The position hint in the winbase0 header turned out to be dialog-local, not a screen position - the window stuck to the top edge. It now docks to the bottom of the play area: (12,300), 616x152.
- The autodrive test gained an optional periodic frame dump for headless render checks (IKURADROID_SHOTS).
- The version in common/config.h finally caught up with the release commits.

## 2.0.1 — 2026-09-19

- Crescendo: the English release boots again with the original CresD.suf. ProbeSUF had a filename typo (it hunted the nonexistent CRES.suf) and never checked the CRESD key.
- Branding: ViLE renamed to IkuraDroid in every user-visible string - the "No valid game resources" dialog, window caption and version, logcat tag, intro logo, screenshot names, --about. GPL license headers untouched.
- Crescendo choices smoke-tested end-to-end on the English scripts: the story branches correctly.
- The log is cleaned of debug leftovers, and a format string with two %d for one argument is fixed.
- PC build: LOGCAT gained a desktop fallback, the choice autodrive (IKURADROID_AUTODRIVE) is documented in pc/Makefile.

## 2.0.0 — 2026-09-18

- New app icon: portrait art on the adaptive background layer — the launcher mask crops it on its own, keeping the face inside the safe zone. PNGs for Android 7.x and older were regenerated up to xxxhdpi; the salmon-roe silhouette remains as the themed icon on Android 13+.
- The launcher toolbar menu now matches the tile context menu style: a "More options" button opens the same rounded panel with the Engines, Add game folder and About rows. The stock text-only overflow list is gone.

## 1.9.8 — 2026-09-17

- The About screen is now a modal styled like the tile long-press menu instead of a separate screen.
- Added a top-down swipe that closes game-drawn windows. It behaves like Escape on PC.
- The menu can now be opened with a bottom-up swipe from anywhere on the screen. Regular taps and drags still reach the game.
- Added extra logging for the `Exit` button in the save/load dialogs. Logcat now shows the click coordinates and the hit-test result; engine touch events are logged too, with normalized and logical coordinates.
- Fixed the black screen when relaunching a game. The game now runs in a process separated from the library.
- Added launch-stage breadcrumbs to logcat (`ikuradroid`).
- An SDL renderer creation failure is no longer ignored: `InitVideo` fails instead of running the engine into a black screen.
- The directory enumerator (`EDL_GetFileReset`) is reset at the start of a new session, so an aborted exit can no longer hand over a stale `DIR*`.
- The bundled SDL was upgraded from 2.0.3 to 2.30.12, together with the Android glue; game launch, the menu and the mixer pause were adapted.
- Fixed the library freezing after exiting a game. The native thread now shuts down cleanly and the next session starts with fresh settings.
- The tile context menu now respects the safe area and no longer covers the system navigation.
- Critical Point: the choice highlight now matches the PC version — the row is inverted, and on touch the highlight follows the finger and commits on release.
- Critical Point: choice block coordinates were fitted to PC screenshots; the rows form a solid block with no dead zones.
- Fixed the game window size after the SDL 2.30 switch. The game scales to the full screen again.
- On Android the synthesis of mouse events from touch is disabled: the engine receives pure touch input.
- The status and navigation bars hide in-game again. Immersive mode is reapplied whenever focus returns.
- Fixed the dead library screen after exiting via the menu. The native thread now receives QUIT and shuts down properly, and `join()` no longer blocks on a sleeping SDL thread.
- On Android QUIT now ends the game without an extra confirmation dialog. The confirmation stays with the games' own menus and the desktop build.
- Critical Point: choice rows are now 32 px tall and 32 px apart, so they are easier to hit with a finger.

## 1.9.7 — 2026-09-11

- Critical Point: fixed the black screen during event scenes. Multi-layer EV graphics from WIPF are now composed from several frames.
- Critical Point: the text window and the text coordinates are aligned with the PC version.
- Critical Point: choices are drawn inside the text window again, as in the original. Hit areas span the full row width.
- Critical Point: fixed the first two letters of a choice being eaten. CP no longer reads a parameter that only exists in Little My Maid.
- Leaving a game via the menu no longer leaves the app in landscape.
- Build: minSdk raised to 24; CI workflows removed, APKs are built locally. Release signing became optional.

## 1.9.6 — 2026-09-10

- Ikura GDL no longer needs `vilevn.pck`: three PNGs for the settings widgets are embedded in the engine.
- Ikura GDL games are now recognized by their own data (`GGD`+`ISF` or `drssnr`+`drsgrp`).

## 1.9.5 — 2026-09-10

- The menu swipe capture zone was raised from 96 to 40 dp above the bottom edge. The diagonal threshold was widened from 2:1 to 1.5:1.
- `vile_log.txt` was renamed to `ikuradroid_log.txt`.
- `libvile.so` was renamed to `libikuradroid.so`, the logcat tag `vile` to `ikuradroid`.
- README updated: the PC build, the Little My Maid module and the new menu controls.
- This changelog was added.

## 1.9.4 — 2026-09-09

- Fixed touch on LMM choices: the dialog rectangle now matches the text window.
- LMM choice entries were spaced out for a finger; with many entries the layout becomes denser.
- Taps inside the window while a choice is shown no longer scroll the text.
- Fixed the version number left behind in 1.9.3.

## 1.9.3 — 2026-09-09

- LMM choices were brought to the PC version: entries live in the text window, a `SELECT` label was added, the old backing plates are gone. Rendering was moved into `LayoutTextSelection()`.
- Added the «ОЧКИ» (points) bar on the LMM game window frame.

## 1.9.2 — 2026-09-09

- Fixed choices in the Russian Will releases: LMM no longer crashes and Critical Point no longer loses script sync.
- The `OP02` grammar was split for CP and LMM.

## 1.9.1 — 2026-09-09

- The library tile context menu opens under the tile on a compact rounded panel.

## 1.9.0 — 2026-09-09

- Library rework: multiple root folders, a context menu, new tiles and an in-app game exit.
- Added a bottom-up swipe for the in-game menu.
- A desktop core build for Linux was set up (`pc/`, `vile-pc`).

## 1.8.2 — 2026-09-09

- Added the **Little My Maid** module for Will.
- Will dialogs were brought closer to the PC originals.
- Mouse-event synthesis from touch was disabled in SDL 2.0.3; input logs were added.

## 1.8.1 — 2026-09-09

- The engine survives broken Will WIPF data.
- The engine log is written to a file inside the game folder.
- Unpacking of uncompressed WIPF frames (`0xCCCCCCCC`) was added.

## 1.8.0 — 2026-09-09

- The library recognizes game folders of all supported engines.

## 1.7.0 — 2026-09-08

- A built-in file browser replaced the SAF picker.

## 1.6.0 — 2026-09-08

- The "Engines" screen moved to `engines.json`: the game list and statuses now live in data.

## 1.5.0 — 2026-09-08

- Added the gesture menu. The swipe direction was later changed to bottom-up.
- Removed the duplicate menu sheets.

## 1.4.0 — 2026-09-06

- The project was fully renamed to IkuraDroid: Java namespace `su.viende.ikuradroid`, versionCode 6.

## 1.3.0 — 2026-09-05

- The menu moved to Material 3.
- CI and signed releases were added.
- Old code was removed and resource linking was fixed.

## 1.2.0 — 2026-09-05

- The library moved to SAF.
- A full Material 3 UI and an "Engines" screen were added.

## 1.1.0 — 2026-09-05

- Material 3: DayNight, Material You, M3 dialogs and cards, an adaptive icon.
- Added an "About" screen with attribution and the port's origins.

## 1.0.0 — 2026-09-05

- Vile was renamed to IkuraDroid (`applicationId` — `su.viende.ikuradroid`).

## 0.54.4 — 2026-09-05

- The game font is installed automatically.
- The unused `vilevn.pck` was removed.

## 0.54.3 — 2026-09-05

- Fixed a library grid crash and version captions.

## 0.54.2 — 2026-09-05

- The first public push of the revived port: ViLE builds on Android again on a modern NDK, libraries were rebuilt for AArch64 and release APK builds were restored.
