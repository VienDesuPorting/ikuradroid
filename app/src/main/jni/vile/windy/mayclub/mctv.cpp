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

#include "mctv.h"
#include "mayclub.h"

MCTextview::MCTextview(Mayclub *Engine) : Textview(Engine) {
	// Map up buttons surface
	SDL_Rect cln_button;
	SDL_Rect src_btn_save_1;
	SDL_Rect src_btn_save_2;
	SDL_Rect src_btn_load_1;
	SDL_Rect src_btn_load_2;
	SDL_Rect src_btn_back_1;
	SDL_Rect src_btn_back_2;
	SDL_Rect src_btn_back_3;
	SDL_Rect src_btn_skip_1;
	SDL_Rect src_btn_skip_2;
	SDL_Rect src_btn_skip_3;
		
	// Map up destination surface
	SDL_Rect dst_btn_save;
	SDL_Rect dst_btn_load;
	SDL_Rect dst_btn_skip;
	SDL_Rect dst_btn_back;

	// Map buttons
	EDL_SETRECT(cln_button,0,0,44,24);
	EDL_SETRECT(src_btn_save_1,0,2,cln_button.w,cln_button.h);
	EDL_SETRECT(src_btn_save_2,cln_button.w,2,cln_button.w,cln_button.h);
	EDL_SETRECT(src_btn_load_1,0,30,cln_button.w,cln_button.h);
	EDL_SETRECT(src_btn_load_2,cln_button.w,30,cln_button.w,cln_button.h);
	EDL_SETRECT(src_btn_skip_1,0,60,cln_button.w,cln_button.h);
	EDL_SETRECT(src_btn_skip_2,cln_button.w,60,cln_button.w,cln_button.h);
	EDL_SETRECT(src_btn_skip_3,cln_button.w*2,60,cln_button.w,cln_button.h);
	EDL_SETRECT(src_btn_back_1,0,86,cln_button.w,cln_button.h);
	EDL_SETRECT(src_btn_back_2,cln_button.w,86,cln_button.w,cln_button.h);
	EDL_SETRECT(src_btn_back_3,cln_button.w*2,86,cln_button.w,cln_button.h);

	// Map screen
	EDL_SETRECT(dst_btn_save,20,392,cln_button.w,cln_button.h);
	EDL_SETRECT(dst_btn_load,20,422,cln_button.w,cln_button.h);
	EDL_SETRECT(dst_btn_back,576,392,cln_button.w,cln_button.h);
	EDL_SETRECT(dst_btn_skip,576,422,cln_button.w,cln_button.h);

	// Load textview graphics
	sbuttons=0;
	swindow=0;
	SDL_Surface *pmain=Engine->LoadImage("window.bet");
	SDL_Surface *pmask=Engine->LoadImage("window_.bet");
	if(pmain && pmask){
		SDL_Surface *bmain=Engine->LoadImage("button.bet");
		SDL_Surface *bmask=Engine->LoadImage("button_.bet");
		if(bmain && bmask){
			// Mask images
			sbuttons=EDL_MaskSurface(bmain,bmask,0);
			swindow=EDL_MaskSurface(pmain,pmask,0);
			if(sbuttons && swindow){
				// Prepare widget object
				Move(0,0);
				Resize(swindow->w,swindow->h);

				// Configure apperance
				Set(swindow);
				SetTextPosition(80,384,480,84);
				SetFontColor(0xFF,0xFF,0xFF);

				// Configure widgets
				w_save=new BitmapButton();
				w_save->Move(dst_btn_save);
				w_save->SetState(WS_NORMAL,sbuttons,&src_btn_save_1);
				w_save->SetState(WS_HOVER,sbuttons,&src_btn_save_2);
				w_save->SetState(WS_SELECT,sbuttons,&src_btn_save_1);
				SetSaveButton(w_save);
				w_load=new BitmapButton();
				w_load->Move(dst_btn_load);
				w_load->SetState(WS_NORMAL,sbuttons,&src_btn_load_1);
				w_load->SetState(WS_HOVER,sbuttons,&src_btn_load_2);
				w_load->SetState(WS_SELECT,sbuttons,&src_btn_load_1);
				SetLoadButton(w_load);
				w_back=new BitmapButton();
				w_back->Move(dst_btn_back);
				w_back->SetState(WS_NORMAL,sbuttons,&src_btn_back_1);
				w_back->SetState(WS_HOVER,sbuttons,&src_btn_back_2);
				w_back->SetState(WS_SELECT,sbuttons,&src_btn_back_1);
				SetLoadButton(w_back);
				w_skip=new BitmapButton();
				w_skip->Move(dst_btn_skip);
				w_skip->SetState(WS_NORMAL,sbuttons,&src_btn_skip_1);
				w_skip->SetState(WS_HOVER,sbuttons,&src_btn_skip_2);
				w_skip->SetState(WS_SELECT,sbuttons,&src_btn_skip_1);
				SetLoadButton(w_skip);
			}

			// Clean up graphics
			SDL_FreeSurface(bmain);
			SDL_FreeSurface(bmask);
		}
		else{
			LogError("Cannot find textview overlay: button.bet");
		}

		// Clean up window graphics
		SDL_FreeSurface(pmain);
		SDL_FreeSurface(pmask);
	}
	else{
		LogError("Cannot find textview image: window.bet");
	}
}

MCTextview::~MCTextview(){
	if(swindow){
		SDL_FreeSurface(swindow);
	}
	if(sbuttons){
		SDL_FreeSurface(sbuttons);
	}
}

bool MCTextview::InputOk(Widget *Object){
	bool retval=false;
	if(Object==w_load){
		engine->EventGameDialog(VD_LOAD);
		retval=true;
	}
	else if(Object==w_save){
		engine->EventGameDialog(VD_SAVE);
		retval=true;
	}
	else if(Object==w_back){
		engine->EventGameDialog(VD_LOG);
	}
	else if(Object==w_skip){
		((Mayclub*)engine)->SetSkipmode(true);
		retval=true;
	}
	return retval;
}

bool MCTextview::GetTimeofday(Uint16 TOD,SDL_Rect *R){
	if(TOD==0x0000)				EDL_SETRECT((*R),0,112,72,72);
	else if(TOD==0x0001)		EDL_SETRECT((*R),72,112,72,72);
	else if(TOD==0x0002)		EDL_SETRECT((*R),0,184,72,72);
	else if(TOD==0x0003)		EDL_SETRECT((*R),72,184,72,72);
	else						return false;
	return true;
}

bool MCTextview::GetDayofweek(Uint16 DOW,SDL_Rect *R){
	if(DOW==0x0001)				EDL_SETRECT((*R),28*0,256,28,12);
	else if(DOW==0x0002)		EDL_SETRECT((*R),28*1,256,28,12);
	else if(DOW==0x0003)		EDL_SETRECT((*R),28*2,256,28,12);
	else if(DOW==0x0004)		EDL_SETRECT((*R),28*3,256,28,12);
	else if(DOW==0x0005)		EDL_SETRECT((*R),28*0,268,28,12);
	else if(DOW==0x0006)		EDL_SETRECT((*R),28*1,268,28,12);
	else if(DOW==0x0000)		EDL_SETRECT((*R),28*2,268,28,12);
	else						return false;
	return true;
}

bool MCTextview::GetDigit(Uint16 Number,SDL_Rect *R){
	if(Number==0x0000)			EDL_SETRECT((*R),20*0,280,20,24);
	else if(Number==0x0001)		EDL_SETRECT((*R),20*1,280,20,24);
	else if(Number==0x0002)		EDL_SETRECT((*R),20*2,280,20,24);
	else if(Number==0x0003)		EDL_SETRECT((*R),20*3,280,20,24);
	else if(Number==0x0004)		EDL_SETRECT((*R),20*4,280,20,24);
	else if(Number==0x0005)		EDL_SETRECT((*R),20*0,304,20,24);
	else if(Number==0x0006)		EDL_SETRECT((*R),20*1,304,20,24);
	else if(Number==0x0007)		EDL_SETRECT((*R),20*2,304,20,24);
	else if(Number==0x0008)		EDL_SETRECT((*R),20*3,304,20,24);
	else if(Number==0x0009)		EDL_SETRECT((*R),20*4,304,20,24);
	else						return false;
	return true;
}

bool MCTextview::SetTime(Uint16 Month,Uint16 Day,Uint16 TOD,Uint16 DOW){
	bool retval=(TOD<4);
	SDL_Rect dst_month={24,9,20,24};
	SDL_Rect dst_day_1={60,9,20,24};
	SDL_Rect dst_day_2={78,9,20,24};
	SDL_Rect dst_timeofday={4,55,72,72};
	SDL_Rect dst_dayofweek={64,38,28,12};
	if(retval){
		// Clear old graphics
		Blit(swindow,&dst_month,&dst_month);
		Blit(swindow,&dst_day_1,&dst_day_1);
		Blit(swindow,&dst_day_2,&dst_day_2);
		Blit(swindow,&dst_dayofweek,&dst_dayofweek);

		// Paint new graphics
		SDL_Rect src;
		if(GetDigit(Month,&src)){
			Blend(sbuttons,&src,&dst_month);
		}
		if(Day){
			if(GetDigit(Day/10,&src)){
				Blend(sbuttons,&src,&dst_day_1);
			}
			if(GetDigit(Day%10,&src)){
				Blend(sbuttons,&src,&dst_day_2);
			}
		}
		if(GetDayofweek(DOW,&src)){
			Blend(sbuttons,&src,&dst_dayofweek);
		}
		if(GetTimeofday(TOD,&src)){
			Blend(sbuttons,&src,&dst_timeofday);
		}
	}
	return retval;
}

bool MCTextview::SetTickets(Uint16 Tickets){
	bool retval=(Tickets<=99);
	if(retval){
		// Clear old graphics
		SDL_Rect dst_ticket_1={8,158,20,30};
		SDL_Rect dst_ticket_2={25,158,20,30};
		Blit(swindow,&dst_ticket_1,&dst_ticket_1);
		Blit(swindow,&dst_ticket_2,&dst_ticket_2);

		// Paint new graphics
		SDL_Rect src1;
		SDL_Rect src2;
		if(Tickets){
			if(GetDigit(Tickets/10,&src1) && GetDigit(Tickets%10,&src2)){
				Blend(sbuttons,&src1,&dst_ticket_1);
				Blend(sbuttons,&src2,&dst_ticket_2);
			}
		}
		else{
			if(GetDigit(0,&src1) && GetDigit(Tickets,&src2)){
				Blend(sbuttons,&src1,&dst_ticket_1);
				Blend(sbuttons,&src2,&dst_ticket_2);
			}
		}

	}
	return retval;
}


