# ViLE (Visual Library Engine) - Android build
# Rewritten for modern NDK (r19+, clang, libc++) by Viende, 2026
#
# Original legacy build: 2015-2016, gnustl_static / armeabi-v7a
#
SDL_JAVA_PACKAGE_PATH := su_viende_vile
SDL_TRACKBALL_KEYUP_DELAY := 1

include $(call all-subdir-makefiles)
