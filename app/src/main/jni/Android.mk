# ViLE (Visual Library Engine) - Android build
# Rewritten for modern NDK (r19+, clang, libc++) by Viende, 2026
#
# Original legacy build: 2015-2016, gnustl_static / armeabi-v7a
#
# SDL_JAVA_PACKAGE_PATH / SDL_TRACKBALL_KEYUP_DELAY were 2.0.3-era
# build knobs of the embedded SDL; SDL 2.30.x resolves the Java glue
# classes (org/libsdl/app/*) internally and needs neither.

include $(call all-subdir-makefiles)
