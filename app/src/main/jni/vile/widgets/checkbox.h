/*! \class Checkbox
 *  \brief Widget with state graphics
 */
/*
 *  IkuraDroid modification notice - 2026
 *
 *  This file is modified for IkuraDroid (su.viende.ikuradroid) - the
 *  ViLE-based Android visual novel player - and differs from the
 *  original ViLE 0.4.13 source.
 *
 *  Base: ViLE 0.4.13 by the ViLE Team <team@vilevn.org>. The Android
 *  port continues the original VILE for Android (2015-2016) and is
 *  modified for IkuraDroid by the VienDesu! Porting Team
 *  <github.com/VienDesuPorting/ikuradroid>.
 *
 *  Distributed under the same GNU GPL (version 3) as the original
 *  ViLE; this notice supplements the original copyright.
 */
#ifndef _CHECKBOX_H_
#define _CHECKBOX_H_

#include "vbutton.h"

class Checkbox : public ValueButton {
	private:
		bool leftsided;
		uString caption;
		void autogenerate();
	public:
		Checkbox(SDL_Rect Position,uString Caption,bool Leftsided=true);
		Checkbox(uString Caption,bool Leftsided=true);
		void SetCaption(uString Caption);
		void SetOrientation(bool Leftsided);

		// Overrides
		virtual bool KeyDown(SDL_Keycode Key);
		virtual bool MouseLeftDown(int X,int Y);
};

#endif

