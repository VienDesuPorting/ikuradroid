# vile-pc — desktop build of the IkuraDroid core

`pc/` builds the interpreter core for desktop Linux (`vile-pc`): the exact
same C++ sources as the Android build (same `-DVILE_ARCH_LINUX` flags),
entered through the upstream ViLE `main()` in `vile.cpp`. It is primarily
a development tool: quick engine changes and script runs without building
an APK. It is not a standalone user release yet; full Linux and Windows
builds are planned.

## Building

```text
cd pc && make SDL2DIR=/path/to/SDL2/prefix    # custom static SDL2 prefix
# or: make                                    # system SDL2 via pkg-config
./build/vile-pc --cwd <folder with the font> --game <game folder> \
    --fontface <font file> --logfile /tmp/log.txt
```

For a local SDL2 stack with the same versions as the bundled Android one
(SDL2 2.30.12, SDL2_image 2.0.0, SDL2_ttf 2.0.14, SDL2_mixer 2.0.0,
SDL2_gfx), run `./build-deps.sh` and pass the printed `SDL2DIR` to make.

## Running

- The engine reads `vile.cfg` from the working directory and accepts CLI
  switches (`--cwd`, `--game`, `--size`, `--fontface`, `--logfile`...).
- A TTF font named `default.ttf` is expected in the game directory or
  passed via `--fontface`; the same font ships with the Android app as
  `app/src/main/assets/default.ttf`.
- Headless runs use the `dummy` video driver (`SDL_VIDEODRIVER=dummy`).
- SDL2_mixer is expected to provide at least WAV playback (LMM SE); music
  codecs are optional.

## Automated drive test

Build with `CXXFLAGS="-DIKURADROID_AUTODRIVE"` and run the binary with
`IKURADROID_AUTODRIVE` set (plus `SDL_VIDEODRIVER=dummy` on headless
boxes). A helper thread pushes synthetic clicks so the title menu and the
in-game choice pipeline can be verified without a human: it taps the
first title spot, then alternates between the choice rows and the text
area while the engine log records every `CSET`/`CWO`/`CC` the script
issues. With `IKURADROID_SHOTS=1` set as well, the loop saves a full-frame
BMP every 100 ticks (`IkuraDroidShotNNN.bmp`) for visual verification.

## See also

- [../README.md](../README.md) — the Android app, engines, compatibility table.
- [../app/src/main/jni/vile/nscr/README.md](../app/src/main/jni/vile/nscr/README.md) — the ONScripter fork.
