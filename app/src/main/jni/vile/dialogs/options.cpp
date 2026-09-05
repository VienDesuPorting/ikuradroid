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

#include "options.h"
#include "../engine/evn.h"

Options::Options(EngineVN *Engine) : DialogBase(Engine,true){
	// Create background graphics
	SDL_Rect rfull={0,0,Engine->NativeWidth(),Engine->NativeHeight()};
	Move(rfull);
	Fill(Cfg::Color::DialogBackground);
	int space=rfull.h/10;
	SDL_Rect rtmp={space,space,rfull.w,rfull.h/10};

	//EDL_BlendText("Configuration",Cfg::Color::DialogHeader,simage,&rtmp);
	//rtmp.y=rfull.h/4;
	//rtmp.h=rtmp.h/2;
	//EDL_BlendText("Graphics",Cfg::Color::DialogTopic,simage,&rtmp);
	////rtmp.x=space+rfull.w/2;
	//EDL_BlendText("Audio",Cfg::Color::DialogTopic,simage,&rtmp);
	//rtmp.x=space;
	//rtmp.y=rfull.h/2;
	//EDL_BlendText("Generic",Cfg::Color::DialogTopic,simage,&rtmp);
	// Fullscreen setting
	//rtmp.x=(rfull.w/2)+space*2;

    rtmp.x=space*2;
	rtmp.y=(rfull.h/10)+space*0;
	rtmp.w=rfull.w/2;
	rtmp.h=space;
	cb_fullscreen=new Checkbox(rtmp,"Fullscreen mode");
	cb_fullscreen->SetValue(Engine->GetFullscreen());

	// BGM control
	rtmp.x=space*2;
	rtmp.y=(rfull.h/8)+space*1;
	rtmp.w=rfull.w/5;
	rtmp.h=space;
	cb_bgm=new Checkbox(rtmp,"Music");
	cb_bgm->SetValue(Engine->GetMusicEnabled());
	cb_bgm->SetOrientation(false);
	rtmp.x+=rtmp.w+4;
	rtmp.w=rfull.w/2;
	sl_bgm=new Slider(rtmp);
	sl_bgm->SetValue(Engine->GetVolumeMusic());

	// SE control
	rtmp.x=space*2;
	rtmp.y=(rfull.h/8)+space*2;
	rtmp.w=rfull.w/5;
	rtmp.h=space;
	cb_se=new Checkbox(rtmp,"SE");
	cb_se->SetOrientation(false);
	cb_se->SetValue(Engine->GetSoundEnabled(VA_SOUNDS));
	rtmp.x+=rtmp.w+4;
	rtmp.w=rfull.w/2;
	sl_se=new Slider(rtmp);
	sl_se->SetValue(Engine->GetVolumeSound(VA_SOUNDS));

	// Voice control
	rtmp.x=space*2;
	rtmp.y=(rfull.h/8)+space*3;
	rtmp.w=rfull.w/5;
	rtmp.h=space;
	cb_voice=new Checkbox(rtmp,"Voices");
	cb_voice->SetOrientation(false);
	cb_voice->SetValue(Engine->GetSoundEnabled(VA_VOICES));
	rtmp.x+=rtmp.w+4;
	rtmp.w=rfull.w/2;
	sl_voice=new Slider(rtmp);
	sl_voice->SetValue(engine->GetVolumeSound(VA_VOICES));

	rtmp.x=space*2;
	rtmp.y=(rfull.h/8)+space*4;
	rtmp.w=rfull.w/5;
	rtmp.h=space;
	cb_delay=new Checkbox(rtmp,"Text delay");
	cb_delay->SetValue(Engine->GetMessageDelayEnabled());
	cb_delay->SetOrientation(false);
	rtmp.x+=rtmp.w+4;
	rtmp.w=rfull.w/2;
	sl_delay=new Slider(rtmp);
	sl_delay->SetValue(Engine->GetMessageDelayInterval());

	// Create buttons
	int buttonw=rfull.w/6;
	int buttonh=Cfg::Font::default_size*1.5;
	int emptyw=(rfull.w-(buttonw*5))/6;
	EDL_SETRECT(rtmp,emptyw,rfull.h-(buttonh+emptyw),buttonw,buttonh);
	tb_load=new BitmapButton(rtmp);
	tb_load->SetCaption("Load");
	rtmp.x+=rtmp.w+emptyw;
	tb_save=new BitmapButton(rtmp);
	tb_save->SetCaption("Save");
	rtmp.x+=rtmp.w+emptyw;
	tb_title=new BitmapButton(rtmp);
	tb_title->SetCaption("Title");
	rtmp.x+=rtmp.w+emptyw;
	tb_exit=new BitmapButton(rtmp);
	tb_exit->SetCaption("Exit");
	rtmp.x+=rtmp.w+emptyw;
	tb_resume=new BitmapButton(rtmp);
	tb_resume->SetCaption("Resume");

	// Delay control


	// Add widgets to the dialog
#ifndef ANDROID
	AddWidget(cb_fullscreen);
	AddWidget(tb_load);
	AddWidget(tb_save);
	AddWidget(tb_title);
	AddWidget(tb_exit);
#endif
	AddWidget(cb_delay);
	AddWidget(sl_delay);
	AddWidget(cb_bgm);
	AddWidget(sl_bgm);
	AddWidget(cb_se);
	AddWidget(sl_se);
	AddWidget(cb_voice);
	AddWidget(sl_voice);

	AddWidget(tb_resume);
}

Options::~Options(){
}

/*! \brief Passes mouse event to widget and reads back updated values
 *  \param Object Widget object that recieved the event
 *  \param X Mouse coordinate
 *  \param Y Mouse coordinate
 *
 *  Passes mouse event to widget (Usually a slider) and reads back the
 *  updated value and passes it to the relevant engine member (Ie. set
 *  bgm volume etc).
 */
bool Options::MouseMove(Widget* Object,int X,int Y){
	bool retval=false;
	if(Object==sl_delay){
		sl_delay->MouseMove(X,Y);
		engine->SetMessageDelayInterval(sl_delay->GetValue());
		retval=true;
	}
	else if(Object==sl_bgm){
		sl_bgm->MouseMove(X,Y);
		engine->SetVolumeMusic(sl_bgm->GetValue());
		retval=true;
	}
	else if(Object==sl_se){
		sl_se->MouseMove(X,Y);
		engine->SetVolumeSound(VA_SOUNDS,sl_se->GetValue());
		retval=true;
	}
	else if(Object==sl_voice){
		sl_voice->MouseMove(X,Y);
		engine->SetVolumeSound(VA_VOICES,sl_voice->GetValue());
		retval=true;
	}
	return retval;
}

bool Options::InputOk(Widget *Object){
	bool retval=false;
	if(Object==cb_delay){
		engine->SetMessageDelayEnabled(!cb_delay->GetValue());
	}
	else if(Object==cb_bgm){
		engine->SetMusicEnabled(!cb_bgm->GetValue());
	}
	else if(Object==cb_se){
		engine->SetSoundEnabled(VA_SOUNDS,!cb_se->GetValue());
	}
	else if(Object==cb_voice){
		engine->SetSoundEnabled(VA_VOICES,!cb_voice->GetValue());
	}
	else if(Object==cb_fullscreen){
		engine->SetFullscreen(!cb_fullscreen->GetValue());
	}
	else if(Object==tb_load){
		engine->DestroyWidget(this);
		engine->EventGameDialog(VD_LOAD);
		retval=true;
	}
	else if(Object==tb_save){
		engine->DestroyWidget(this);
		engine->EventGameDialog(VD_SAVE);
		retval=true;
	}
	else if(Object==tb_title){
		engine->DestroyWidget(this);
		engine->EventGameDialog(VD_TITLE);
		retval=true;
	}
	else if(Object==tb_resume){
		engine->DestroyWidget(this);
		retval=true;
	}
	else if(Object==tb_exit){
		engine->EventGameDialog(VD_SHUTDOWN);
		retval=true;
	}
	return retval;
}

/*! \brief Passes mouse event to widget and reads back updated values
 *  \param Object Widget object that recieved the event
 *  \param X Mouse coordinate
 *  \param Y Mouse coordinate
 *
 *  Passes mouse event to widget (Usually a slider) and reads back the
 *  updated value and passes it to the relevant engine member (Ie. set
 *  bgm volume etc).
 */
bool Options::MouseLeftDown(Widget* Object,int X,int Y){
	bool retval=false;
	if(InputOk(Object)){
		// Input was accepted
		retval=true;
	}
	else if(Object==sl_delay){
		sl_delay->MouseLeftDown(X,Y);
		engine->SetMessageDelayInterval(sl_delay->GetValue());
		retval=true;
	}
	else if(Object==sl_bgm){
		sl_bgm->MouseLeftDown(X,Y);
		engine->SetVolumeMusic(sl_bgm->GetValue());
		retval=true;
	}
	else if(Object==sl_se){
		sl_se->MouseLeftDown(X,Y);
		engine->SetVolumeSound(VA_SOUNDS,sl_se->GetValue());
		retval=true;
	}
	else if(Object==sl_voice){
		sl_voice->MouseLeftDown(X,Y);
		engine->SetVolumeSound(VA_VOICES,sl_voice->GetValue());
		retval=true;
	}
	return retval;
}

