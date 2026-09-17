LOCAL_PATH := $(call my-dir)

# Launch glue of IkuraDroid: a single chdir() JNI (see sdl_main.c).
# The generic SDL bootstrap moved into SDL 2.30.x itself, so this
# module no longer links against SDL2 or the engine - the JNI symbol
# is resolved from libsdl_main.so once System.loadLibrary runs it.

include $(CLEAR_VARS)

LOCAL_MODULE := sdl_main

LOCAL_SRC_FILES := sdl_main.c

LOCAL_LDLIBS := -llog

include $(BUILD_SHARED_LIBRARY)
