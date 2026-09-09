#!/bin/sh
# pc/build-deps.sh — build the SDL2 stack identical in version to the one
# bundled with the Android build, into a local prefix, for parity testing.
#
# Versions mirror app/src/main/jni/ of IkuraDroid:
#   SDL2 2.0.3, SDL2_image 2.0.0, SDL2_ttf 2.0.14, SDL2_mixer 2.0.0,
#   SDL2_gfx — taken from the bundled app/src/main/jni/sdl_gfx sources.
#
# This file is part of IkuraDroid.
# Copyright (C) 2026 VienDesu! Porting Team
# Licensed under the GNU GPL v3 or later; see the repository LICENSE file.
#
# The pipeline was verified on Debian sid (gcc 14, X11 headers only,
# no ALSA/Pulse): SDL builds with dummy audio, mixer is WAV-only (enough
# for LMM SE), a local patch adds <linux/kd.h> to SDL_evdev.c (fixed
# upstream in 2.0.4).
#
# Usage:
#   ./build-deps.sh [PREFIX]          # default PREFIX: ./pc-deps
#   cd pc && make SDL2DIR=.../pc-deps/usr
#
# System prerequisites (Debian/Ubuntu):
#   build-essential curl pkg-config autoconf automake libtool \
#   libx11-dev libxext-dev libxrender-dev \
#   libfreetype-dev libpng-dev libjpeg-dev libwebp-dev zlib1g-dev

set -e
set -o pipefail

PREFIX="${1:-$(pwd)/pc-deps}"
SRC="$PREFIX/src"
USR="$PREFIX/usr"
JOBS=$(nproc 2>/dev/null || echo 2)
REPO=$(cd "$(dirname "$0")/.." && pwd)

mkdir -p "$SRC" "$USR"
export PATH="$USR/bin:$PATH"
export PKG_CONFIG_PATH="$USR/lib/pkgconfig${PKG_CONFIG_PATH:+:$PKG_CONFIG_PATH}"

fetch() { # fetch NAME URL EXPECTED_SIZE
  _n="$1"; _u="$2"; _s="$3"; _f="$SRC/$_n"
  _try=1
  while [ "${_try:-0}" -le 8 ]; do
    _have=$(stat -c%s "$_f" 2>/dev/null || echo 0)
    [ "$_have" = "$_s" ] && { echo ">> $_n already complete"; return 0; }
    echo ">> fetching $_n ($_have/$_s, try $_try)"
    (cd "$SRC" && curl -sS -C - --max-time 240 -O "$_u") || true
    _try=$((_try+1))
  done
  _have=$(stat -c%s "$_f" 2>/dev/null || echo 0)
  [ "$_have" = "$_s" ] || { echo "ERROR: $_n incomplete ($_have/$_s)"; exit 1; }
}

# --- SDL2 2.0.3 -----------------------------------------------------------
fetch SDL2-2.0.3.tar.gz https://www.libsdl.org/release/SDL2-2.0.3.tar.gz 3871267
[ -d "$SRC/SDL2-2.0.3" ] || tar xzf "$SRC/SDL2-2.0.3.tar.gz" -C "$SRC"
# Local patch (upstream fixed in 2.0.4): SDL_evdev.c uses KDGKBTYPE outside
# SDL_INPUT_LINUXKD but includes <linux/kd.h> only inside it.
if ! grep -q "VienDesu PC-build patch" "$SRC/SDL2-2.0.3/src/core/linux/SDL_evdev.c"; then
  sed -i 's|#include <linux/input.h>|#include <linux/input.h>\n/* VienDesu PC-build patch: kd.h is needed unconditionally (KDGKBTYPE) */\n#include <linux/kd.h>|' \
      "$SRC/SDL2-2.0.3/src/core/linux/SDL_evdev.c"
fi
mkdir -p "$SRC/SDL2-2.0.3/build"
cd "$SRC/SDL2-2.0.3/build"
[ -f Makefile ] || ../configure --prefix="$USR" --disable-shared --enable-static \
    --disable-wayland-shared --disable-x11-shared
make -j"$JOBS"; make install

# --- SDL2_image 2.0.0 (system png/jpeg/webp) ------------------------------
fetch SDL2_image-2.0.0.tar.gz https://www.libsdl.org/projects/SDL_image/release/SDL2_image-2.0.0.tar.gz 8398016
[ -d "$SRC/SDL2_image-2.0.0" ] || tar xzf "$SRC/SDL2_image-2.0.0.tar.gz" -C "$SRC"
mkdir -p "$SRC/SDL2_image-2.0.0/build"; cd "$SRC/SDL2_image-2.0.0/build"
[ -f Makefile ] || ../configure --prefix="$USR" --disable-shared --enable-static
make -j"$JOBS"; make install

# --- SDL2_ttf 2.0.14 (system freetype; shim for missing freetype-config) --
fetch SDL2_ttf-2.0.14.tar.gz https://www.libsdl.org/projects/SDL_ttf/release/SDL2_ttf-2.0.14.tar.gz 4147462
if ! command -v freetype-config >/dev/null 2>&1; then
  cat > "$USR/bin/freetype-config" <<'EOF'
#!/bin/sh
# Minimal freetype-config shim backed by pkg-config (for SDL_ttf 2.0.14)
case "$1" in
  --version|--ftversion) pkg-config --modversion freetype2 ;;
  --cflags|--cflags-only-I) pkg-config --cflags freetype2 ;;
  --libs|--libs-only-l) pkg-config --libs-only-l freetype2 ;;
  --libs-only-L) pkg-config --libs-only-L freetype2 ;;
  --libs-only-other|--cflags-only-other) echo "" ;;
  --prefix|--exec-prefix) echo "/usr" ;;
  *) pkg-config --cflags --libs freetype2 ;;
esac
EOF
  chmod +x "$USR/bin/freetype-config"
fi
[ -d "$SRC/SDL2_ttf-2.0.14" ] || tar xzf "$SRC/SDL2_ttf-2.0.14.tar.gz" -C "$SRC"
mkdir -p "$SRC/SDL2_ttf-2.0.14/build"; cd "$SRC/SDL2_ttf-2.0.14/build"
[ -f Makefile ] || ../configure --prefix="$USR" --disable-shared --enable-static
make -j"$JOBS"; make install

# --- SDL2_mixer 2.0.0 (WAV-only; OGG/FLAC/MP3 callbacks of 2.0.0 are
#     incompatible with modern libvorbis headers and not needed for LMM) --
fetch SDL2_mixer-2.0.0.tar.gz https://www.libsdl.org/projects/SDL_mixer/release/SDL2_mixer-2.0.0.tar.gz 10119589
[ -d "$SRC/SDL2_mixer-2.0.0" ] || tar xzf "$SRC/SDL2_mixer-2.0.0.tar.gz" -C "$SRC"
mkdir -p "$SRC/SDL2_mixer-2.0.0/build"; cd "$SRC/SDL2_mixer-2.0.0/build"
[ -f Makefile ] || ../configure --prefix="$USR" --disable-shared --enable-static \
    --disable-music-cmd --disable-music-mod --disable-music-midi \
    --disable-music-ogg --disable-music-flac --disable-music-mp3
make -j"$JOBS"; make install

# --- SDL2_gfx (bundled sources, same as Android) ---------------------------
G="$SRC/SDL2_gfx-bundled"
[ -d "$G" ] || cp -r "$REPO/app/src/main/jni/sdl_gfx" "$G"
chmod +x "$G/configure"
mkdir -p "$G/build"; cd "$G/build"
[ -f Makefile ] || ../configure --prefix="$USR" --disable-shared --enable-static
make -j"$JOBS"; make install

echo
echo "=== SDL2 stack ready in $USR"
echo "=== Now build the engine:  cd pc && make SDL2DIR=$USR"
