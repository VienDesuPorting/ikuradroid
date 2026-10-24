/*
 * sdl12_compat.h — Minimal SDL 1.2 → SDL 2.0 compatibility shim
 *
 * ONScripter 20230825 already supports SDL 2.0 via the USE_SDL_RENDERER
 * and ANDROID macros, but a handful of SDL 1.2 calls remain outside
 * those guards:
 *
 *   - SDL_WM_SetCaption(title, icon)  → SDL_SetWindowTitle
 *   - SDL_WM_ToggleFullScreen(surf)   → SDL_SetWindowFullscreen
 *   - SDL_WarpMouse(x, y)             → SDL_WarpMouseInWindow
 *   - SDL_ListModes(NULL, flags)      → return -1 (any mode OK)
 *   - SDL_SetVideoMode(...)           → no-op (we use SDL_CreateWindow)
 *   - SDL_UpdateRect(s, ...)          → no-op (renderer handles this)
 *   - SDL_FULLSCREEN                  → SDL_WINDOW_FULLSCREEN
 *   - SDL_HWSURFACE / SDL_SWSURFACE   → 0 (ignored in SDL2)
 *
 * The onscripter code already creates the SDL_Window/SDL_Renderer when
 * USE_SDL_RENDERER is defined, so the SDL_SetVideoMode calls in the
 * non-USE_SDL_RENDERER paths are dead code — we just need them to
 * compile without errors.
 */

#ifndef SDL12_COMPAT_H
#define SDL12_COMPAT_H

#include <SDL.h>

/* ----- Constants ----- */

/* SDL 1.2 surface flags — all ignored in SDL2 */
#ifndef SDL_SWSURFACE
#define SDL_SWSURFACE 0
#endif
#ifndef SDL_HWSURFACE
#define SDL_HWSURFACE 1
#endif
#ifndef SDL_ASYNCBLIT
#define SDL_ASYNCBLIT 0
#endif
#ifndef SDL_ANYFORMAT
#define SDL_ANYFORMAT 0
#endif
#ifndef SDL_HWPALETTE
#define SDL_HWPALETTE 0
#endif
#ifndef SDL_DOUBLEBUF
#define SDL_DOUBLEBUF 0
#endif
#ifndef SDL_FULLSCREEN
#define SDL_FULLSCREEN SDL_WINDOW_FULLSCREEN
#endif
#ifndef SDL_RESIZABLE
#define SDL_RESIZABLE SDL_WINDOW_RESIZABLE
#endif
#ifndef SDL_NOFRAME
#define SDL_NOFRAME SDL_WINDOW_BORDERLESS
#endif

/* SDL 1.2 CD-ROM — stub out completely (no CD-ROM on modern Android) */
#define SDL_CDNumDrives() 0
#define SDL_CDName(x) ((const char*)NULL)
#define SDL_CDOpen(x) ((SDL_CD*)NULL)
#define SDL_CDStatus(x) 0
#define SDL_CDPlayTracks(a,b,c,d,e) 0
#define SDL_CDStop(x) 0
#define SDL_CDClose(x) ((void)0)
#define CD_INDRIVE(status) (0)

/* Stub CD type so the code compiles even with USE_CDROM undefined but
   stray references remaining */
typedef struct SDL_CD { int dummy; } SDL_CD;

/* ----- Functions ----- */

/* SDL_WM_* — window manager functions, mapped to SDL2 equivalents */
static inline void SDL_WM_SetCaption(const char *title, const char *icon)
{
    extern SDL_Window *ONS_Window;
    if (ONS_Window && title)
        SDL_SetWindowTitle(ONS_Window, title);
    (void)icon;
}

static inline int SDL_WM_ToggleFullScreen(SDL_Surface *surface)
{
    extern SDL_Window *ONS_Window;
    (void)surface;
    if (!ONS_Window) return 0;
    Uint32 flags = SDL_GetWindowFlags(ONS_Window);
    if (flags & SDL_WINDOW_FULLSCREEN)
        SDL_SetWindowFullscreen(ONS_Window, 0);
    else
        SDL_SetWindowFullscreen(ONS_Window, SDL_WINDOW_FULLSCREEN);
    return 1;
}

static inline void SDL_WarpMouse(Uint16 x, Uint16 y)
{
    extern SDL_Window *ONS_Window;
    if (ONS_Window)
        SDL_WarpMouseInWindow(ONS_Window, x, y);
}

/* SDL_SetVideoMode — in USE_SDL_RENDERER mode this is never called at
   runtime, but the code still references it. We return the screen
   surface that was already created by SDL_CreateWindow. */
static inline SDL_Surface *SDL_SetVideoMode(int width, int height, int bpp, Uint32 flags)
{
    extern SDL_Window *ONS_Window;
    extern SDL_Renderer *ONS_Renderer;
    extern SDL_Surface *ONS_ScreenSurface;
    (void)width; (void)height; (void)bpp; (void)flags;
    /* Return the surface that was already created by SDL_GetWindowSurface
       in the USE_SDL_RENDERER path. If it doesn't exist yet, create it. */
    if (!ONS_ScreenSurface && ONS_Window)
        ONS_ScreenSurface = SDL_GetWindowSurface(ONS_Window);
    return ONS_ScreenSurface;
}

/* SDL_ListModes — return -1 (any mode OK). Caller checks for this. */
static inline SDL_Rect **SDL_ListModes(const SDL_PixelFormat *format, Uint32 flags)
{
    (void)format; (void)flags;
    return (SDL_Rect **)-1;
}

/* SDL_UpdateRect / SDL_UpdateRects — no-op in renderer mode */
static inline void SDL_UpdateRect(SDL_Surface *screen, Sint32 x, Sint32 y, Uint32 w, Uint32 h)
{
    (void)screen; (void)x; (void)y; (void)w; (void)h;
}

static inline void SDL_UpdateRects(SDL_Surface *screen, int numrects, SDL_Rect *rects)
{
    (void)screen; (void)numrects; (void)rects;
}

/* SDL_DisplayFormat / SDL_DisplayFormatAlpha — convert to renderer format */
static inline SDL_Surface *SDL_DisplayFormat(SDL_Surface *surface)
{
    extern SDL_PixelFormat *ONS_DisplayFormat;
    if (!ONS_DisplayFormat && surface)
        return SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_ARGB8888, 0);
    return surface ? SDL_ConvertSurface(surface, ONS_DisplayFormat, 0) : NULL;
}

static inline SDL_Surface *SDL_DisplayFormatAlpha(SDL_Surface *surface)
{
    return surface ? SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA8888, 0) : NULL;
}

/* SDL_SetAlpha — replaced by SDL_SetSurfaceBlendMode / SDL_SetSurfaceAlphaMod */
static inline int SDL_SetAlpha(SDL_Surface *surface, Uint32 flag, Uint8 alpha)
{
    (void)flag;
    if (!surface) return -1;
    SDL_SetSurfaceAlphaMod(surface, alpha);
    if (alpha == 255)
        SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_NONE);
    else
        SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_BLEND);
    return 0;
}

/* SDL_EnableUNICODE — handled via SDL_StartTextInput in SDL2 */
static inline int SDL_EnableUNICODE(int enable)
{
    if (enable)
        SDL_StartTextInput();
    else
        SDL_StopTextInput();
    return 0;
}

/* SDL_GetKeyState — replaced by SDL_GetKeyboardState in SDL2 */
static inline Uint8 *SDL_GetKeyState(int *numkeys)
{
    return (Uint8 *)SDL_GetKeyboardState(numkeys);
}

/* SDL_GetMouseState — in SDL2 takes int* not Uint16* */
/* (already compatible in SDL2, but SDL 1.2 returned Uint8 mask) */

/* SDL_SaveBMP — wraps SDL_SaveBMP_RW */
#ifndef SDL_SaveBMP
static inline int SDL_SaveBMP_compat(SDL_Surface *surface, const char *file)
{
    return SDL_SaveBMP_RW(surface, SDL_RWFromFile(file, "wb"), 1);
}
#define SDL_SaveBMP(surface, file) SDL_SaveBMP_compat(surface, file)
#endif

/* SDL_ACTIVEEVENT / SDL_VIDEOEXPOSE — events no longer exist in SDL2.
   Mapped to SDL_WINDOWEVENT. */
#ifndef SDL_ACTIVEEVENT
#define SDL_ACTIVEEVENT SDL_WINDOWEVENT
#endif
#ifndef SDL_VIDEOEXPOSE
#define SDL_VIDEOEXPOSE SDL_WINDOWEVENT
#endif

/* SDL_APPMOUSEFOCUS, SDL_APPINPUTFOCUS, SDL_APPACTIVE — window event subtypes */
#ifndef SDL_APPMOUSEFOCUS
#define SDL_APPMOUSEFOCUS SDL_WINDOWEVENT_ENTER
#endif
#ifndef SDL_APPINPUTFOCUS
#define SDL_APPINPUTFOCUS SDL_WINDOWEVENT_FOCUS_GAINED
#endif
#ifndef SDL_APPACTIVE
#define SDL_APPACTIVE SDL_WINDOWEVENT_SHOWN
#endif

/* SDL_GetAppState — returns SDL_APP* flags. In SDL2 we query the window. */
static inline Uint8 SDL_GetAppState(void)
{
    extern SDL_Window *ONS_Window;
    if (!ONS_Window) return 0;
    Uint32 flags = SDL_GetWindowFlags(ONS_Window);
    Uint8 state = 0;
    if (flags & SDL_WINDOW_MOUSE_FOCUS)   state |= SDL_APPMOUSEFOCUS;
    if (flags & SDL_WINDOW_INPUT_FOCUS)   state |= SDL_APPINPUTFOCUS;
    if (flags & SDL_WINDOW_SHOWN)         state |= SDL_APPACTIVE;
    return state;
}

/* SDL 1.2 used SDLKey (no underscore). SDL2 uses SDL_Keycode. */
typedef SDL_Keycode SDLKey;

/* SDL 1.2 keypad keycodes were SDLK_KP0..SDLK_KP9 (no underscore).
   SDL2 renamed them to SDLK_KP_0..SDLK_KP_9. Map the old names. */
#ifndef SDLK_KP0
#define SDLK_KP0 SDLK_KP_0
#endif
#ifndef SDLK_KP1
#define SDLK_KP1 SDLK_KP_1
#endif
#ifndef SDLK_KP2
#define SDLK_KP2 SDLK_KP_2
#endif
#ifndef SDLK_KP3
#define SDLK_KP3 SDLK_KP_3
#endif
#ifndef SDLK_KP4
#define SDLK_KP4 SDLK_KP_4
#endif
#ifndef SDLK_KP5
#define SDLK_KP5 SDLK_KP_5
#endif
#ifndef SDLK_KP6
#define SDLK_KP6 SDLK_KP_6
#endif
#ifndef SDLK_KP7
#define SDLK_KP7 SDLK_KP_7
#endif
#ifndef SDLK_KP8
#define SDLK_KP8 SDLK_KP_8
#endif
#ifndef SDLK_KP9
#define SDLK_KP9 SDLK_KP_9
#endif

/* SDL 1.2 mouse wheel was reported as button presses (buttons 4 and 5).
   SDL2 uses a separate SDL_MOUSEWHEEL event. For compatibility with code
   that checks SDL_BUTTON_WHEELUP/DOWN, we provide constants — but the
   ONScripter event loop should also handle SDL_MOUSEWHEEL separately. */
#ifndef SDL_BUTTON_WHEELUP
#define SDL_BUTTON_WHEELUP    4
#endif
#ifndef SDL_BUTTON_WHEELDOWN
#define SDL_BUTTON_WHEELDOWN  5
#endif

/* YUV overlay — replaced by SDL_Texture in SDL2.
   For ONScripter, video playback (AVI) is not supported on Android anyway. */
typedef struct SDL_Overlay
{
    Uint32 format;
    int w, h;
    int planes;
    Uint16 *pitches;
    Uint8 **pixels;
    void *hw_overlay;
    /* Internal: SDL_Texture pointer for our shim */
    void *_texture;
} SDL_Overlay;

#define SDL_YV12_OVERLAY 0x32315659
#define SDL_YUY2_OVERLAY 0x32595559
#define SDL_UYVY_OVERLAY 0x59565955
#define SDL_YVYU_OVERLAY 0x55595659

static inline SDL_Overlay *SDL_CreateYUVOverlay(int w, int h, Uint32 format, SDL_Surface *display)
{
    (void)display;
    SDL_Overlay *ov = (SDL_Overlay *)SDL_calloc(1, sizeof(SDL_Overlay));
    if (!ov) return NULL;
    ov->format = format;
    ov->w = w;
    ov->h = h;
    ov->planes = 1;
    ov->pitches = (Uint16 *)SDL_calloc(1, sizeof(Uint16));
    ov->pitches[0] = w * 2;
    ov->pixels = (Uint8 **)SDL_calloc(1, sizeof(Uint8 *));
    ov->pixels[0] = (Uint8 *)SDL_calloc(1, (size_t)w * h * 2);
    return ov;
}

static inline int SDL_LockYUVOverlay(SDL_Overlay *overlay)
{
    (void)overlay;
    return 0;
}

static inline void SDL_UnlockYUVOverlay(SDL_Overlay *overlay)
{
    (void)overlay;
}

static inline int SDL_DisplayYUVOverlay(SDL_Overlay *overlay, SDL_Rect *dstrect)
{
    (void)overlay; (void)dstrect;
    return 0;
}

static inline void SDL_FreeYUVOverlay(SDL_Overlay *overlay)
{
    if (!overlay) return;
    if (overlay->pixels) {
        if (overlay->pixels[0]) SDL_free(overlay->pixels[0]);
        SDL_free(overlay->pixels);
    }
    if (overlay->pitches) SDL_free(overlay->pitches);
    if (overlay->_texture) SDL_DestroyTexture((SDL_Texture *)overlay->_texture);
    SDL_free(overlay);
}

#endif /* SDL12_COMPAT_H */
