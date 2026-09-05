/*! \unit Graphical extensions for SDL
 *
 *  EDL is a ViLE-specific extension of SDL. EDL_GFX handles graphical tasks
 *  such as blitting and blending, rendering text and masking graphics. It
 *  usually overrides the default SDL methods and implements standard methods
 *  for handling graphics.
 */

#ifndef _EDL_GFX_H_
#define _EDL_GFX_H_

#include "edl_common.h"
#include <SDL2_gfxPrimitives.h>
#include <SDL2_rotozoom.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include "edl_texture.h"
#ifdef VILE_FEATURE_SCALER
#include "../external/hq2x/hq2x.h"
#include "../external/hq3x/hq3x.h"
#include "../external/hq4x/hq4x.h"
extern void EDL_ScaleHQ2X(SDL_Surface *src,SDL_Surface *dst);
extern void EDL_ScaleHQ3X(SDL_Surface *src,SDL_Surface *dst);
extern void EDL_ScaleHQ4X(SDL_Surface *src,SDL_Surface *dst);
#endif
extern SDL_Renderer* EDLRenderer;
extern TTF_Font *EDLfont;
extern EDLTexture *EDLrendererTex;

// ----- Graphical API -----

extern EDLTexture *EDL_CreateTexture(int Width,int Height);
extern void EDL_SetBox(SDL_Surface *src, int x, int y, int w, int h, Uint8 R, Uint8 G, Uint8 B, Uint8 A);

extern void EDL_BlendSurface(
		SDL_Surface *src, SDL_Rect *srcrect,
		SDL_Surface *dst, SDL_Rect *dstrect);

extern void EDL_BlitSurface(
		SDL_Surface *src, SDL_Rect *srcrect,
		SDL_Surface *dst, SDL_Rect *dstrect);

extern void EDL_BlitText(
		uString Text,Uint32 Color,
		SDL_Surface *dst,SDL_Rect *dstrect);

extern void EDL_BlendText(
		uString Text,Uint32 Color,
		SDL_Surface *dst,SDL_Rect *dstrect);

extern void EDL_ResizeSurface(SDL_Surface *src,SDL_Surface *dst,bool Smooth);

extern void EDL_SetAlpha(SDL_Surface *src, SDL_Rect *srcrect,Uint8 Alpha);

extern SDL_Surface *EDL_MapSurface(SDL_Surface *Source,SDL_Rect *Rect);

extern SDL_Surface *EDL_MaskSurface(SDL_Surface *Bitmap,
		SDL_Surface *Mask,SDL_Rect *Rect);

extern void EDL_setupImageAlpha( SDL_Surface *surface, SDL_Surface *surface_m,Uint8 a);

extern SDL_Surface *EDL_ColorkeySurface(SDL_Surface *Bitmap,
		Uint32 Key,SDL_Rect *Rect);

extern SDL_Surface *EDL_ColorkeySurface(SDL_Surface *Bitmap,SDL_Rect *Rect);

extern void EDL_SaveSurface(SDL_Surface * surface,uString File);

extern SDL_Surface *EDL_CopySurface(SDL_Surface *Source,SDL_Rect *Rect);

extern SDL_Surface *EDL_CreateSurface(int Width,int Height);

extern SDL_Surface *EDL_CreateText(uString Text,
		Uint32 Color,int Width,int Height,bool newF = false);

extern SDL_Surface *EDL_RenderText(TTF_Font *Font,
		const uString &Text,SDL_Color fg);

extern int EDL_SizeUTF8(TTF_Font *font, const uString &Text, int *w, int *h);

extern SDL_Surface *EDL_CreateMultilineText(uString Text,
		Uint32 Color,int Width,int Height);

extern bool EDL_SizeText(uString Text,int Size,int *Width,int *Height);

extern Uint32 EDL_GetPixel(SDL_Surface * surface,Sint16 x,Sint16 y);

extern void EDL_GetPixel(SDL_Surface * surface,
		Sint16 x,Sint16 y,Uint8 *red,Uint8 *green,Uint8 *blue,Uint8 *alpha);


extern void EDL_SetPixel(SDL_Surface * surface,
		Sint16 x,Sint16 y,Uint8 red,Uint8 green,Uint8 blue,Uint8 alpha);

#endif

