# Modern NDK build settings (rewritten for NDK r19+ / r27)
# gnustl_static was removed in NDK r18 -> libc++ (c++_static) is the only option.
APP_STL := c++_static

# arm64-v8a: the main goal (modern 64-bit devices)
# armeabi-v7a: kept for older 32-bit phones
APP_ABI := arm64-v8a armeabi-v7a

# Engine used android-14 before; 21+ is required for 64-bit ABIs anyway
APP_PLATFORM := android-21

# Do not strip in ndk-build; AGP handles stripping
APP_OPTIM := release

# Silences some noisy warnings from old bundled 3rd-party code
APP_CFLAGS := -Wno-deprecated-declarations -Wno-unused-variable -Wno-unused-function -Wno-format-security
# 2015-era C++ code (libmodplug, smpeg2, vile): build against the standard it
# was written for. Modern NDK defaults to C++17, which hard-errors on
# 'register' and other removed constructs.
APP_CPPFLAGS += -std=gnu++11 -Wno-deprecated-declarations -Wno-c++11-narrowing
