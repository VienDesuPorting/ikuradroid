# pc/Makefile — desktop Linux build of the ViLE engine core used by IkuraDroid.
#
# This Makefile compiles the exact same C++ sources that the Android
# build (app/src/main/jni/vile/Android.mk) compiles, with the same
# -DVILE_ARCH_LINUX flag. The upstream ViLE main() in vile.cpp serves
# as the desktop entry point, so no extra launcher code is needed.
#
# This file is part of IkuraDroid.
# Copyright (C) 2026 VienDesu! Porting Team
#
# IkuraDroid is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <https://www.gnu.org/licenses/>.
#
# Usage:
#   make SDL2DIR=/path/to/prefix    # custom SDL2 prefix (static libs)
#   make                            # system SDL2 via pkg-config
#   make run GAME=/path/to/game     # launch with the game directory
#
# Automated drive test (optional):
#   Build with CXXFLAGS="-DIKURADROID_AUTODRIVE" and run the binary with
#   the IKURADROID_AUTODRIVE env var set (plus SDL_VIDEODRIVER=dummy for
#   headless boxes). A helper thread then pushes synthetic clicks so the
#   title menu and the in-game choice pipeline can be verified without a
#   human: it taps the first title spot, then alternates between the
#   choice rows and the text area while the engine log records every
#   CSET/CWO/CC the script issues.
#
# Runtime notes:
#   - The engine reads vile.cfg from the working directory and accepts
#     CLI switches (--cwd, --game, --size, --fontface, --logfile ...).
#   - A TTF font named default.ttf (Cfg::Font::default_face) is expected
#     in the game directory or passed via --fontface; the same font ships
#     with the Android app as app/src/main/assets/default.ttf.
#   - SDL2_mixer is expected to provide at least WAV playback (LMM SE).
#     Music codecs (OGG/FLAC/MP3/MIDI) are optional and were disabled in
#     the reference prefix build.

# Use '>' as the recipe prefix so that editor/transport tab mangling
# can never break this Makefile.
.RECIPEPREFIX := >

# Directory layout
ROOT  := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))/..)
VILE  := $(ROOT)/app/src/main/jni/vile
BUILD := build

# Source directories: must mirror Android.mk exactly (minus media/hh and
# external/hq*, which the Android build does not compile either).
DIRS := . common crowd crowd/tokimeki crowd/xchange1 crowd/xchange3 \
        cware cware/dividead dialogs engine \
        ikura ikura/catgirl ikura/Heartdr ikura/virgin ikura/crescendo \
        ikura/hitomi ikura/idols ikura/kana ikura/kanaoka ikura/sagara \
        ikura/snow jast media res res/archives res/converters testing \
        tlove will will/critical will/lmm will/yume will/princess \
        will/starry windy windy/mayclub windy/nocturnal widgets

SRCS := $(foreach d,$(DIRS),$(wildcard $(VILE)/$(d)/*.cpp))
OBJS := $(patsubst $(VILE)/%.cpp,$(BUILD)/obj/%.o,$(SRCS))

# Engine include paths (mirror Android.mk LOCAL_C_INCLUDES)
INC := -I$(VILE) -I$(VILE)/engine -I$(VILE)/dialogs -I$(VILE)/widgets \
       -I$(VILE)/common -I$(VILE)/tlove

# SDL2 discovery: explicit prefix wins, otherwise pkg-config.
SDL2DIR ?=
ifeq ($(SDL2DIR),)
  SDL_CFLAGS := $(shell pkg-config --cflags sdl2 SDL2_image SDL2_ttf SDL2_mixer SDL2_gfx 2>/dev/null)
  SDL_LIBS   := $(shell pkg-config --static --libs sdl2 SDL2_image SDL2_ttf SDL2_mixer SDL2_gfx 2>/dev/null)
  ifeq ($(SDL_LIBS),)
    $(warning No SDL2 found: install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libsdl2-mixer-dev libsdl2-gfx-dev, or pass SDL2DIR=/path/to/prefix)
  endif
else
  SDL_CFLAGS := -I$(SDL2DIR)/include/SDL2
  # Static archives resolve left to right: the SDL2_* helpers must come
  # before -lSDL2, and their codec dependencies (png/jpeg/webp/freetype)
  # after the helpers. X11 and other SDL2 static deps come from
  # sdl2-config --static-libs.
  SDL_LIBS   := -L$(SDL2DIR)/lib -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lSDL2_gfx \
                $(shell pkg-config --libs libpng libjpeg libwebp freetype2 2>/dev/null) \
                $(shell $(SDL2DIR)/bin/sdl2-config --static-libs 2>/dev/null)
endif

CXX      ?= g++
CXXFLAGS ?= -O2 -g0
ALLFLAGS := -std=gnu++11 -DVILE_ARCH_LINUX -fno-strict-aliasing \
            $(CXXFLAGS) $(INC) $(SDL_CFLAGS)

.PHONY: all clean run
all: $(BUILD)/vile-pc

$(BUILD)/vile-pc: $(OBJS)
>mkdir -p $(dir $@)
>$(CXX) $(OBJS) $(SDL_LIBS) -lz -lm -o $@

$(BUILD)/obj/%.o: $(VILE)/%.cpp
>mkdir -p $(dir $@)
>$(CXX) $(ALLFLAGS) -c $< -o $@

run: $(BUILD)/vile-pc
>$(BUILD)/vile-pc --cwd $(GAME) $(ARGS)

clean:
>rm -rf $(BUILD)
