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

#include "stdsave.h"
#include "../engine/evn.h"

StdSave::StdSave(EngineVN *Engine) : DialogBase(Engine,true){
	// Create background graphics
	SDL_Rect rfull={0,0,Engine->NativeWidth(),Engine->NativeHeight()};
	Move(rfull);
	Fill(0x00,0x00,0x00,200);

	// Default title
	int space=rfull.h/20;
	w_title=new Printer();
	w_title->Move(space,space);
	w_title->SetFontSize(space*2);
	w_title->SetFontColor(100,0,0);
	w_title->Autosize("Save");
	w_title->Print("Save",0);
	AddWidget(w_title);

	// the exit button
	w_exit=new TextButton((rfull.w/10)*9,rfull.h-(space*2),"Exit");
	AddWidget(w_exit);

	// Create saveslab widgets
	int y1=w_title->GetY()+w_title->GetHeight()+space;
	int y2=w_exit->GetY()-space;
	int yh=(y2-y1)/4;
	int yw=(rfull.w/2)-(space*2);
	for(int i=0;i<4;i++){
		w_slabs[i]=new SaveSlab(space,y1+(yh*i)+(i*2),yw,yh);
		AddWidget(w_slabs[i]);
		w_slabs[i+4]=new SaveSlab((rfull.w/2)+space,y1+(yh*i)+(i*2),yw,yh);
		AddWidget(w_slabs[i+4]);
	}

	// Create page buttons
	int x1=w_title->GetX()+w_title->GetWidth()+space;
	int x2=rfull.w-space;
	int xd=(x2-x1)/5;
	y1=w_title->GetY()+(w_title->GetHeight()/3);
	w_pages[0]=new TextButton(x1+(xd*0),y1,"Page 1");
	w_pages[1]=new TextButton(x1+(xd*1),y1,"Page 2");
	w_pages[2]=new TextButton(x1+(xd*2),y1,"Page 3");
	w_pages[3]=new TextButton(x1+(xd*3),y1,"Page 4");
	w_pages[4]=new TextButton(x1+(xd*4),y1,"Page 5");
	AddWidget(w_pages[0]);
	AddWidget(w_pages[1]);
	AddWidget(w_pages[2]);
	AddWidget(w_pages[3]);
	AddWidget(w_pages[4]);

	// Set defaults
	page=-1;
	SetPage(0);

    for(int i=0;i<5;i++)
    {
        for(int j=0;j<8;j++)
        {
            Savegame save(engine->NativeID(),(i*8)+j);
            if(save.Read())
                SetPage(i);
        }
    }
}

StdSave::~StdSave(){
	DestroyWidgets();
}

/*! \brief Sets title string to be displayed
 *  \param Caption Text to set
 */
void StdSave::SetCaption(uString Caption){
	w_title->Clear();
	w_title->Print(Caption.c_str(),0);
	Refresh();
}

/*! \brief Event to signal clicking of a saveslab
 *  \param Index Integer for naming the game
 */
void StdSave::Save(int Index){
	if(engine->EventSave(Index)){
		engine->DestroyWidget(this);
		engine->SetTransition();
	}
}

/*! \brief Displays a given page of the dialog
 *  \param Page Page to display (0-4 for page 1-5)
 */
void StdSave::SetPage(int Page){
	if(Page!=page){
		// Register new page
		Refresh();
		page=Page;
		// Change paginators
		Uint32 fore=0xFFFFFFFF;
		Uint32 back=0x00FF00FF;
		for(int i=0;i<5;i++){
			w_pages[i]->SetColorForeground(WS_NORMAL,back);
			w_pages[i]->FocusEnter();
			w_pages[i]->FocusLeave();
		}
		w_pages[page]->SetColorForeground(WS_NORMAL,fore);
		w_pages[page]->FocusEnter();

		// Change savegame slabs
		for(int i=0;i<8;i++){
			// Flush slab graphic and create background
			w_slabs[i]->Flush();
			SDL_Surface *savebg=EDL_CreateSurface(
					w_slabs[i]->GetWidth(),
					w_slabs[i]->GetHeight());
			EDL_SetBox(savebg,0,0,savebg->w,savebg->h,0xFF,0xFF,0xFF,0x9F);

			w_slabs[i]->SetBackground(savebg,0);
			SDL_FreeSurface(savebg);
            w_slabs[i]->SetBackgroundAlpha(0x7F);
			// Load data from savefile
			Savegame save(engine->NativeID(),(page*8)+i);
			if(save.Read()){
				int space=5;
				int h=w_slabs[i]->GetHeight()-(space*2);
				double r=engine->NativeWidth()/(double)engine->NativeHeight();
				SDL_Surface *screen=0;
				if(save.LoadSurface("screen-thumb",&screen,h*r,h)){
					// Set thumbview
					SDL_Rect dst={space,space,screen->w,screen->h};
					w_slabs[i]->SetThumb(screen,0,&dst);
					SDL_FreeSurface(screen);

					// Set highlight
					SDL_Surface *hl=EDL_CreateSurface(
							w_slabs[i]->GetWidth(),
							w_slabs[i]->GetHeight());
					EDL_SetBox(hl,0,0,hl->w,hl->h,0x00,0x00,0x00,0x00);

					w_slabs[i]->Fill(0,0,0,0.5);
					w_slabs[i]->SetOverlay(hl,&dst);
					SDL_FreeSurface(hl);
					// Set saveinfo
					EDL_SETRECT(dst,(h*r)+(space*2),space,
							w_slabs[i]->GetWidth()-(h*r)-(space*3),
							w_slabs[i]->GetHeight()-(space*2));
					uString tmpstr="Unknown";
					save.LoadString("savemsg",&tmpstr);
					uString msgstr=tmpstr+"\n";
					save.LoadString("savedate",&tmpstr);
					msgstr+=tmpstr;
					w_slabs[i]->SetText(msgstr,&dst);
				}
			}
		}
	}
}

/*! \brief Highlights the focused saveslab by adjusting the background alpha
 */
bool StdSave::FocusEnter(Widget *Object){
	bool retval=Object==w_slabs[0] || Object==w_slabs[1] ||
			Object==w_slabs[2] || Object==w_slabs[3] ||
			Object==w_slabs[4] || Object==w_slabs[5] ||
			Object==w_slabs[6] || Object==w_slabs[7];
	if(retval){
		((SaveSlab*)Object)->SetBackgroundAlpha(0xFF);
		Refresh();
	}
	return retval;
}

/*! \brief Removes highlight from the previously focused saveslab
 */
bool StdSave::FocusLeave(Widget *Object){
	bool retval=Object==w_slabs[0] || Object==w_slabs[1] ||
			Object==w_slabs[2] || Object==w_slabs[3] ||
			Object==w_slabs[4] || Object==w_slabs[5] ||
			Object==w_slabs[6] || Object==w_slabs[7];
	if(retval){
		((SaveSlab*)Object)->SetBackgroundAlpha(0x7F);
		Refresh();
	}
	return retval;
}

bool StdSave::InputOk(Widget *Object){
	LOGCAT("vile input: StdSave InputOk object=%p exit=%p",(void*)Object,(void*)w_exit);
	bool retval=false;
	if(Object==w_exit){
		// Close dialog
		engine->DestroyWidget(this);
		engine->SetTransition();
		retval=true;
	}
	else if(Object==w_pages[0] ||
			Object==w_pages[1] ||
			Object==w_pages[2] ||
			Object==w_pages[3] ||
			Object==w_pages[4]){
		// Change page
		if(Object==w_pages[0])	SetPage(0);
		if(Object==w_pages[1])	SetPage(1);
		if(Object==w_pages[2])	SetPage(2);
		if(Object==w_pages[3])	SetPage(3);
		if(Object==w_pages[4])	SetPage(4);
		retval=true;
	}
	else if(Object==w_slabs[0] ||
			Object==w_slabs[1] ||
			Object==w_slabs[2] ||
			Object==w_slabs[3] ||
			Object==w_slabs[4] ||
			Object==w_slabs[5] ||
			Object==w_slabs[6] ||
			Object==w_slabs[7]){
		// Get savegame index
		int index=-1;
		if(Object==w_slabs[0])	index=(page*8)+0;
		if(Object==w_slabs[1])	index=(page*8)+1;
		if(Object==w_slabs[2])	index=(page*8)+2;
		if(Object==w_slabs[3])	index=(page*8)+3;
		if(Object==w_slabs[4])	index=(page*8)+4;
		if(Object==w_slabs[5])	index=(page*8)+5;
		if(Object==w_slabs[6])	index=(page*8)+6;
		if(Object==w_slabs[7])	index=(page*8)+7;

		// Save game
		if(index!=-1)			Save(index);
		retval=true;
	}

	return retval;
}

