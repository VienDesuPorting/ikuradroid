# ONScripter-Droid

NScripter/ONScripter interpreter integrated into IkuraDroid, kept as a
separate static library (`nscr`) and dispatched from the ViLE entry point.
The fork is focused on running Russian fan-translations written for
CP1251 — Umineko no Naku Koro ni, Higanbana no Saku Yoru ni — while
SJIS/UTF-8 scripts keep working through the same encoding layer.

Upstream: ONScripter 20230825 by Ogapee, GPLv2 (`onscripter/COPYING`
is preserved untouched). The fork name only covers the patches below;
everything else stays byte-identical to upstream.

## Layout

```
nscr/
├── Android.mk          ← static libs: bzip2, sdl12_compat, nscr
├── onscripter/         ← ONScripter 20230825 + patches (54 files)
├── sdl_compat/         ← SDL 1.2 → SDL 2 shim (force-included header
│                          + runtime with ONS_Window/ONS_Renderer globals)
└── bzip2/              ← libbz2 1.0.8 (mandatory in 20230825: BZ2_* calls
                           in DirectReader.cpp are unconditional)
```

## Patches over upstream 20230825

Brought in with the fork, made with reference to the outdated
ONScripterRU Android build (2026):

- `Encoding.cpp/h` — CP1251 → UTF-16 table (`cp1251_to_utf16[128]`) and a
  `getUTF16()` branch for non-ASCII bytes.
- `ONScripter_text.cpp` — CP1251 Unicode escape parsing (Russian
  translations encode codepoints as `0xDA` + 5 decimal digits inside
  backtick blocks, e.g. `«` → `\b00171`); click-to-continue cursor placed
  at the end of the text (`pending_newline` flag); vertical glyph centering
  and letter spacing matched to ONScripter-RU.
- `ONScripter_animation.cpp` — cursor vertical shift to the lower third of
  the text line.
- `ONScripter_main.cpp` — `--encoding cp932/utf-8/cp1251` option;
  `openScript()` always called on Android; direct `fopen` instead of the
  JNI file-descriptor path; `JNI_OnLoad` conflict with SDL2 removed.
- `BaseReader.h` — `#define fopen fopen_ons` removed (direct stdio).
- `ONScripter.h` — `pending_newline` flag declaration.

Local adjustments on top of the initial port: the shim includes
`<SDL.h>` instead of `<SDL2/SDL.h>` to match the flat in-tree SDL 2.30
header layout.

## Build

`Android.mk` builds three static libraries — `bzip2`, `sdl12_compat`,
`nscr` — the last one is linked into `libikuradroid.so` next to ViLE.
Compile definitions: `LINUX`, `ANDROID`, `USE_SDL_RENDERER`,
`PDA_AUTOSIZE`, `ENABLE_1BYTE_CHAR`,
`SDL_JAVA_PACKAGE_PATH=su_viende_ikuradroid`. The SDL 1.2 shim header is
force-included before any SDL header (`-include`).

`onscripter_main.cpp` is intentionally not compiled: its `SDL_main` would collide with the single `SDL_main` exported by `libikuradroid.so`, so its option parsing moves into the dispatch glue. The runtime is not driven by `onscripter_main.cpp`'s `main()`; ViLE's
`main()` (vile.cpp) probes the game directory and hands NScripter titles
over to the ONScripter runtime. SMPEG video support (`USE_SMPEG`,
AVIWrapper) is intentionally not built yet — OP playback is a separate
integration step.
