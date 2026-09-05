/*! \class Options
 *  \brief Standard dialog for setting options
 */
#ifndef _OPTIONS_H_
#define _OPTIONS_H_

#include "dlgbase.h"

class Options : public DialogBase {
	protected:
		Checkbox *cb_fullscreen;
		//Checkbox *cb_readonly;
		Checkbox *cb_bgm;
		Checkbox *cb_se;
		Checkbox *cb_voice;
		Checkbox *cb_delay;
		Slider *sl_bgm;
		Slider *sl_se;
		Slider *sl_voice;
		Slider *sl_delay;
		BitmapButton *tb_load;
		BitmapButton *tb_save;
		BitmapButton *tb_title;
		BitmapButton *tb_exit;
		BitmapButton *tb_resume;
	public:
		Options(EngineVN *Engine);
		~Options();

		// Hook into user input
		bool MouseMove(Widget *Object,int X,int Y);
		bool MouseLeftDown(Widget* Object,int X,int Y);
		bool InputOk(Widget *Object);

};

#endif

