/*! \class EngineVideo
 *  \brief Implements video and animation features
 *
 *  This class is basically a driver class for the decoder implementations
 *  in the media/ folder (See Video class). We currently only support FFMPEG.
 *
 *  Video is considered a special case of media that requires as little
 *  overhead as possible. To accomplish this it overrides the host events
 *  of the base, effectively blocking all other execution until the video
 *  has ended. Input events (Mouse/keyboard) are used to exit the video state
 *  prematurely, and the paint event are used to paint the frames.
 *
 *  Animation in this context means series of bitmaps that animate into a
 *  video-like sequence. Please check EngineBase for dynamical widgets that
 *  animate on their own accord.
 *
 *  The EngineMixer class is called to handle any audio streams.
 */

#ifndef _EVIDEO_H_
#define _EVIDEO_H_

#include "emixer.h"
#include "../media/video.h"



class EngineVideo : public EngineMixer{
	private:
		// Simple animation
		SDL_Surface **asurface;				//!< Animation frames in memory
		int aframe;							//!< Current animation frame
	protected:
		// Video events
		//virtual void EventGameStartVideo();
		//virtual void EventGameStopVideo();
	public:
		EngineVideo(int Width,int Height);
		~EngineVideo();
        SDL_Renderer * getrenderer();
		// Video API
		void StopAnimation();
		bool PlayingAnimation();
		virtual bool PlayAnimation(SDL_Surface **Animation);
		// Override host events
		void render();
		void EventHostTick();
		void EventHostMouseMove(SDL_Surface *Screen,int X,int Y);
		void EventHostMouseLeftDown(SDL_Surface *Screen,int X,int Y);
		void EventHostMouseRightDown(SDL_Surface *Screen,int X,int Y);
		void EventHostMouseLeftUp(SDL_Surface *Screen,int X,int Y);
		void EventHostMouseRightUp(SDL_Surface *Screen,int X,int Y);
		void EventHostKeyDown(SDL_Keycode Key);
		void EventHostKeyUp(SDL_Keycode Key);
};

#endif


