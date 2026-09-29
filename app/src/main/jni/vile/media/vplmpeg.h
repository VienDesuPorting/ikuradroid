/*! \file vplmpeg.h
 *  \brief Blocking MPEG-1 playback on top of pl_mpeg.
 *
 *  The player takes a ViLE resource stream (a loose file or a window
 *  into an archive), decodes it with the vendored pl_mpeg decoder and
 *  presents every frame through the engine renderer. It blocks the
 *  calling thread until the stream ends, the user skips it or the
 *  application goes to the background.
 */

#ifndef _VPLMPEG_H_
#define _VPLMPEG_H_

#include <SDL.h>
#include "../res/rwops.h"

/*! \brief Plays an MPEG-1 stream and blocks until it is over
 *  \param Resource Stream to decode, ownership stays with the caller
 *  \param Dest Destination rectangle on the game screen, NULL = fullscreen
 *  \param ScreenW Game screen width (logical viewport)
 *  \param ScreenH Game screen height (logical viewport)
 *  \return True if a decoder was created and playback ran
 */
bool VideoPLMPEG_Play(RWops *Resource,const SDL_Rect *Dest,int ScreenW,int ScreenH);

#endif

