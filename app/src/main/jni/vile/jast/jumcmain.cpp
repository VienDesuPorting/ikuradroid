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

#include "jumcmain.h"
#include "jumc.h"

JUMCMain::JUMCMain(JUMC *Engine) : DialogBase(Engine,false){
	// Load graphics
	SDL_Surface *s_main=Engine->LoadImage("mainscreen");
	SDL_Surface *s_sos_dim=Engine->LoadImage("sosdim");
	SDL_Surface *s_sos_new=Engine->LoadImage("sosnew");
	SDL_Surface *s_sos_load=Engine->LoadImage("sosload");
	SDL_Surface *s_sos_cg=Engine->LoadImage("soscg");
	SDL_Surface *s_sis_dim=Engine->LoadImage("sisdim");
	SDL_Surface *s_sis_new=Engine->LoadImage("sisnew");
	SDL_Surface *s_sis_load=Engine->LoadImage("sisload");
	SDL_Surface *s_sis_cg=Engine->LoadImage("siscg");
	SDL_Surface *s_run_dim=Engine->LoadImage("rundim");
	SDL_Surface *s_run_new=Engine->LoadImage("runnew");
	SDL_Surface *s_run_load=Engine->LoadImage("runload");
	SDL_Surface *s_run_cg=Engine->LoadImage("runcg");
	SDL_Surface *s_gen_dim=Engine->LoadImage("gendim");
	SDL_Surface *s_gen_cg=Engine->LoadImage("gencg");
	SDL_Surface *s_gen_music=Engine->LoadImage("genmusic");
	SDL_Surface *s_gen_setup=Engine->LoadImage("gensetup");
	SDL_Surface *s_gen_exit=Engine->LoadImage("genexit");

	// Configure base dialog
	Move(0,40);
	Resize(640,400);
	Blit(s_main);

	// Runaway city
	SDL_Rect r_run_new={10,90,305,32};
	SDL_Rect r_run_load={10,119,305,32};
	SDL_Rect r_run_cg={10,152,305,32};
	d_runaway=new Selection(Engine);
	d_runaway->Move(10,50);
	d_runaway->Resize(305,185);
	//d_runaway->Blit(s_run_dim);
	d_runaway->SetSurface(s_run_new,0,r_run_new,JI_RUNAWAY_NEW);
	d_runaway->SetSurface(s_run_load,0,r_run_load,JI_RUNAWAY_LOAD);
	d_runaway->SetSurface(s_run_cg,0,r_run_cg,JI_RUNAWAY_CG);
	AddWidget(d_runaway);

	// Three sisters story
	SDL_Rect r_sis_new={325,90,305,32};
	SDL_Rect r_sis_load={325,119,305,32};
	SDL_Rect r_sis_cg={325,152,305,32};
	d_sisters=new Selection(Engine);
	d_sisters->Move(325,50);
	d_sisters->Resize(305,185);
	//d_sisters->Blit(s_sis_dim);
	d_sisters->SetSurface(s_sis_new,0,r_sis_new,JI_SISTERS_NEW);
	d_sisters->SetSurface(s_sis_load,0,r_sis_load,JI_SISTERS_LOAD);
	d_sisters->SetSurface(s_sis_cg,0,r_sis_cg,JI_SISTERS_CG);
	AddWidget(d_sisters);

	// Season of the sakura
	SDL_Rect r_sos_new={10,295,305,32};
	SDL_Rect r_sos_load={10,324,305,32};
	SDL_Rect r_sos_cg={10,357,305,32};
	d_sakura=new Selection(Engine);
	d_sakura->Move(10,245);
	d_sakura->Resize(305,185);
	//d_sakura->Blit(s_sos_dim);
	d_sakura->SetSurface(s_sos_new,0,r_sos_new,JI_SAKURA_NEW);
	d_sakura->SetSurface(s_sos_load,0,r_sos_load,JI_SAKURA_LOAD);
	d_sakura->SetSurface(s_sos_cg,0,r_sos_cg,JI_SAKURA_CG);
	AddWidget(d_sakura);

	// System menu
	SDL_Rect r_gen_cg={326,254,305,48};
	SDL_Rect r_gen_music={326,301,305,48};
	SDL_Rect r_gen_setup={326,345,305,48};
	SDL_Rect r_gen_exit={326,392,305,48};
	d_system=new Selection(Engine);
	d_system->Move(324,245);
	d_system->Resize(305,185);
	//d_system->Blit(s_gen_dim);
	d_system->SetSurface(s_gen_cg,0,r_gen_cg,JI_SYSTEM_CG);
	d_system->SetSurface(s_gen_music,0,r_gen_music,JI_SYSTEM_MUSIC);
	d_system->SetSurface(s_gen_setup,0,r_gen_setup,JI_SYSTEM_SETUP);
	d_system->SetSurface(s_gen_exit,0,r_gen_exit,JI_SYSTEM_EXIT);
	AddWidget(d_system);

	// Delete temporary graphics
	SDL_FreeSurface(s_main);
	SDL_FreeSurface(s_sos_dim);
	SDL_FreeSurface(s_sos_new);
	SDL_FreeSurface(s_sos_load);
	SDL_FreeSurface(s_sos_cg);
	SDL_FreeSurface(s_sis_dim);
	SDL_FreeSurface(s_sis_new);
	SDL_FreeSurface(s_sis_load);
	SDL_FreeSurface(s_sis_cg);
	SDL_FreeSurface(s_run_dim);
	SDL_FreeSurface(s_run_new);
	SDL_FreeSurface(s_run_load);
	SDL_FreeSurface(s_run_cg);
	SDL_FreeSurface(s_gen_dim);
	SDL_FreeSurface(s_gen_cg);
	SDL_FreeSurface(s_gen_music);
	SDL_FreeSurface(s_gen_setup);
	SDL_FreeSurface(s_gen_exit);
}

bool JUMCMain::InputOk(Widget *Object){
	JUMC *jengine=(JUMC*)engine;
	bool retval=false;
	bool t1=(Object==d_runaway);
	bool t2=(Object==d_sisters);
	bool t3=(Object==d_sakura);
	bool t4=(Object==d_system);
	if(t1 || t2 || t3 || t4){
		int result=((Selection*)Object)->GetFocusItem();
		if(result==JI_RUNAWAY_NEW){
			jengine->StartGame(JT_RUN);
			SetVisible(false);
		}
		if(result==JI_SAKURA_NEW){
			jengine->StartGame(JT_SOS);
			SetVisible(false);
		}
		if(result==JI_SISTERS_NEW){
			jengine->StartGame(JT_3SIS);
			SetVisible(false);
		}
		if(result==JI_SYSTEM_EXIT){
			// Close application
			jengine->EventGameShutdown();
		}
		retval=true;
	}
	return retval;
}

bool JUMCMain::FocusEnter(Widget *Object){
	SDL_Surface *tmp=0;
	if(Object==d_runaway)	tmp=engine->LoadImage("rundim");
	if(Object==d_sisters)	tmp=engine->LoadImage("sisdim");
	if(Object==d_sakura)	tmp=engine->LoadImage("sosdim");
	if(Object==d_system)	tmp=engine->LoadImage("gendim");
	if(tmp){
		Object->Blit(tmp);
		SDL_FreeSurface(tmp);
	}
	return false;
}

bool JUMCMain::FocusLeave(Widget *Object){
	SDL_Surface *tmp=0;
	if(Object==d_runaway)	tmp=engine->LoadImage("runnorm");
	if(Object==d_sisters)	tmp=engine->LoadImage("sisnorm");
	if(Object==d_sakura)	tmp=engine->LoadImage("sosnorm");
	if(Object==d_system)	tmp=engine->LoadImage("gennorm");
	if(tmp){
		Object->Blit(tmp);
		SDL_FreeSurface(tmp);
	}
	return false;
}

