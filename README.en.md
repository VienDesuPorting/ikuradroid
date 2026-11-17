# IkuraDroid

**IkuraDroid** is an Android player for visual novels with two engine lines: classic JAST USA / G-Collections titles on the **Ikura GDL** and **Will** engines (*Crescendo*, *Heart de Roommate*, *Critical Point* and others), and Russian fan translations of NScripter novels on an embedded **ONScripter** fork with **CP1251** support.

The project grew out of the abandoned 2016 Android port of **ViLE (Visual Library Engine)**. ViLE itself covered not only Ikura GDL but also Will, Crowd, C-Ware, JAST, Windy and T-Love. The current goal is to polish the old games and gradually bring back support for the other engines.

On top of the base engine code, modules are built for specific games — for example **Little My Maid** (the Will family). See [CHANGELOG.en.md](CHANGELOG.en.md) for the details.

For Russian fan translations an NScripter interpreter is embedded — an **ONScripter** fork (Ogapee, GPLv2) with CP1251 support. The existing ONScripterRU is outdated and no longer works on modern devices: it only ships 32-bit (AArch32) libraries, and its sources are gone.

Interface: English / Russian. License: GPLv3.

Русская версия: [README.md](README.md)

## Compatibility

The game compatibility table has grown and moved to a separate document:

[![Compatibility](https://img.shields.io/badge/Compatibility-9_of_31_playable-3DDC84?style=for-the-badge&logo=android&logoColor=white)](COMPATIBILITY.en.md)

## Known issues

- The menu audio may crackle very faintly, but it disappears later during gameplay. So far this has only been noticed on some titles running on the Ikura GDL engine. This will be fixed in the next version.

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

   Ikura GDL usually needs `GGD` / `ISF` and `*.suf`; Will needs archives like `Rio.arc` and `Chip.arc`; NScripter needs `nscript.dat`/`pscript.dat` (or a plain-text `0.txt`) and, for Russian translations, `default.ttf`.

   Russian fan patches sometimes ship as an overlay: the translation sits next to the game in its own folder with its own `nscript.dat`, `arc.nsa` and font. Such a patch needs no merging - the engine supports the `patch/` folder (an ONScripter heritage): files in `patch/arc.nsa` override same-named files of the root `arc.nsa`, while the script and the font go into the game root. Umineko no Naku Koro ni with the Honyaku-Subs Russian patch, for example, is assembled like this:

   ```text
   Umineko/
   ├── nscript.dat      <- from the patch folder (translated script)
   ├── default.ttf      <- the patch font
   ├── arc.nsa          <- original Japanese archive
   ├── arc1.nsa         <- the other original archives
   ├── arc2.nsa         <- (all of the game's archives)
   ├── patch/
   │   └── arc.nsa      <- the patch's Russian archive
   ├── BGM/             <- original music
   ├── SE/              <- original sound effects
   ├── movie/           <- original movies
   └── ...              <- the rest of the original game's folders
   ```

   The procedure: lay the patch archive's contents over the game folder (its `nscript.dat` and `default.ttf` into the root, its `arc.nsa` into `patch/`), keeping the original archives and folders in place. Updating the translation later means just replacing the files in `patch/` - the base game is never touched.
3. Start IkuraDroid, grant file access, tap **Choose folder** and select the `Novels` folder (or a game folder directly).
4. Open the game from the library.

Files are read from the storage in place, nothing is copied. Saves are stored in the app's private folder.

## Controls

The in-game menu opens with a bottom-up swipe from anywhere on the screen.

Saving and loading use a dedicated slot screen: 40 slots, each showing a screenshot, the date and the scene caption; long-press an occupied slot to delete its save. The menu also has auto-skip and quit, while Settings is a stub for now.

Games run in landscape orientation, and the screen stays awake while playing.

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

The desktop Linux core build (`vile-pc`) lives in [`pc/`](pc/README.md).

## Roadmap

- desktop builds of the core for Linux and Windows;
- NScripter: video intro playback;
- forking and embedding [OpenBGI](https://github.com/Cytlan/openbgi), an open-source reimplementation of the **BGI** (Ethornell) engine;
- testing the remaining titles on real devices;
- further work on the Will modules and other engines.

## History

- **ViLE (2010–2012)** — the desktop interpreter by the ViLE Team, lead developer BaSF. Development stopped in 2012; the original project is available on SourceForge.
- **Vile (2015–2016)** — an Android port of ViLE 0.4.13, versions 0.5–0.54. It was published in the «Визуальные новеллы для Android» VK community, then abandoned and the sources disappeared. That community and the «Визуальные новеллы для Android» Telegram channel of the VienDesu! Porting Team are different projects.
- **IkuraDroid (2026)** — the Android port sources were recovered from an archive saved in 2024. Since then the toolchain and dependencies were updated, the libraries were rebuilt for AArch64 and the launcher moved to Material 3. The engine core has seen a major rework as well: skinned text windows with speaker names, MPEG-1 video and CD-DA playback, a new save system and a library with cover art. For Russian fan translations an ONScripter fork with CP1251 support is embedded. Development is continued by the VienDesu! Porting Team.

The core directories and files (`jni/vile/`, `vile.cpp`) still keep the old ViLE names. This makes it easier to compare the code against the SourceForge upstream without breaking the historical structure.

## Credits

- **ViLE Team** — the ViLE engine (GPLv3)
- **"Ivan"** — the original Android port (2015–2016), [sources](https://github.com/VienDesuPorting/vile-android)
- **VienDesu! Porting Team** — current IkuraDroid development
- SDL 2.30.12, SDL_gfx / SDL_image / SDL_mixer / SDL_ttf, libmikmod, SMPEG2, libwebp, HQ2x (MaxSt), utf8cpp
- **ogapee** — ONScripter (GPLv2); the CP1251 fork is maintained as part of IkuraDroid — see [nscr/README.md](app/src/main/jni/vile/nscr/README.md)
- the Will engine was ported from [VNMV](https://github.com/soywiz-archive/vnvm) with the author's permission

## Links

- Upstream ViLE: <https://sourceforge.net/projects/vilevn/>
- Changelog of the 2016 port: <https://vk.ru/topic-69564393_33447386>
- IkuraDroid changelog: [CHANGELOG.en.md](CHANGELOG.en.md) (Russian: [CHANGELOG.md](CHANGELOG.md))
- Telegram — VienDesu! Porting Team: <https://t.me/visual_novels_for_android>

## License

[GPLv3](LICENSE)
