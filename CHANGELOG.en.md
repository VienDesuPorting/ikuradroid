# IkuraDroid Changelog

English translation of the Russian changelog ([CHANGELOG.md](CHANGELOG.md)), which remains the authoritative version. The history of the old 2015–2016 Android port lives in the README and the archived VK thread.

All releases older than 2.5.0 have been removed from GitHub Releases: after the rework that went into 2.5.0, the old builds are obsolete and no longer make sense. The current version is 2.5.1.

## 2.5.1 — 2026-10-03

- Heart de Roommate: background music plays again after loading a save — the same track that was playing when it was saved. The track is written into the save, and music stopped before saving stays off after the load. Old saves work as before.

## 2.5.0 — 2026-10-01

- Heart de Roommate: a real textbox with the graphics from the game files, and choices that actually work. The text is drawn with the font bundled with the app instead of the engine's built-in one. The textbox renders without distortion, and the scenario no longer stalls on an empty screen.
- Heart de Roommate: the dialog layout is aligned with the PC version. The speaker name sits on the top row, the quote below it, and the line spacing matches the original. Narration and continuation pages no longer start on the name row.
- Heart de Roommate: speaker names that the original never showed, taken from the Russian release data. Narration stays unnamed.
- Heart de Roommate: fixed the transparent rendering of 24-bit images. The picture is no longer covered by a black rectangle; 32-bit `GGA` keeps its regular alpha blending.
- Ikura: MPEG-1 video playback via `pl_mpeg`. Videos from archives and plain `.mpg`/`.avi` files in the game folder are supported; Critical Point can use `.dat` files carrying an MPEG stream. Playback can be skipped with a tap on the screen; on PC, with a mouse click or any key.
- Ikura: CD-DA tracks from `WMSC` can play. Also fixed the reading of compressed WAVs from `Crescendo` — IMA ADPCM now decodes properly.
- Ikura: added the animated `GP 15/16/17` transitions and support for `GSCRL (0x62)`, which pans the viewport across an oversized VRAM surface. The 24-bit `GGD` alignment is fixed along the way.
- Touch: input is closer to PC behavior — the choice highlight is visible while pressing and commits on release. Fixed the click coordinate shift on wide screens, where the letterbox math was applied a second time.
- Save/load: the native Android save/load calls now open a Java slot dialog. 40 slots with screenshots, dates and scene captions are available. Fixed the Cyrillic captions and saving into empty slots.
- Library: tiles are now 2:3 posters with VNDB covers. Automatic cover search, a cache and manual cover removal are added. Ikura GDL titles can be auto-named from their `SUF`.
- Exiting a game on Android no longer shows the second `Exit game?` prompt when the user has already chosen exit inside the game. The save on exit stays.
- The "Settings" row of the in-game menu is disabled for now and shows a notice that the feature will arrive later.
- Little My Maid is marked as untested again.
- The remaining `vilevn.pck` mentions are gone: the file is no longer needed, so it disappeared from the hints and the regular log too.

## 2.0.6 — 2026-09-21

- Crescendo: music plays again on the intro, in the menu and during gameplay. `SE` and `WMSC` are read as native `SM2MPX10`; the MIDI bank comes from the separate MIDI file.
- Fixed the playback of IMA ADPCM WAVs.
- Critical Point: fixed mirrored savegame thumbnails on some devices.
- Critical Point: loading a save resumes the current scene music. New saves store the track; old saves behave as before.

## 2.0.5 — 2026-09-21

- Critical Point: choices are back inside the text window, as in the original. Fixed the row positions and the tap handling outside the choices.
- Updated the version strings in `config.h`.

## 2.0.4 — 2026-09-21

- Critical Point: nameless text now starts on the same row as the regular dialog text.

## 2.0.3 — 2026-09-20

- Critical Point: the text window shows speaker names with the colors from the original game.
- Fixed the `SAY` read: the name no longer loses its first two characters.
- The dialog text is back inside the window.
- Added a text shadow.
- The window position (`12,300`) was checked against the original.

## 2.0.2 — 2026-09-20

- Critical Point: fixed the character sprite coordinates.
- The text window moved to the bottom, as in the original (`12,300`, 616x152).
- The headless test gained an optional periodic frame dump.
- Updated the version in `common/config.h`.

## 2.0.1 — 2026-09-19

- Crescendo: fixed the recognition of the English release with the original `CresD.suf`.
- ViLE was renamed to IkuraDroid in all user-visible messages and logs; GPL headers untouched.
- The Crescendo choices were verified on the English release.
- Removed debug leftovers from the log and fixed a format string.
- The PC build gained a `LOGCAT` fallback and a documented choice autodrive.

## 2.0.0 — 2026-09-18

- Updated the app icon for different Android versions.
- The launcher menu was restyled to match the tile context menu.

## 1.9.8 — 2026-09-17

- The "About" screen now opens as a modal window.
- Added a top-down swipe that closes game-drawn windows.
- The game menu can be opened with a bottom-up swipe from anywhere on the screen.
- Fixed the black screen when relaunching a game.
- Added launch-stage messages and proper handling of an SDL renderer creation error.
- Fixed the library freezing after exiting a game.
- SDL was upgraded from 2.0.3 to 2.30.12.
- The tile context menu now respects the safe area.
- Critical Point: the choice highlight and geometry are closer to the PC version, with rows enlarged for touch.
- Fixed the game window scaling after the SDL 2.30 switch.
- The synthesis of mouse events from touch is disabled on Android.
- Immersive mode is reapplied when focus returns.
- Exiting through the game menu no longer leaves the library in a broken state.

## 1.9.7 — 2026-09-11

- Critical Point: fixed the black screen during events with multi-layer EV graphics.
- The dialog and text are aligned with the PC version.
- The choices are back inside the text window.
- Fixed the first two letters of choice entries being eaten.
- After exiting a game the app no longer stays in landscape.
- `minSdk` raised to 24; CI workflows removed, release signing became optional.

## 1.9.6 — 2026-09-10

- `vilevn.pck` is no longer needed for Ikura GDL.
- Ikura GDL games are recognized by their own data files.

## 1.9.5 — 2026-09-10

- The game menu opening gesture was changed.
- `vile_log.txt` was renamed to `ikuradroid_log.txt`.
- `libvile.so` was renamed to `libikuradroid.so`, the logcat tag to `ikuradroid`.
- The README was updated and this changelog was added.

## 1.9.4 — 2026-09-09

- Fixed touches on the Little My Maid choices.
- Choice entries were spaced out for a finger.
- Taps inside the window during a choice no longer scroll the text.
- Fixed the version number.

## 1.9.3 — 2026-09-09

- The Little My Maid choices were brought to the PC version.
- Added the «ОЧКИ» (points) bar on the game window frame.

## 1.9.2 — 2026-09-09

- Fixed the choices in the Russian Will releases: Little My Maid no longer crashes, Critical Point keeps script sync.
- The `OP02` grammar was split for CP and LMM.

## 1.9.1 — 2026-09-09

- The library tile context menu opens under the tile.

## 1.9.0 — 2026-09-09

- The library was reworked: multiple root folders, a context menu, new tiles and an in-app game exit.
- Added a bottom-up swipe for the game menu.
- A desktop core build for Linux was prepared (`pc/`, `vile-pc`).

## 1.8.2 — 2026-09-09

- Added the **Little My Maid** module for Will.
- Will dialogs were brought closer to the PC originals.
- Mouse-event synthesis from touch was disabled in SDL 2.0.3.

## 1.8.1 — 2026-09-09

- Added protection against broken Will WIPF data.
- The engine log is written to a file in the game folder.
- Unpacking of uncompressed WIPF frames was added.

## 1.8.0 — 2026-09-09

- The library recognizes game folders for all supported engines.

## 1.7.0 — 2026-09-08

- A built-in file browser replaced the SAF picker.

## 1.6.0 — 2026-09-08

- The "Engines" screen moved to `engines.json`.

## 1.5.0 — 2026-09-08

- Added the gesture menu.
- Removed the duplicate menu sheets.

## 1.4.0 — 2026-09-06

- The project was fully renamed to IkuraDroid (`su.viende.ikuradroid`).

## 1.3.0 — 2026-09-05

- The menu moved to Material 3.
- Added CI and signed releases.
- Removed old code and fixed resource linking.

## 1.2.0 — 2026-09-05

- The library moved to SAF.
- Added Material 3 UI and the "Engines" screen.

## 1.1.0 — 2026-09-05

- Added DayNight / Material You, M3 dialogs and cards, an adaptive icon.
- Added an "About" screen with attribution and the port's origins.

## 1.0.0 — 2026-09-05

- Vile was renamed to IkuraDroid (`applicationId` — `su.viende.ikuradroid`).

## 0.54.4 — 2026-09-05

- The game font is installed automatically.
- Removed the unused `vilevn.pck`.

## 0.54.3 — 2026-09-05

- Fixed the library grid crash and version captions.

## 0.54.2 — 2026-09-05

- First public push of the revived port: ViLE builds under Android again on a modern NDK, libraries are built for AArch64, release APK builds are restored.
