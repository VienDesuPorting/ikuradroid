/*! \class Sequencer
 *  \brief Encapsulates series of animations as sequences
 *
 *  Sequencer is a pure convenience class and does not introduce any features 
 *  on its own. It is a helper-class for EngineVN that encapsulates series of
 *  animations into sequences that can be loaded by executing a function.
 */
#ifndef _SEQUENCER_H_
#define _SEQUENCER_H_

#define SEQUENCER_MSPERSCREEN	10000

#include "../dialogs/dlgbase.h"

class EngineVN;

class Sequencer {
	private:
		EngineVN *engine;
	protected:
		// Macros for creating logo source
		void CreateLogoText(SDL_Surface *Surface,SDL_Color FG);
		SDL_Surface *CreateLogoAlpha(int Width,int Height,SDL_Color FG);
		SDL_Surface *CreateLogoOpaque(int Width,int Height,
				SDL_Color FG,SDL_Color BG);
	public:
		// Constructor registers host engine
		Sequencer(EngineVN *Engine);

		// Available sequences
		void RollVertical(SDL_Surface *Surface);
		void RollPaddedVertical(SDL_Surface *Surface);
		void LogoRandom();
		void LogoRandom(Uint8 R,Uint8 G,Uint8 B);
		void LogoZoom(Uint8 R,Uint8 G,Uint8 B);
		void LogoFlicker(Uint8 R,Uint8 G,Uint8 B);
};

#endif

