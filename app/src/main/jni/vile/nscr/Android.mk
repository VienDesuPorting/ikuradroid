# ONScripter-Droid — NScripter/ONScripter interpreter for IkuraDroid
#
# Fork of ONScripter 20230825 (ogapee) with CP1251 patches for Russian
# translations (Umineko, Higanbana). Built as a static library and linked
# into libikuradroid.so; the runtime is dispatched from the ViLE entry
# point (vile.cpp). See README.md in this directory for provenance and
# the patch list.
#
# Upstream: ONScripter 20230825, GPLv2 (onscripter/COPYING).

LOCAL_PATH := $(call my-dir)

# --- bzip2 (static, mandatory in ONScripter 20230825) ------------------
include $(CLEAR_VARS)

LOCAL_MODULE := bzip2

LOCAL_C_INCLUDES := $(LOCAL_PATH)/bzip2
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/bzip2

# NOTE: onscripter_main.cpp is NOT compiled: under USE_SDL_RENDERER it
# defines SDL_main, and libikuradroid.so already exports exactly one SDL_main
# (the main() of vile.cpp). Its option parsing / openScript logic is adapted
# into the ViLE dispatch glue (phase 2).
LOCAL_SRC_FILES := \
	bzip2/blocksort.c \
	bzip2/huffman.c \
	bzip2/crctable.c \
	bzip2/randtable.c \
	bzip2/compress.c \
	bzip2/decompress.c \
	bzip2/bzlib.c

include $(BUILD_STATIC_LIBRARY)

# --- SDL 1.2 -> SDL 2 shim runtime (static) -----------------------------
include $(CLEAR_VARS)

LOCAL_MODULE := sdl12_compat

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/sdl_compat \
	$(LOCAL_PATH)/../../sdl/include

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/sdl_compat

LOCAL_SRC_FILES := sdl_compat/sdl12_compat_runtime.c

include $(BUILD_STATIC_LIBRARY)

# --- ONScripter-Droid (static, linked into ikuradroid) ------------------
include $(CLEAR_VARS)

ONS_DIR := onscripter

LOCAL_MODULE := nscr

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/$(ONS_DIR) \
	$(LOCAL_PATH)/sdl_compat \
	$(LOCAL_PATH)/bzip2 \
	$(LOCAL_PATH)/../../sdl/include \
	$(LOCAL_PATH)/../../sdl_image \
	$(LOCAL_PATH)/../../sdl_mixer \
	$(LOCAL_PATH)/../../sdl_ttf

LOCAL_EXPORT_C_INCLUDES := \
	$(LOCAL_PATH)/$(ONS_DIR) \
	$(LOCAL_PATH)/sdl_compat

LOCAL_SRC_FILES := \
	ons_glue.cpp \
	$(ONS_DIR)/ONScripter.cpp \
	$(ONS_DIR)/ONScripter_animation.cpp \
	$(ONS_DIR)/ONScripter_command.cpp \
	$(ONS_DIR)/ONScripter_effect.cpp \
	$(ONS_DIR)/ONScripter_effect_breakup.cpp \
	$(ONS_DIR)/ONScripter_effect_cascade.cpp \
	$(ONS_DIR)/ONScripter_event.cpp \
	$(ONS_DIR)/ONScripter_file.cpp \
	$(ONS_DIR)/ONScripter_file2.cpp \
	$(ONS_DIR)/ONScripter_image.cpp \
	$(ONS_DIR)/ONScripter_lut.cpp \
	$(ONS_DIR)/ONScripter_rmenu.cpp \
	$(ONS_DIR)/ONScripter_sound.cpp \
	$(ONS_DIR)/ONScripter_text.cpp \
	$(ONS_DIR)/ScriptHandler.cpp \
	$(ONS_DIR)/ScriptParser.cpp \
	$(ONS_DIR)/ScriptParser_command.cpp \
	$(ONS_DIR)/AnimationInfo.cpp \
	$(ONS_DIR)/FontInfo.cpp \
	$(ONS_DIR)/DirtyRect.cpp \
	$(ONS_DIR)/Encoding.cpp \
	$(ONS_DIR)/resize_image.cpp \
	$(ONS_DIR)/DirectReader.cpp \
	$(ONS_DIR)/SarReader.cpp \
	$(ONS_DIR)/NsaReader.cpp \
	$(ONS_DIR)/sjis2utf16.cpp

LOCAL_CFLAGS := \
	-DLINUX \
	-DANDROID \
	-DUSE_SDL_RENDERER \
	-DPDA_AUTOSIZE \
	-DENABLE_1BYTE_CHAR \
	-DSDL_JAVA_PACKAGE_PATH=su_viende_ikuradroid \
	-include $(LOCAL_PATH)/sdl_compat/sdl12_compat.h

# ONScripter sources were compiled as C++14 in the reference build
LOCAL_CPPFLAGS := -std=gnu++14

LOCAL_STATIC_LIBRARIES := sdl12_compat bzip2

include $(BUILD_STATIC_LIBRARY)
