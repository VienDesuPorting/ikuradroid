/*
 * ViLE - Visual Library Engine
 * Copyright (c) 2010-2011, ViLE Team (team@vilevn.org)
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "stdtitle.h"
#include "../engine/evn.h"

StdTitle::StdTitle(EngineVN *Engine) : DialogBase(Engine,true) {
	// Set defaults
	w_new_button=0;
	w_load_button=0;
	w_option_button=0;
	w_extra_button=0;
	w_exit_button=0;
	w_resume_button=0;

	// Adjust to the engine
	Resize(Engine->NativeWidth(),Engine->NativeHeight());
}

StdTitle::~StdTitle(){
}

/*! \brief Assign button for starting a new game
 *  \param Object Preconfigured button widget
 */
void StdTitle::SetNewButton(BitmapButton *Object){
	w_new_button=Object;
	AddWidget(Object);
}

/*! \brief Assign button for accessing the load screen
 *  \param Object Preconfigured button widget
 */
void StdTitle::SetLoadButton(BitmapButton *Object){
	w_load_button=Object;
	AddWidget(Object);
}

/*! \brief Assign button for accessing the option screen
 *  \param Object Preconfigured button widget
 */
void StdTitle::SetOptionButton(BitmapButton *Object){
	w_option_button=Object;
	AddWidget(Object);
}

/*! \brief Assign button for accessing the extras screen
 *  \param Object Preconfigured button widget
 */
void StdTitle::SetExtraButton(BitmapButton *Object){
	w_extra_button=Object;
	AddWidget(Object);
}

/*! \brief Assign button for accessing the extras screen
 *  \param Object Preconfigured button widget
 */
void StdTitle::SetExtraCGButton(BitmapButton *Object){
	w_extra_cg_button=Object;
	AddWidget(Object);
}

/*! \brief Assign button for accessing the extras screen
 *  \param Object Preconfigured button widget
 */
void StdTitle::SetExtraBGMButton(BitmapButton *Object){
	w_extra_bgm_button=Object;
	AddWidget(Object);
}

/*! \brief Assign button for accessing the extras screen
 *  \param Object Preconfigured button widget
 */
void StdTitle::SetExtraSceneButton(BitmapButton *Object){
	w_extra_scene_button=Object;
	AddWidget(Object);
}

/*! \brief Assign button for exiting the application
 *  \param Object Preconfigured button widget
 */
void StdTitle::SetExitButton(BitmapButton *Object){
	w_exit_button=Object;
	AddWidget(Object);
}

/*! \brief Assign button for hiding the title screen
 *  \param Object Preconfigured button widget
 */
void StdTitle::SetResumeButton(BitmapButton *Object){
	w_resume_button=Object;
	AddWidget(Object);
}

/*! \brief Overrides mouse click events
 *  \param Widget The selected widget object
 *  \return True if event was handled
 */
bool StdTitle::InputOk(Widget *Object){
	bool retval=true;
	if(Object==w_new_button){
		// Generate savebutton event and hide dialog
		engine->EventNew();
	}
	else if(Object==w_load_button){
		// Generate loadbutton event
		engine->EventGameDialog(VD_LOAD);
	}
	else if(Object==w_option_button){
		// Generate optionbutton event
		engine->EventGameDialog(VD_OPTIONS);
	}
	else if(Object==w_extra_button){
		// Generate optionbutton event
		engine->EventGameDialog(VD_EXTRAS);
	}
	else if(Object==w_extra_cg_button){
		// Generate optionbutton event
		engine->EventGameDialog(VD_EXTRAS_CG);
	}
	else if(Object==w_extra_bgm_button){
		// Generate optionbutton event
		engine->EventGameDialog(VD_EXTRAS_BGM);
	}
	else if(Object==w_extra_scene_button){
		// Generate optionbutton event
		engine->EventGameDialog(VD_EXTRAS_SCENE);
	}
	else if(Object==w_exit_button){
		// Generate optionbutton event
		engine->EventGameDialog(VD_SHUTDOWN);
	}
	else if(Object==w_resume_button){
		// Generate optionbutton event
		SetVisible(false);
	}
	else{
		retval=false;
	}
	return retval;
}

