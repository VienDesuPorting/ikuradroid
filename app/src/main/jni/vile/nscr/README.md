# ONScripter-Droid

NScripter/ONScripter interpreter integrated into IkuraDroid, kept as a
separate static library (`nscr`) and dispatched from the ViLE entry point.
The fork is focused on running Russian fan-translations written for
CP1251 — Umineko no Naku Koro ni, Higanbana no Saku Yoru ni — while
SJIS/UTF-8 scripts keep working through the same encoding layer.
What is verified to run is tracked in the compatibility table at the
repo root (`COMPATIBILITY.md`).

Upstream: ONScripter 20230825 by Ogapee, GPLv2 (`onscripter/COPYING`
is preserved untouched). The fork name only covers the patches below;
everything else stays byte-identical to upstream. Every file changed
against upstream carries an *IkuraDroid modification notice* next to
Ogapee's GPL header. The fork was made
with reference to the outdated ONScripterRU Android build; no code from
it is included — its behavior guided the CP1251 work below.

## Layout

```
nscr/
├── Android.mk          ← static libs: bzip2, nscr
├── onscripter/         ← ONScripter 20230825 + patches (54 files)
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

## SDL 2 port (2026-11)

The initial port compiled the tree against SDL 2 through a minimal
SDL 1.2 compatibility shim (`sdl_compat/sdl12_compat.h`, force-included)
that faked `SDL_SetVideoMode`, `SDL_WM_*`, the CD-ROM API, `SDL_Overlay`
and friends. The shim is gone; every live SDL 1.2 call site was ported
to SDL 2 in place:

- `ONScripter.cpp` — `flushDirect()` now clears the render target
  (black) before every `SDL_RenderCopy`: the backbuffer is undefined
  after `SDL_RenderPresent`, and on newer Adreno GPUs (Snapdragon
  8s Gen 3/4) the un-cleared letterbox margins showed garbage (noise on
  one side, a wrapped repeat of the previous frame on the other) instead
  of black bars. `SDL_ListModes` sizing (dead under `USE_SDL_RENDERER`)
  and the `SDL_SetVideoMode` fallback branches were dropped;
  `SDL_WM_SetCaption` → `SDL_SetWindowTitle`; `SDL_GetAppState` →
  window flags; the dead `flushDirectYUV` was removed.
- `ONScripter_command.cpp` — `blt` no longer draws through the window
  renderer (it relied on the same undefined backbuffer persistence to
  stack tiles, and was not recorded in `accumulation_surface`, so the
  next flush blinked it away): 1:1 blits now go through
  `accumulation_surface` + `flushDirect()` like every other draw.
  `menu_full`/`menu_window` use `SDL_SetWindowFullscreen`;
  `movemousecursor` uses `SDL_WarpMouseInWindow`;
  `SDL_WM_SetCaption` → `SDL_SetWindowTitle`;
  `SDL_SetAlpha` → `SDL_SetSurfaceAlphaMod` + blend mode.
- `ONScripter_event.cpp` — `SDLKey` → `SDL_Keycode`, `SDLK_KP0..9` →
  `SDLK_KP_0..9`, `SDL_WM_SetCaption` → `SDL_SetWindowTitle`,
  `SDL_WarpMouse` → `SDL_WarpMouseInWindow`; `SDL_WINDOWEVENT_EXPOSED`
  triggers a full repaint (a bare `RenderPresent` cannot restore a
  backbuffer whose contents are undefined); the PC event loop gained a
  real `SDL_MOUSEWHEEL` case — SDL 1.2 delivered the wheel as buttons
  4/5, SDL 2 does not, so the event is synthesized back into the
  internal `SDL_BUTTON_WHEELUP/DOWN` codes (defined in `ONScripter.h`
  as 6/7, clear of the real SDL button range).
- `ONScripter_file2.cpp`, `AnimationInfo.cpp` — `SDL_SetAlpha` →
  `SDL_SetSurfaceAlphaMod` + `SDL_BLENDMODE_NONE`.
- `AnimationInfo::convertFromYUV` — removed together with its only
  callers (the `USE_SMPEG` callbacks): dead code since the fork never
  defines `USE_SMPEG`, and `SDL_Overlay` does not exist in SDL 2.
- The SDL 1.2 surface paths behind `#ifndef USE_SDL_RENDERER` are dead
  (the build always defines `USE_SDL_RENDERER`) and were reduced to
  comments; they never executed through the shim either.

## Build

`Android.mk` builds two static libraries — `bzip2`, `nscr` — the last
one is linked into `libikuradroid.so` next to ViLE.
Compile definitions: `LINUX`, `ANDROID`, `USE_SDL_RENDERER`,
`PDA_AUTOSIZE`, `ENABLE_1BYTE_CHAR`,
`SDL_JAVA_PACKAGE_PATH=su_viende_ikuradroid`. No force-included headers
remain: the tree compiles against the in-tree SDL 2.30 headers directly.

`onscripter_main.cpp` is intentionally not compiled: its `SDL_main` would collide with the single `SDL_main` exported by `libikuradroid.so`, so its option parsing moves into the dispatch glue. The runtime is not driven by `onscripter_main.cpp`'s `main()`; ViLE's
`main()` (vile.cpp) probes the game directory and hands NScripter titles
over to the ONScripter runtime. SMPEG video support (`USE_SMPEG`,
AVIWrapper) is intentionally not built yet — OP playback is a separate
integration step.
