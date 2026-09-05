/*! \class StdTitle
 *  \brief Widget for managing a textview with buttons
 *
 *  This should only be used as a basis for complex textview object which
 *  has embedded buttons etc. You should just use a Printer object if all
 *  you need is the animated text.
 *
 *  This class must be revised from bottom up.
 */
#ifndef _STDTITLE_H_
#define _STDTITLE_H_

#include "dlgbase.h"

class StdTitle : public DialogBase {
	private:
		// Standard widgets
		BitmapButton *w_new_button;
		BitmapButton *w_load_button;
		BitmapButton *w_option_button;
		BitmapButton *w_extra_button;
		BitmapButton *w_extra_cg_button;
		BitmapButton *w_extra_bgm_button;
		BitmapButton *w_extra_scene_button;
		BitmapButton *w_exit_button;
		BitmapButton *w_resume_button;

		// Hook into user input
		virtual bool InputOk(Widget *Object);
	public:
		StdTitle(EngineVN *Engine);
		~StdTitle();

		// Register standard buttons
		void SetNewButton(BitmapButton *Object);
		void SetLoadButton(BitmapButton *Object);
		void SetOptionButton(BitmapButton *Object);
		void SetExtraButton(BitmapButton *Object);
		void SetExtraCGButton(BitmapButton *Object);
		void SetExtraBGMButton(BitmapButton *Object);
		void SetExtraSceneButton(BitmapButton *Object);
		void SetExitButton(BitmapButton *Object);
		void SetResumeButton(BitmapButton *Object);
};

#endif

