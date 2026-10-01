# IkuraDroid

**IkuraDroid** is an Android interpreter for visual novels built with the **Ikura GDL** engine. Its main focus is the classic JAST USA / G-Collections titles: *Crescendo*, *Kana ~ Little Sister*, *The Sagara Family* and others.

The project grew out of the abandoned 2016 Android port of **ViLE (Visual Library Engine)**. ViLE itself covered not only Ikura GDL but also Will, Crowd, C-Ware, JAST, Windy and T-Love. The current goal is to polish the old games and gradually bring back support for the other engines.

Interface: English / Russian. License: GPLv3.

Русская версия: [README.md](README.md)

## Compatibility

Every engine module from the table is compiled into the app. A title earns the tested mark only after it has been checked end to end on a real device; what exactly has been reworked is visible in the [changelog](CHANGELOG.en.md).

Current statuses also live in `app/src/main/assets/engines.json` and are shown on the in-app **Engines** screen.

| Engine | Title | Status |
| --- | --- | --- |
| Ikura GDL | Crescendo | tested |
| Ikura GDL | Heart de Roommate | tested |
| Ikura GDL | Cat Girl Alliance | not tested |
| Ikura GDL | Hitomi -My Stepsister- | not tested |
| Ikura GDL | Idols Galore! | not tested |
| Ikura GDL | Kana ... Okaeri! | not tested |
| Ikura GDL | Kana ~ Little Sister | not tested |
| Ikura GDL | The Sagara Family | not tested |
| Ikura GDL | Snow | not tested |
| Ikura GDL | Virgin | not tested |
| Will | Critical Point | tested |
| Will | Princess Waltz | not tested |
| Will | Starry Sky | not tested |
| Will | Yume Miru Kusuri | not tested |
| Will | Little My Maid | not tested |
| Crowd | Tokimeki Check-in! | not tested |
| Crowd | XChange 1 | not tested |
| Crowd | XChange 3 | not tested |
| C-Ware | DiviDead | not tested |
| JAST | Season of the Sakura | not tested |
| JAST | 3 Sisters Story | not tested |
| JAST | Runaway City | not tested |
| Windy | Mayclub VR Dating SX | not tested |
| Windy | Nocturnal Illusion Renewal | not tested |
| T-Love | True Love | not tested |

## Known issues

- The menu audio may crackle very faintly, but it disappears later during gameplay. This will be fixed in the next version.

## Additional modules

The base engine code is extended with dedicated modules built for specific games. The first one is **Little My Maid (LMM)** for the Will family. Currently working: game launch and scene playback, PC-style choices, enlarged touch hit areas and the points bar on the game window frame.

Still planned: speaker name plates and an extra playthrough check on real saves. See [CHANGELOG.en.md](CHANGELOG.en.md) for the per-version details.

## Installation

1. Grab an APK from the Releases page and install it. `arm64-v8a` and `armeabi-v7a` are supported.
2. Put each game into its own folder. For example:

   ```text
   /storage/emulated/0/Novels/
   └── Crescendo/
       ├── GGD
       ├── ISF
       └── ...game files...
   ```

   Ikura GDL usually needs `GGD` / `ISF` and `*.suf`; Will needs archives like `Rio.arc` and `Chip.arc`.
3. Start IkuraDroid, grant file access, tap **Choose folder** and select the `Novels` folder (or a game folder directly).
4. Open the game from the library.

Files are read from the storage in place, nothing is copied. Saves are stored in the app's private folder.

## Controls

The in-game menu opens with a bottom-up swipe from anywhere on the screen.

Saving and loading use a dedicated slot screen: 40 slots, each showing a screenshot, the date and the scene caption; long-press an occupied slot to delete its save. The menu also has auto-skip and quit, while Settings is a stub for now.

Games run in landscape orientation, and the screen stays awake while playing.

## Desktop (PC) build

`pc/` builds the interpreter core for desktop Linux (`vile-pc`). It is primarily a development tool: quick engine changes and script runs without building an APK. Headless runs use the `dummy` video driver.

It is not a standalone user release yet. Full Linux and Windows builds are planned.

```text
cd pc && make SDL2DIR=/path/to/SDL2/prefix
./build/vile-pc --cwd <folder with the font> --game <game folder> \
    --fontface <font file> --logfile /tmp/log.txt
```

## Troubleshooting

Every launch writes `ikuradroid_log.txt` into the game folder. The file is overwritten on the next launch.

If a game does not show up in the library, fails to start or crashes, attach that log to your issue. It contains the same messages as `logcat` with the `ikuradroid` tag, but does not require Android Studio.

## Building

Requirements:

- JDK 17+
- Android SDK (platform 35)
- Android NDK r27c

```bash
./gradlew assembleRelease
# or
./gradlew assembleDebug
```

The Gradle Wrapper 8.10.2 is already in the repository, so there is no need to install Gradle separately.

## Roadmap

- desktop builds of the core for Linux and Windows;
- testing the remaining titles on real devices;
- further work on the Will modules and other engines.

The bundled SDL 2.0.3 was already upgraded to 2.30.x in 1.9.8: SDL 2.30.12 is in use now.

## History

- **ViLE (2010–2012)** — the desktop interpreter by the ViLE Team, lead developer BaSF. Development stopped in 2012; the original project is available on SourceForge.
- **Vile (2015–2016)** — an Android port of ViLE 0.4.13, versions 0.5–0.54. It was published in the «Визуальные новеллы для Android» VK community, then abandoned and the sources disappeared. That community and the «Визуальные новеллы для Android» Telegram channel of the VienDesu! Porting Team are different projects.
- **IkuraDroid (2026)** — the Android port sources were recovered from an archive saved in 2024. Since then the toolchain and dependencies were updated, the libraries were rebuilt for AArch64 and the launcher moved to Material 3. The engine core has seen a major rework as well: skinned text windows with speaker names, MPEG-1 video and CD-DA playback, a new save system and a library with cover art. Development is continued by the VienDesu! Porting Team.

The core directories and files (`jni/vile/`, `vile.cpp`) still keep the old ViLE names. This makes it easier to compare the code against the SourceForge upstream without breaking the historical structure.

## Credits

- **ViLE Team** — the ViLE engine (GPLv3)
- **"Ivan"** — the original Android port (2015–2016)
- **VienDesu! Porting Team** — current IkuraDroid development
- SDL 2.30.12, SDL_gfx / SDL_image / SDL_mixer / SDL_ttf, libmikmod, SMPEG2, libwebp, HQ2x (MaxSt), utf8cpp
- the Will engine was ported from [VNMV](https://github.com/soywiz-archive/vnvm) with the author's permission

## Links

- Upstream ViLE: <https://sourceforge.net/projects/vilevn/>
- Changelog of the 2016 port: <https://vk.ru/topic-69564393_33447386>
- IkuraDroid changelog: [CHANGELOG.en.md](CHANGELOG.en.md) (Russian: [CHANGELOG.md](CHANGELOG.md))
- Telegram — VienDesu! Porting Team: <https://t.me/visual_novels_for_android>

## License

[GPLv3](LICENSE)
