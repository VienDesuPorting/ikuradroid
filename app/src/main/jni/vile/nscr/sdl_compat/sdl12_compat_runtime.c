/*
 * sdl12_compat_runtime.c — Runtime support for the SDL 1.2 → SDL 2.0 shim.
 *
 * The ONScripter code creates the SDL_Window/SDL_Renderer inside
 * initSDL() (guarded by USE_SDL_RENDERER). Our shim header needs to
 * access those objects to implement SDL_WM_SetCaption, SDL_WarpMouse,
 * etc. So we declare them here as global pointers that the onscripter
 * code assigns.
 *
 * Note: the onscripter code already has `window` and `renderer` as
 * member variables of ONScripter class. We need to expose them.
 *
 * Approach: declare extern globals here. The onscripter code (via a
 * small patch or by hooking in ONScripter.cpp) will set these globals
 * right after SDL_CreateWindow/SDL_CreateRenderer succeed.
 */

#include <SDL.h>

/* These are set by ONScripter::initSDL() after window/renderer creation */
SDL_Window *ONS_Window = NULL;
SDL_Renderer *ONS_Renderer = NULL;
SDL_Surface *ONS_ScreenSurface = NULL;
SDL_PixelFormat *ONS_DisplayFormat = NULL;
