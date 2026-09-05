/*! \class Renderer
 *  \brief Renders widgets onto a surface
 *
 *  Rendering was originally a part of EngineBase but was diverted to make
 *  it easier to implement and test alternative versions in the live code
 *  base. The base class is a port of the original rendering method.
 */

#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "../widgets/widget.h"
#include "../widgets/animation.h"
#include "../widgets/group.h"

enum GRAPHIC_SCALER {					//!< Enumerates scalers
	GS_DEFAULT=0,						//!< Disable scaling
	GS_FAST,							//!< Resize with quick and dirty scaler
	GS_NEAREST,							//!< Resize using nearest neigbour
	GS_HQ2X,							//!< HQ2X High quality scaler
	GS_HQ3X,							//!< HQ3X High quality scaler
	GS_HQ4X,							//!< HQ4X High quality scaler
	GS_SIZE								//!< Defines size of enum
};

class Renderer {
	protected:
		// Object data
		GRAPHIC_SCALER scaler;
		SDL_Surface *gamesurface;
		SDL_Window* window;
		Group *animations;
		Group *widgets;
		int height;
		int width;

		// Internal rendering interface
		virtual void Scale(SDL_Surface *In,SDL_Surface *Out);
		virtual void UpdateAnimations();
		virtual void UpdateWidgets();
	public:
		// Public rendering interface
		virtual void setWindow(SDL_Window* win);

		Renderer(Group *Widgets,Group *Animations,int Width,int Height);
		virtual ~Renderer();
		virtual void UpdateResolution();
		virtual void Render(int Top=0);
		virtual void Update();

		virtual void SetScaler(GRAPHIC_SCALER Scaler);
};

#endif

