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

#include "will.h"

EngineWill::EngineWill(int Width,int Height) : EngineVN(Width,Height){
	// Set defaults
	effect=0;
	clearOverlays = false;
	first = true;
	prev_backgroudSurface=0;
	backgroudSurface = 0;
	aniSurface =0;
	prev_aniSurface=0;

    for(int i=0;i<3;i++)
    {
        overlaySurfaces[i]=0;
        prev_overlaySurfaces[i]=0;
    }

	table_data=0;
	table_mouseindex=0;
	anim_data=0;
	script=0;
	choicecount=0;
	ticks_value=0;
	ticks_stamp=SDL_GetTicks();
	state=WILLSTATE_NORMAL;
	script_v2=false;	// YumeMiru-era grammar; CriticalPoint enables the shorter CP blocks

	// Load gui interface
	int owidth=Width/3;
	display=new Widget(0,0,Width,Height);
	overlay[0]=new Widget(owidth*0,0,Width,Height);
	overlay[1]=new Widget(owidth*1,0,Width,Height);
	overlay[2]=new Widget(owidth*2,0,Width,Height);
	textview=new Textview(this);

	selection=new Selection(this);
	AddWidget(display,VL_BACKGROUND);
	AddWidget(overlay[0],VL_BACKGROUND);
	AddWidget(overlay[1],VL_BACKGROUND);
	AddWidget(overlay[2],VL_BACKGROUND);
	AddWidget(textview,VL_TEXTVIEW);
	AddWidget(selection,VL_CHOICES);

	// Prelod 3000 signed shorts. (For Yume only?)
	//vars.SetSint16(3000,0);
}

EngineWill::~EngineWill(){
	Stop();
	if(table_data){
		if(table_data->surface){
			SDL_FreeSurface(table_data->surface);
		}
		delete table_data;
	}
	if(anim_data){
		for(int i=0;anim_data->frames[i];i++){
			SDL_FreeSurface(anim_data->frames[i]);
		}
		delete anim_data;
	}
	if(effect){
		SDL_FreeSurface(effect);
	}
    if(prev_backgroudSurface){
		SDL_FreeSurface(prev_backgroudSurface);
	}

    if(backgroudSurface){
		SDL_FreeSurface(backgroudSurface);
	}

    if(aniSurface){
		SDL_FreeSurface(aniSurface);
	}

    if(prev_aniSurface){
		SDL_FreeSurface(prev_aniSurface);
	}


	for(int i=0;i<3;i++)
    {
        if(overlaySurfaces[i])
            SDL_FreeSurface(overlaySurfaces[i]);
        if(overlaySurfaces[i])
            SDL_FreeSurface(prev_overlaySurfaces[i]);
    }

}

/*! \brief Tells wether a script is loaded
 *  \returns True if a script is properly loaded
 */
bool EngineWill::Running(){
	return script && script->buffer;
}

/*! \brief Stops the engine and flushes any script data
 */
void EngineWill::Stop(){
	while(script){
		if(script->buffer){
			delete [] script->buffer;
		}
		SCRIPT *tmp=script->nextptr;
		delete script;
		script=tmp;
	}
}

/*! \brief Jumps to an absolute position in the current script
 *  \param Byteposition in the script
 */
bool EngineWill::Jump(unsigned int Position){
	bool retval=false;
	if(script){
		if(Position<script->length){
			script->index=Position;
			state=WILLSTATE_NORMAL;
			retval=true;
		}
	}
	return retval;
}

/*! \brief Extracts positional data from WIPF images
 *  \param Name Name of image resource
 *  \param Index Framenumber in image resource
 *  \paeam Rect Rect to be populated with position data
 *
 *  WIPF images has coordinates stored in the header, and the SDL_Surface
 *  structure cannot hold the X and Y positions. This method parses out the
 *  necessary parameters without touching the graphics.
 */
bool EngineWill::GetImagePosition(uString Name,Uint8 Index,SDL_Rect *Rect){
	bool retval;
	RWops *Object=LoadOther(Name,"wip");
	if(Object){
		Uint8 hdr[24];
		Object->Seek(0,SEEK_SET);
		Object->Read(hdr,8);
		unsigned int count=hdr[4]|(hdr[5]<<8);
		if(hdr[0]=='W' && hdr[1]=='I' && hdr[2]=='P' && hdr[3]=='F'){
			unsigned int pos=8+(Index*24);
			if(count>Index){
				Object->Seek(pos,SEEK_SET);
				if(Object->Read(hdr,24)==24){
					int w=hdr[0]|(hdr[1]<<8)|(hdr[2]<<16)|(hdr[3]<<24);
					int h=hdr[4]|(hdr[5]<<8)|(hdr[6]<<16)|(hdr[7]<<24);
					int x=hdr[8]|(hdr[9]<<8)|(hdr[10]<<16)|(hdr[11]<<24);
					int y=hdr[12]|(hdr[13]<<8)|(hdr[14]<<16)|(hdr[15]<<24);
					if(Rect){
						Rect->x=x;
						Rect->y=y;
						Rect->w=w;
						Rect->h=h;
						retval=true;
					}
				}

			}
		}
		delete Object;
	}
	return retval;
}

/*! \brief Loads and parses a Will ANM file
 *  \param Name Name of animation file
 *
 *  Note that the animation infrastructure is often used for other purposes.
 *  such as menu graphics and properties.
 */
WILLANIMATION *EngineWill::LoadAnimation(uString Name){
	WILLANIMATION *retval=0;
	RWops *blob=0;
	if((blob=LoadOther(Name,"anm")) && blob->Size()){
		int size=blob->Size();
		unsigned char buffer[size];
		if(blob->Read(buffer,size)==size){
			// Guard the name extraction against unterminated data
			buffer[size-1]=0;
			retval=new WILLANIMATION();
			retval->name=(char*)buffer;
			retval->frames=EngineVN::LoadAnimation(retval->name);
			if(retval->frames){
				// The animation table is a fixed size structure;
				// refuse to read past the buffer if the file is short
				if(size>=80211){
					for(int i=0;i<100;i++){
						for(int j=0;j<402;j++){
							int a=9+(((i*401)+j)*2);
							retval->entries[i][j]=GETWORD(buffer+a);
						}
					}
				}
				else{
					LogError("Animation table too small: %s (%d bytes)",Name.c_str(),size);
				}
				for(int i=0;i<100;i++){
					retval->visible[i]=true;
				}
			}
			else{
				LogError("Failed to load animation frames: %s",buffer);
				delete retval;
				retval=0;
			}
		}
		delete blob;
	}
	return retval;
}

/*! \brief Loads a Will table
 *  \param Name Table resource
 */
WILLTABLE *EngineWill::LoadTable(uString Name){
	WILLTABLE *retval=0;
	RWops *blob=0;
	if((blob=LoadOther(Name,"tbl")) && blob->Size()){
		int size=blob->Size();
		unsigned char buffer[size];
		if(blob->Read(buffer,size)==size){
			retval=new WILLTABLE();
			retval->count=GETDWORD(buffer);
			retval->surface=LoadImage((char*)buffer+4);
//LogTest("NAME:%s IMAGE:%s COUNT:%d",Name.c_str(),buffer+4,retval->count);
			if(retval->surface){
				// CP-era tables are compact: "count" flag entries
				// follow the 13 byte header and there is no keymap.
				// Bound every read to the actual buffer - the fixed
				// 0xFF-entry loops used to overread short tables
				// (MAINMENU.TBL is 241 bytes) by about a kilobyte.
				int flagspace=(size-13)/4;
				if(flagspace>0xFE){
					flagspace=0xFE;
				}
				for(int i=0;i<flagspace;i++){
					retval->flags[i+1]=GETDWORD(buffer+4+9+(i*4));
				}
				int keymapspace=(size-(4+9+(0xFE*4)))/0x10;
				if(keymapspace>0x12){
					keymapspace=0x12;
				}
				for(int i=0;i<keymapspace;i++){
					for(int j=0;j<0x10;j++){
						int p=4+9+(0xFe*4)+(i*0x10)+j;
						retval->keymap[i][j]=GETBYTE(buffer+p);
					}
				}
				LogTest("Will table: %s (%dx%d, %d zones)",
						Name.c_str(),retval->surface->w,
						retval->surface->h,retval->count);
			}
			else{
				LogError("Failed to load table graphic: %s",buffer+4);
				delete retval;
				retval=0;
			}
		}
		delete blob;
	}
	return retval;
}

/*! \brief Loads a masked Will image (msk+wip=>alpha)
 *  \param Resource name
 */
SDL_Surface *EngineWill::LoadMaskedImage(uString Name){
	SDL_Surface *mask=LoadImage(Name,"msk");
	SDL_Surface *image=LoadImage(Name,"wip");
	if(mask && image){
		// Copy mask
		int w=EDL_MIN(mask->w,image->w);
		int h=EDL_MIN(mask->h,image->h);
		if(SDL_MUSTLOCK(mask)){
			if(SDL_LockSurface(mask)<0){
				return image;
			}
		}
		if(SDL_MUSTLOCK(image)){
			if(SDL_LockSurface(image)<0){
				return 0;
			}
		}
		for(int y=0;y<h;y++){
			for(int x=0;x<w;x++){
				Uint8 *srcp=static_cast<Uint8*>(mask->pixels)+
					(y*mask->pitch)+(x*mask->format->BytesPerPixel);
				Uint8 *dstp=static_cast<Uint8*>(image->pixels)+
					(y*image->pitch)+(x*image->format->BytesPerPixel);
				dstp[3]=srcp[0];
			}
		}
		if(SDL_MUSTLOCK(mask)){
			SDL_UnlockSurface(mask);
		}
		if(SDL_MUSTLOCK(image)){
			SDL_UnlockSurface(image);
		}
	}

	if(mask){

		SDL_FreeSurface(mask);
	}

	return image;
}

/*! \brief Loads a masked Will image (msk+wip=>alpha)
 *  \param Resource name
 */
SDL_Surface **EngineWill::LoadMaskedAnimation(uString Name){
	SDL_Surface **mask=EngineVN::LoadAnimation(Name,"msk");
	SDL_Surface **image=EngineVN::LoadAnimation(Name,"wip");
	for(int i=0;mask[i] && image[i];i++){
		// Copy mask
		int w=EDL_MIN(mask[i]->w,image[i]->w);
		int h=EDL_MIN(mask[i]->h,image[i]->h);
		if(SDL_MUSTLOCK(mask[i])){
			if(SDL_LockSurface(mask[i])<0){
				return 0;
			}
		}
		if(SDL_MUSTLOCK(image[i])){
			if(SDL_LockSurface(image[i])<0){
				return 0;
			}
		}
		for(int y=0;y<h;y++){
			for(int x=0;x<w;x++){
				Uint8 *srcp=static_cast<Uint8*>(mask[i]->pixels)+
					(y*mask[i]->pitch)+(x*mask[i]->format->BytesPerPixel);
				Uint8 *dstp=static_cast<Uint8*>(image[i]->pixels)+
					(y*image[i]->pitch)+(x*image[i]->format->BytesPerPixel);
				dstp[3]=srcp[0];
			}
		}
		if(SDL_MUSTLOCK(mask[i])){
			SDL_UnlockSurface(mask[i]);
		}
		if(SDL_MUSTLOCK(image[i])){
			SDL_UnlockSurface(image[i]);
		}
	}
	if(mask){
		for(int i=0;mask[i];i++){
			SDL_FreeSurface(mask[i]);
		}
		delete [] mask;
	}
	return image;
}


/*! \brief Loads a Will script and pushes it on top of the stack
 *  \param Script Name of the script file
 *  \return True if script loaded
 */
bool EngineWill::LoadWillScript(uString Script){
	bool retval=false;
	RWops *blob=0;
	if((blob=LoadScript(Script,"wsc"))){
		// Extract and verify data
		LogVerbose("Loading Will script: %s",Script.c_str());
		int tlength=blob->Seek(0,SEEK_END);
		if(tlength>(int)0){
			// Replace blob
			blob->Seek(0,SEEK_SET);
			unsigned char *tbuffer=new unsigned char[tlength];
			if(blob->Read(tbuffer,tlength)>(int)0){
				// Decrypt script data
				int p=2%8;
				for(int i=0;i<tlength;i++){
					tbuffer[i]=(tbuffer[i]>>p)|(tbuffer[i]<<(8-p));
				}

				// Push script data to stack
				SCRIPT *tmp=new SCRIPT;
				tmp->name=Script;
				tmp->buffer=tbuffer;
				tmp->length=tlength;
				tmp->index=0;
				tmp->save=0;
				tmp->nextptr=script;
				script=tmp;
				retval=true;

				// Force new start
				textview->ClearText();
				state=WILLSTATE_NORMAL;
			}
		}
		delete blob;
	}
	return retval;
}

bool EngineWill::EventBackgroundMouseMove(int X,int Y){
	if(state==WILLSTATE_GUI){
		if(table_data && table_data->surface){
			SDL_Surface *s=table_data->surface;
			if(X>0 &&X<s->w && Y>0 && Y<s->h){
				Uint8 *pixels=static_cast<Uint8*>
					(s->pixels)+(Y*s->pitch)+
					(X*s->format->BytesPerPixel);
				if(pixels[0]!=table_mouseindex){
					// Get image position and blit it
					table_mouseindex=pixels[0];
					SDL_Rect r;
					if(GetImagePosition(anim_data->name,table_mouseindex,&r)){
						SDL_Surface *frame=anim_data->frames[table_mouseindex];
						if(frame){
							display->Blit(frame,0,&r);
						}
					}
				}
			}
		}
	}
	else{
		return EngineVN::EventBackgroundMouseMove(X,Y);
	}
	return false;
}

bool EngineWill::EventBackgroundMouseLeftDown(int X,int Y){
	bool retval=false;
	LogTest("Will GUI: left down at %d,%d (state=%d, table=%s)",
	                X,Y,(int)state,table_data?"ok":"none");
	if(state==WILLSTATE_GUI){
		if(table_data && table_data->surface){
			SDL_Surface *s=table_data->surface;
			if(X>0 &&X<s->w && Y>0 && Y<s->h){
				Uint8 *pixels=static_cast<Uint8*>
					(s->pixels)+(Y*s->pitch)+
					(X*s->format->BytesPerPixel);
				if(pixels[0]){
					// Set click vars
					vars.SetUint16(table_varclick,1);
					vars.SetUint16(table_varkind,pixels[0]);
					state=WILLSTATE_NORMAL;
					retval=true;
					LogTest("Will GUI click: zone %d at %d,%d",pixels[0],X,Y);
				}
				else{
					LogTest("Will GUI click: miss at %d,%d",X,Y);
				}
			}
		}
	}
	else{
		retval=EngineVN::EventBackgroundMouseLeftDown(X,Y);
	}
	return retval;
}

bool EngineWill::EventBackgroundMouseRightDown(int X,int Y){
	bool retval=false;
	if(state==WILLSTATE_GUI){
		if(table_data && table_data->surface){
			// Set click vars
			vars.SetUint16(table_varclick,-1);
			vars.SetUint16(table_varkind,0);
			state=WILLSTATE_NORMAL;
		}
	}
	else{
		retval=EngineVN::EventBackgroundMouseRightDown(X,Y);
	}
	return retval;
}

bool EngineWill::EventLoad(int Index){
	bool retval=false;
	Savegame *load=new Savegame(NativeID(),Index);
	if(load->Read()){
		// Clear existing data
		selection->SetVisible(false);
		textview->ClearText();
		OnSelectClosed();
		Stop();

		// Recreate script stack
		Uint32 count=0;
		load->LoadUint32("size",&count);
		for(int i=count;i>0;i--){
			uString name;
			Uint32 index;
			load->LoadString(EDL_Format("name-%d",i-1),&name);
			load->LoadUint32(EDL_Format("index-%d",i-1),&index);
			LoadWillScript(name);
			script->index=index;
			script->save=index;
		}
		load->LoadVector("variables",&vars);
		state=WILLSTATE_NORMAL;

		// Load graphics
		SDL_Surface *tmps;
		if(load->LoadSurface("screen-display",&tmps)){
			display->Blit(tmps);
			SDL_FreeSurface(tmps);
		}

		// Close dialog
		retval=true;
	}
	delete load;
	return retval;
}

bool EngineWill::EventSave(int Index){
	// Gather date string
	uString date=EDL_DateString(EDL_UnixTime());
	uString time=EDL_TimeString(EDL_UnixTime());
	uString datetime=date+uString(" ")+time;

	// Store script data
	Savegame *save=new Savegame(NativeID(),Index);
	int count=0;
	SCRIPT *tmpptr=script;
	while(tmpptr){
		int index=tmpptr->nextptr?tmpptr->index:tmpptr->save;
		save->SaveString(EDL_Format("name-%d",count),tmpptr->name);
		save->SaveUint32(EDL_Format("index-%d",count),index);
		tmpptr=tmpptr->nextptr;
		count++;
	}
	save->SaveUint32("size",count);
	save->SaveVector("variables",&vars);
	save->SaveString("savedate",datetime);
	save->SaveString("savemsg",GetSavename());

	// Store graphics
	SDL_Surface *screen=EDL_CreateSurface(NativeWidth(),NativeHeight());
	Paint(screen,VL_BACKGROUND);
	save->SaveSurface("screen-thumb",screen,96,72);
	save->SaveSurface("screen-display",screen);
	SDL_FreeSurface(screen);

	// Close dialogs
	save->Write();
	delete save;
	return true;
}

void EngineWill::EventSelect(int Selection){
	selection->SetVisible(false);
	OnSelectClosed();
	// Seek to the picked item's routing instruction; the regular
	// opcode handlers then perform the jump/call/goto themselves
	if(script && Selection>=0 && Selection<choicecount){
		Jump(choiceroutes[Selection]);
		return;
	}
	// Legacy fallback: name routing through the jumptable
	uString name=jumptable.GetString(Selection);
	if(name.length()){
		LoadWillScript(name);
	}
}

void EngineWill::EventGameDialog(VN_DIALOGS Dialog){
	if(Dialog==VD_TITLE){
		DestroyLayer(VL_OVERLAY);
		DestroyLayer(VL_EXTRAS);
		textview->SetVisible(false);
		selection->SetVisible(false);
		OnSelectClosed();
		LoadWillScript("start");
	}
	else{
		EngineVN::EventGameDialog(Dialog);
	}
}

bool EngineWill::EventGameTick(){
	if(state==WILLSTATE_WAITCLICK){
		bool skip=keyok || keyctrl() || GetSkipmode();
		if(skip && textview->GetRemainingText()){
			textview->CompleteText();
			keyok=false;
		}
		else if(skip){
			StopSound(VA_VOICES);
			state=WILLSTATE_NORMAL;
			textview->ClearText();
			keyok=false;
		}
	}
	return !(state==WILLSTATE_NORMAL && !textview->GetRemainingText());
}


bool EngineWill::EventGameProcess(){
	bool retval=true;
	if(script && script->buffer && script->index<script->length){
		Uint8 opcode=GETBYTE(script->buffer+script->index);
		script->index++;
//LogTest("PROCESS: 0x%02X",opcode);
		// Flow opcodes
		if(opcode==0x01)			retval=OP01();
		else if(opcode==0x02)		retval=OP02();
		else if(opcode==0x03)		retval=OP03();
		else if(opcode==0x04)		retval=OP04();
		else if(opcode==0x06)		retval=OP06();
		else if(opcode==0x07)		retval=OP07();
		else if(opcode==0x09)		retval=OP09();
		else if(opcode==0x0A)		retval=OP0A();
		else if(opcode==0xFF)		retval=OPFF();

		// Audio opcodes
		else if(opcode==0x21)		retval=OP21();
		else if(opcode==0x22)		retval=OP22();
		else if(opcode==0x23)		retval=OP23();
		else if(opcode==0x25)		retval=OP25();
		else if(opcode==0x26)		retval=OP26();
		else if(opcode==0x52)		retval=OP52();

		// Display codes
		else if(opcode==0x46)		retval=OP46();
		else if(opcode==0x47)		retval=OP47();
		else if(opcode==0x48)		retval=OP48();
		else if(opcode==0x49)		retval=OP49();
		else if(opcode==0x64)		retval=OP64();
		else if(opcode==0x68)		retval=OP68();
		else if(opcode==0x73)		retval=OP73();
		else if(opcode==0x74)		retval=OP74();
		else if(opcode==0xB8)		retval=OPB8();

		// Other
		else if(opcode==0x05)		retval=OP05();
		else if(opcode==0x08)		retval=OP08();
		else if(opcode==0x0B)		retval=OP0B();
		else if(opcode==0x0C)		retval=OP0C();
		else if(opcode==0x41)		retval=OP41();
		else if(opcode==0x42)		retval=OP42();
		else if(opcode==0x43)		retval=OP43();
		else if(opcode==0x45)		retval=OP45();
		else if(opcode==0x4A)		retval=OP4A();
		else if(opcode==0x4B)		retval=OP4B();
		else if(opcode==0x4C)		retval=OP4C();
		else if(opcode==0x4D)		retval=OP4D();
		else if(opcode==0x4F)		retval=OP4F();
		else if(opcode==0x50)		retval=OP50();
		else if(opcode==0x51)		retval=OP51();
		else if(opcode==0x54)		retval=OP54();
		else if(opcode==0xB6)		retval=OPB6();
		else if(opcode==0x61)		retval=OP61();
		else if(opcode==0x82)		retval=OP82();
		else if(opcode==0x83)		retval=OP83();
		else if(opcode==0x84)		retval=OP84();
		else if(opcode==0x8B)		retval=OP8B();
		else if(opcode==0xE2)		retval=OPE2();
        ///
        else if(opcode==0x70)		retval=OP70();
		// Trap unknown codes
		else if(opcode==0x28)		retval=OPXX(script_v2?2:3);
		else if(opcode==0x29)		retval=OPXX(script_v2?2:4);
		else if(opcode==0x30)		retval=OPXX(4);
		else if(opcode==0x4E)		retval=OPXX(4);
		else if(opcode==0x55)		retval=OPXX(1);
		else if(opcode==0x62)		retval=OPXX(1);
		else if(opcode==0x76)		retval=OPXX(9);
		else if(opcode==0x85)		retval=OPXX(2);
		else if(opcode==0x86)		retval=OPXX(2);
		else if(opcode==0x88)		retval=OPXX(3);
		else if(opcode==0x89)		retval=OPXX(1);
		else if(opcode==0x8A)		retval=OPXX(script_v2?2:1);
		else if(opcode==0x8C)		retval=OPXX(3);
		else if(opcode==0x16 && script_v2)	retval=OPXX(4);
		else if(opcode==0x27 && script_v2)	retval=OPXX(2);
		else if(opcode==0x9E && script_v2)	retval=OPXX(4);
		else if((opcode==0x0D||opcode==0x11||opcode==0x12)&&script_v2)
						retval=OPXXName(1);
		else if(opcode==0x8E)		retval=OPXX(1);
		else if(opcode==0xBC)		retval=OPXX(4);
		else if(opcode==0xBD)		retval=OPXX(2);
		else if(opcode==0xBE)		retval=OPXX(1);
		else if(opcode==0xE5)		retval=OPXX(1);
		else{
			LogError("Invalid opcode: 0x%02X",opcode);
		}
	}
	return retval;
}

/*! Conditional if
 */
bool EngineWill::OP01(){
	// Parse input data
	Uint8 operation=GETBYTE(script->buffer+script->index+0);
	Uint16 leftreg=GETWORD(script->buffer+script->index+1);
	Uint16 rightreg=GETWORD(script->buffer+script->index+3);
	Uint32 pos=GETDWORD(script->buffer+script->index+5);
	script->index+=10;

	// Parse values and operator
	Uint8 op=operation&0x0F;
	Sint16 leftval=vars.GetSint16(leftreg);
	Sint16 rightval=rightreg;
	if(operation>>4){
		rightval=vars.GetSint16(rightreg);
	}

	// Jump if test fails
	bool test=false;
	switch(op){
		case 1:		test=!(leftval>=rightval);					break;
		case 2:		test=!(leftval<=rightval);					break;
		case 3:		test=!(leftval==rightval);					break;
		case 4:		test=!(leftval!=rightval);					break;
		case 5:		test=!(leftval>rightval);					break;
		case 6:		test=!(leftval<rightval);					break;
		default:	LogError("Unknown IF operator: %d",op);		break;
	}
	if(test){
		Jump(script->index+pos);
	}
	return false;
}

/*! \brief Show selection
 *
 *  Choice grammar of the Russian releases (byte-verified):
 *   v1 (Critical Point): 02 | u16 n | { text\0 route }*
 *   v2 (Little My Maid): 02 | u16 n | { u16 param text\0 route }*
 *  The route is a flow instruction (07 jump / 09 call with a cstring
 *  scene name, 06 goto with a dword offset, 0A return). It is left in
 *  the stream untouched: when the player picks an item, EventSelect
 *  seeks to the route and the regular opcode handlers execute it.
 */
bool EngineWill::OP02(){
	script->save=script->index-1;
	Uint16 n=GETWORD(script->buffer+script->index);
	script->index+=2;

	// Parse the choice block (bounds checked against broken scripts)
	jumptable.Clear();
	choicecount=0;
	Stringlist items;
	for(int i=0;i<n && script->index<script->length;i++){
		if(script_v2){
			// v2 items carry a leading u16 parameter
			script->index+=2;
		}

		// Extract the caption
		aString text;
		while(script->index<script->length && script->buffer[script->index]){
			text+=script->buffer[script->index++];
		}
		script->index++;

		// Empty-caption stubs pad zeros before the routing instruction
		int pad=0;
		while(script->index<script->length &&
				script->buffer[script->index]==0 && pad<4){
			script->index++;
			pad++;
		}

		// Measure the trailing routing instruction
		if(script->index>=script->length){
			LogError("OP02: truncated choice block");
			break;
		}
		Uint8 op=script->buffer[script->index];
		int opsize=0;
		switch(op){
			case 0x07:		// Jump scene: cstring operand
			case 0x09:		// Call script: cstring operand
				opsize=1;
				while(script->index+opsize<script->length &&
						script->buffer[script->index+opsize]){
					opsize++;
				}
				opsize++;
				break;
			case 0x06:		// Goto offset: dword operand + zero pad
				opsize=6;
				break;
			case 0x0A:		// Return: word operand
				opsize=2;
				break;
			default:
				LogError("OP02: unknown choice route 0x%02X",op);
				break;
		}
		if(!opsize || script->index+opsize>script->length){
			LogError("OP02: truncated choice route");
			break;
		}

		// Register the item and its route; display captions get
		// trimmed since the scripts pad them with alignment spaces
		while(text.length() && text[0]==' '){
			text.erase(0,1);
		}
		while(text.length() && text[text.length()-1]==' '){
			text.erase(text.length()-1);
		}
		if(choicecount<WILL_MAX_CHOICES){
			choiceroutes[choicecount++]=script->index;
			items.AddString(text);
		}
		script->index+=opsize;
	}

	// Auto-routing stubs carry empty captions (byte-verified pattern);
	// the original engine falls through their route without showing UI
	bool visible=false;
	for(int i=0;i<choicecount;i++){
		if(items.GetString(i).length()){
			visible=true;
			break;
		}
	}
	if(!visible){
		Jump(choiceroutes[0]);
		return true;
	}

	// Populate the selection widget
	selection->Clear();
	if(choicecount){
		SDL_Surface *normal=LoadMaskedImage("selwnd0");
		SDL_Surface *hover=LoadMaskedImage("selwnd1");
		if(normal && hover){
			// Graphical buttons (releases that ship selwnd graphics)
			int space=10;
			int w=normal->w;
			int h=normal->h;
			int x=(NativeWidth()-w)/2;
			int y=(NativeHeight()-(h*choicecount+space*choicecount))/2;
			for(int i=0;i<choicecount;i++){
				uString text=items.GetString(i);
				SDL_Rect r={x,y,w,h};
				SDL_Surface *s=EDL_CreateSurface(w,h);
				SDL_Surface *u=EDL_CreateSurface(w,h);
				EDL_BlitSurface(hover,0,s,0);
				EDL_BlitSurface(normal,0,u,0);
				EDL_BlendText(text,0xFFFFFFFF,s,0);
				EDL_BlendText(text,0xFFFFFFFF,u,0);
				selection->SetSurface(s,u,r,i);
				SDL_FreeSurface(s);
				SDL_FreeSurface(u);
				y+=(h+space);
			}
		}
	else{
		// Text menu fallback: the RU CP/LMM releases ship no
		// selwnd graphics; the game module restyles its menu
		// through LayoutTextSelection()
		LayoutTextSelection(&items);
	}
		if(normal){
			SDL_FreeSurface(normal);
		}
		if(hover){
			SDL_FreeSurface(hover);
		}
		selection->SetVisible(true);
		state=WILLSTATE_CHOICE;
		return true;
	}

	// Nothing parsed - keep the engine alive and log loudly
	LogError("OP02: no choices parsed at 0x%X",script->save);
	return false;
}

/*! Default text fallback layout: centered translucent strips
 */
void EngineWill::LayoutTextSelection(Stringlist *items){
	// The dialog widget must own an onscreen rectangle or touches
	// never reach the items: EngineBase::GetWidgetAt() hit-tests the
	// dialog itself (Widget::TestMouse) before the embedded buttons
	int w=NativeWidth()/3;
	int h=NativeHeight()/3;
	selection->Move((NativeWidth()-w)/2,(NativeHeight()-h)/2);
	selection->Resize(w,h);
	selection->SetFontSize(NativeHeight()/24);
	selection->SetAlignment(HA_CENTER,VA_CENTER);
	selection->SetColors(0x303030B0,0xFFFFFFFF,0x00000060,0xFFFFFFFF);
	selection->SetText(items);
}

/*! Choice UI dismissed hook (no-op by default)
 */
void EngineWill::OnSelectClosed(){
}

/*! Calculations
 */
bool EngineWill::OP03(){
	// Parse input data
	Uint8 op=GETBYTE(script->buffer+script->index+0);
	Uint16 leftreg=GETWORD(script->buffer+script->index+1);
	Uint8 flag=GETBYTE(script->buffer+script->index+3);
	Uint16 rightreg=GETWORD(script->buffer+script->index+4);
	script->index+=7;

	// Parse values and operator
	Sint16 leftval=vars.GetSint16(leftreg);
	Sint16 rightval=rightreg;
	if(flag){
		rightval=vars.GetSint16(rightreg);
	}
	if(op){
		// Calculate result
		int result=0;
		switch(op){
			case 1:		result=rightval;							break;
			case 2:		result=leftval+rightval;					break;
			case 3:		result=leftval-rightval;					break;
			case 4:		result=vars.GetSint16(rightval);			break;
			case 5:		result=leftval%rightval;					break;
			case 6:		result=rand()%rightval;						break;
			default:	LogError("Unknown CALC operator: %d",op);	break;
		}
		vars.SetSint16(leftreg,result);
	}
	else{
		// Flush temporary flags
		for(int i=0;i<1000;i++){
			vars.SetSint16(i,0);
		}
	}
	return false;
}

/*! Quit application
 */
bool EngineWill::OP04(){
	Stop();
	EventGameShutdown();
	return true;
}

/*! Wait until timer reaces NULL
 */
bool EngineWill::OP05(){
	Uint8 skippable=GETBYTE(script->buffer+script->index+0);
	script->index++;
LogTest("OP05:WAIT:%d",skippable);
	return true;
}

/*! Unconditional jump (Absolute)
 */
bool EngineWill::OP06(){
	Uint32 pos=GETDWORD(script->buffer+script->index+0);
	script->index+=5;
	Jump(pos);
	return false;
}

/*! Sets the timer (In tick at 25 fps)
 */
bool EngineWill::OP0B(){
	Uint16 ticks=GETWORD(script->buffer+script->index+0);
	script->index+=script_v2?4:2;
	//ticks_value=ticks*((1/25)*1000);
	ticks_value=ticks;
	ticks_stamp=SDL_GetTicks();
	return false;
}

/*! Check the timer
 */
bool EngineWill::OP0C(){
	Uint16 flag=GETWORD(script->buffer+script->index+0);
	script->index+=3;

	// Check timer ticks
	if(ticks_value){
		Uint32 now=SDL_GetTicks();
		Uint32 ticked=now-ticks_stamp;
		ticks_stamp=now;
		if(ticks_value>ticked){
			ticks_value-=ticked;
		}
		else{
			ticks_value=0;
		}
	}
	vars.SetSint16(flag,ticks_value==0);
LogTest("OP0C:TIMER:%d=%d",flag,ticks_value);
	return false;
}

/*! Load new script
 */
bool EngineWill::OP07(){
	aString text;
	while(script->buffer[script->index]){
		text+=script->buffer[script->index++];
	}
	script->index++;
	Stop();
	LogVerbose("Loading script: %s",text.c_str());
	LoadWillScript(text);
	return false;
}

/*! Set text size (big/small)
 */
bool EngineWill::OP08(){
	Uint16 size=GETWORD(script->buffer+script->index+0);
	script->index+=2;
LogTest("OP08:Size=%d",size);
	return false;
}

/*! Call script
 */
bool EngineWill::OP09(){
	aString text;
	while(script->buffer[script->index]){
		text+=script->buffer[script->index++];
	}
	script->index++;
	LogVerbose("Calling script: %s",text.c_str());
	LoadWillScript(text);
	return false;
}

/*! Return from a called script
 */
bool EngineWill::OP0A(){
	LogVerbose("Returning");
	//Uint8 param=GETBYTE(script->buffer+script->index+0);
	if(script->buffer){
		delete [] script->buffer;
	}
	SCRIPT *tmp=script->nextptr;
	delete script;
	script=tmp;
	return false;
}

/*! Play musac
 */
bool EngineWill::OP21(){
	//Uint8 repeats=GETBYTE(script->buffer+script->index+0);
	//Uint16 fadein=GETWORD(script->buffer+script->index+1);
	script->index+=script_v2?1:3;
	aString text;
	while(script->buffer[script->index]){
		text+=script->buffer[script->index++];
	}
	script->index++;
	PlayMusic(text);
	return false;
}

/*! Stop musac
 */
bool EngineWill::OP22(){
	//Uint16 fadeout=GETWORD(script->buffer+script->index+1);
	script->index+=script_v2?1:4;
	StopMusic();
	return false;
}

/*! Play voice
 */
bool EngineWill::OP23(){
	Uint8 channel=GETBYTE(script->buffer+script->index+0);
	//Uint8 male=GETBYTE(script->buffer+script->index+4);
	script->index+=script_v2?5:7;
	aString text;
	while(script->buffer[script->index]){
		text+=script->buffer[script->index++];
	}
	script->index++;
	PlayVoice(text,channel);
	return false;
}

/*! Play sound
 */
bool EngineWill::OP25(){
	Uint8 channel=GETBYTE(script->buffer+script->index+0);
	Uint8 repeat=GETBYTE(script->buffer+script->index+1);
	//Uint8 start=GETBYTE(script->buffer+script->index+3);
	//Uint16 fadein=GETWORD(script->buffer+script->index+4);
	//Uint16 volume=GETWORD(script->buffer+script->index+6);
	script->index+=script_v2?4:8;
	aString text;
	while(script->buffer[script->index]){
		text+=script->buffer[script->index++];
	}
	script->index++;
	PlaySound(text,channel,repeat);
	return false;
}

/*! Stop sound
 */
bool EngineWill::OP26(){
	Uint16 channel=GETWORD(script->buffer+script->index+0);
	script->index+=script_v2?4:2;
	StopSound(channel);
	return false;
}

/*! Text without title or caption
 */
bool EngineWill::OP41(){
	script->save=script->index-1;
	script->index+=3;
	uString text;
	while(script->buffer[script->index]){
		if(script->buffer[script->index]=='\\'){
			char escape=script->buffer[++script->index];
			script->index++;
			if(escape=='n' || escape=='N'){
				text+="\n";
			}
			else{
				LogError("Invalid escape character: \\%c",escape);
			}
		}
		else{
			char escape=script->buffer[script->index++];
			text+=escape;
		}
	}

	script->index++;
	textview->SetVisible(true);

	textview->PrintText(text);
	state=WILLSTATE_WAITCLICK;
	return true;
}

/*! Text with title (Usually a name)
 */
bool EngineWill::OP42(){
	//Uint16 id=GETWORD(script->buffer+script->index+0);
	script->index+=4;
	aString title;
	while(script->buffer[script->index]){
		title+=script->buffer[script->index++];
	}
	script->index++;
	aString text;
	while(script->buffer[script->index]){
		if(script->buffer[script->index]=='\\'){
			char escape=script->buffer[++script->index];
			script->index++;
			if(escape=='n' || escape=='N'){
				text+="\n";
			}
			else{
				LogError("Invalid escape character: \\%c",escape);
			}
		}
		else{
			text+=script->buffer[script->index++];
		}
	}
	script->index++;
	textview->SetVisible(true);
	textview->ClearText();
	textview->PrintText(title,text);
	state=WILLSTATE_WAITCLICK;
	return true;
}

/*! Load animation
 */
bool EngineWill::OP43(){
	// Parse parameters
	script->index+=6;
	aString text;
	while(script->buffer[script->index]){
		text+=script->buffer[script->index++];
	}
	script->index++;

	// Load animation
	LogVerbose("Loading animation file: %s",text.c_str());
	if(anim_data && anim_data->frames){
		for(int i=0;anim_data->frames[i];i++){
			SDL_FreeSurface(anim_data->frames[i]);
		}
		delete anim_data;
		anim_data=0;
	}
//*
	anim_data=LoadAnimation(text);
	// Display background frame
	SDL_Rect rect;
	if(anim_data && anim_data->frames && anim_data->frames[0] &&
				GetImagePosition(anim_data->name,0,&rect)){
		SDL_Surface *frame=anim_data->frames[0];

		if (!prev_aniSurface)
            prev_aniSurface = EDL_CreateSurface(frame->w,frame->h);
        if (!aniSurface)
            aniSurface = EDL_CreateSurface(frame->w,frame->h);

        SDL_Surface *displaySurface = display->GetSurface();

        if (displaySurface)
            EDL_BlendSurface(displaySurface,0,prev_aniSurface,0);

		if(frame){
            EDL_BlendSurface(frame,0,aniSurface,&rect);
		//	display->Blit(frame,0,&rect);
		}
	}
	else if(!anim_data){
		// Broken or missing WIPF resource: keep running without
		// the animation instead of dereferencing a null pointer
		LogError("Failed to load animation: %s",text.c_str());
	}//*/
	return false;
}

/*! Enable animation in table
 */
bool EngineWill::OP45(){
	Uint16 index=GETWORD(script->buffer+script->index+1);
	script->index+=4;
	if(anim_data){
		anim_data->visible[index]=true;
	}
	return false;
}

/*! Load background
 */
bool EngineWill::OP46(){
	Uint16 x=GETWORD(script->buffer+script->index+0);
	Uint16 y=GETWORD(script->buffer+script->index+2);
	//Uint8 i=GETBYTE(script->buffer+script->index+8);
	script->index+=9;
	aString text;
	while(script->buffer[script->index]){
		text+=script->buffer[script->index++];
	}
	script->index++;
	SDL_Surface *image=LoadImage(text);
	LogVerbose("LoadImage%s",text.c_str());
	if(image){
		SDL_Rect r={x,y,image->w,image->h};
		if (!prev_backgroudSurface)
            prev_backgroudSurface = EDL_CreateSurface(image->w,image->h);
        if (!backgroudSurface)
            backgroudSurface = EDL_CreateSurface(image->w,image->h);

        SDL_Surface *displaySurface = display->GetSurface();

        if (displaySurface)
            EDL_BlendSurface(displaySurface,0,prev_backgroudSurface,0);

        EDL_BlendSurface(image,0,backgroudSurface,0);

		//display->Blit(image,&r);
		SDL_FreeSurface(image);
	}
	else{
		LogError("Failed to load background: %s",text.c_str());
	}
	return false;
}

/*! Set background color
 */
bool EngineWill::OP47(){
	Uint8 color=GETBYTE(script->buffer+script->index+0);
	script->index+=2;
	if(color==0){
		display->Fill(0x000000FF);
	}
	else{
		LogError("Unknown background color: %d",color);
	}
	return false;
}

/*! Load character graphics
 */
bool EngineWill::OP48(){
    LogVerbose("Loadcharacter");
	Uint8 id=GETBYTE(script->buffer+script->index+0);
	//Uint16 x=GETWORD(script->buffer+script->index+1);
	//Uint16 y=GETWORD(script->buffer+script->index+3);
	//Uint8 index=GETBYTE(script->buffer+script->index+9);
	script->index+=10;
	aString text;
	while(script->buffer[script->index]){
		text+=script->buffer[script->index++];
	}
	 LogVerbose("Loadcharacter2");
	script->index++;
	SDL_Surface *image=0;
	if(id>2){
		LogError("Invalid character id: %d",id);
	}
	else{
		image=LoadMaskedImage(text);
	}
	if(image){
             LogVerbose("Loadcharacter3");
       // if (!overlaySurfaces[id])
        SDL_FreeSurface(overlaySurfaces[id]);
        overlaySurfaces[id]=0;
        if (!overlaySurfaces[id])
            overlaySurfaces[id] = EDL_CreateSurface(image->w,image->h);
        EDL_BlendSurface(image,0,overlaySurfaces[id],0);

        LogVerbose("Loadcharacter4");
		//overlay[id]->Blit(image);
		SDL_FreeSurface(image);
	}
	else{
		LogError("Failed to load foreground: %s",text.c_str());
	}
	return false;
}

/*! Add overlay
 */
bool EngineWill::OP73(){
	Uint16 x=GETWORD(script->buffer+script->index+0);
	Uint16 y=GETWORD(script->buffer+script->index+2);
	script->index+=9;
	aString text;
	while(script->buffer[script->index]){
		text+=script->buffer[script->index++];
	}
	script->index++;
	LogTest("ADD OVERLAY:%d:%d:%s",x,y,text.c_str());
	return false;
}

/*! Clear center overlay
 */
bool EngineWill::OP49(){
	Uint16 id=GETWORD(script->buffer+script->index+0);
	script->index+=3;
	if(id>2){
		LogError("Invalid overlay id: %d",id);
	}
	else{
        LogError("Clear left overlay id: %d",id);
        clearOverlays = true;
		//overlay[id]->Free();
	}
	return false;
}

/*! Clear right overlay
 */
bool EngineWill::OPB8(){
	Uint16 id=GETWORD(script->buffer+script->index+0);
	script->index+=3;
	if(id>2){
		LogError("Invalid overlay id: %d",id);
	}
	else{
        LogError("Clear right overlay id: %d",id);
        clearOverlays = true;
		//overlay[id]->Free();
	}
	return false;
}


/*! Clear overlay
 */
bool EngineWill::OP74(){
	Uint16 index=GETWORD(script->buffer+script->index+0);
	script->index+=2;
	LogTest("CLEAR OVERLAY:%d",index);
	return false;
}

/*! Set transition
 *
 *  The transition timing is very slow and slogs down the game unreasonably
 *  much. Disabling it seems to work *very* fine, but checking the type might
 *  be a good idea.
 */
bool EngineWill::OP4A(){
	//Uint8 type=GETBYTE(script->buffer+script->index+0);
	Uint16 time;
	if(script_v2){
		// CP/LMM scripts pack the transition as "4A <u16 time>"
		// (3 bytes total); verified on MAINMENU.SCR of both games:
		// the byte that used to be skipped here was OP50's opcode,
		// which silently disabled table (GUI zone) loading
		time=GETWORD(script->buffer+script->index+0);
		script->index+=2;
	}
	else{
		// YumeMiru-era scripts carry one extra byte
		time=GETWORD(script->buffer+script->index+1);
		script->index+=4;
	}
	LogVerbose("Transition time: %dmS",time);
	SetTransit(time);
	return true;
}

void EngineWill::SetTransit(Uint32 Duration){
   first = false;
    if (first){
        first = false;
    }
    else
    {
        bool quit = false;
        Uint32 start=SDL_GetTicks();
       // Duration+=1500;

        SDL_Surface *AllSurface = EDL_CreateSurface(800,600);
        SDL_Surface *prev_AllSurface = EDL_CreateSurface(800,600);


        if (backgroudSurface)
            EDL_BlendSurface(backgroudSurface,0,AllSurface,0);
        if (prev_backgroudSurface)
            EDL_BlendSurface(prev_backgroudSurface,0,prev_AllSurface,0);

        if (aniSurface)
                EDL_BlendSurface(aniSurface,0,AllSurface,0);
        if (aniSurface)
                EDL_BlendSurface(prev_aniSurface,0,prev_AllSurface,0);

        for(int id=0;id<3;id++)
        {
            if (clearOverlays)
                overlay[id]->Free();

            if (overlaySurfaces[id])
            {
                SDL_Rect pos = overlay[id]->GetPosition();
                EDL_BlendSurface(overlaySurfaces[id],0,AllSurface,&pos);
                overlay[id]->Blit(overlaySurfaces[id]);
                if (prev_overlaySurfaces[id])
                    EDL_BlendSurface(prev_overlaySurfaces[id],0,prev_AllSurface,&pos);
            }
        }


		EDLTexture *gTexture1=EDL_CreateTexture(prev_AllSurface->w,prev_AllSurface->h);
        EDLTexture *gTexture2=EDL_CreateTexture(AllSurface->w,AllSurface->h);

        gTexture1->lockTexture();
        gTexture1->copyPixels( prev_AllSurface->pixels );
        gTexture1->unlockTexture();

        if (!effect)
        {
             gTexture2->lockTexture();
             gTexture2->copyPixels( AllSurface->pixels );
             gTexture2->unlockTexture();
        }

        if(!start && Duration){
		// Start effect
            LogVerbose("SDL_GetTicks();%dmS",Duration);
            start=SDL_GetTicks();
        }
        else
            while(!quit){
            SDL_RenderClear(EDLRenderer);
            Uint32 now=SDL_GetTicks();

			// Move one step
			double progress=(now-start)/(double)Duration;
            progress=255-(EDL_HANNING(progress,2)*255);

            if (!effect)
            {
                gTexture2->setAlpha((Uint8)EDL_LIMIT(progress,0x00,0xFF));
            }
            else
            {
                EDL_setupImageAlpha(AllSurface,effect,(Uint8)EDL_LIMIT(progress,0x00,0xFF));
                gTexture2->lockTexture();
                gTexture2->copyPixels( AllSurface->pixels );
                gTexture2->unlockTexture();
            }

            gTexture1->render(0,0);
            gTexture2->render(0,0);
            textview->Render();
            SDL_RenderPresent(EDLRenderer);

            if(now>=start+Duration){
                gTexture1->free();
                gTexture2->free();
                gTexture1 = 0;
                gTexture2 = 0;
                display->Blit(backgroudSurface,0);


                if (aniSurface)
                {
                        display->Blit(aniSurface,0);
                        SDL_FreeSurface(aniSurface);
                        aniSurface=0;
                }

                if (prev_aniSurface)
                {
                        SDL_FreeSurface(prev_aniSurface);
                        prev_aniSurface=0;
                }

                for(int id=0;id<3;id++)
                {
                    if (prev_overlaySurfaces[id])
                    {
                        SDL_FreeSurface(prev_overlaySurfaces[id]);
                        prev_overlaySurfaces[id]=0;
                    }
                    if (overlaySurfaces[id]){
                        prev_overlaySurfaces[id] = EDL_CreateSurface(overlaySurfaces[id]->w,overlaySurfaces[id]->h);
                        EDL_BlendSurface(overlaySurfaces[id],0,prev_overlaySurfaces[id],0);
                        SDL_FreeSurface(overlaySurfaces[id]);
                        overlaySurfaces[id]=0;
                    }
                }

                SDL_FreeSurface(AllSurface);
                SDL_FreeSurface(prev_AllSurface);
                SDL_FreeSurface(effect);
                effect=0;
                clearOverlays = false;
                quit =true;
            }
        }
    }
    LogVerbose("SetTransitEnd");
}

/*! Add animation
 */
bool EngineWill::OP4B(){
	Uint8 id=GETBYTE(script->buffer+script->index+0);
	Uint16 x=GETWORD(script->buffer+script->index+1);
	Uint16 y=GETWORD(script->buffer+script->index+3);
	Uint16 time=GETWORD(script->buffer+script->index+5);
	Uint16 alpha=GETWORD(script->buffer+script->index+9);
	script->index+=12;
	LogVerbose("ADD ANIMATION:%d:%d:%d:%d:%d",id,x,y,time,alpha);
	return false;
}

/*! Play animation
 */
bool EngineWill::OP4C(){
	//Uint8 skippable=GETBYTE(script->buffer+script->index+0);
	script->index++;
	LogVerbose("PLAY ANIMATION");
	return true;
}

/*! Load effect
 */
bool EngineWill::OP4D(){
	Uint8 kind=GETBYTE(script->buffer+script->index+0);
	Uint8 duration=GETBYTE(script->buffer+script->index+1);
	Uint16 factor=GETWORD(script->buffer+script->index+2);
	script->index+=5;
	if(kind==1){
		LogTest("EFFECT:QUAKE:%dx%d",duration,factor);
	}
	else if(kind==2){
		LogTest("EFFECT:HEAT:%dx%d",duration,factor);
	}
	else{
		LogError("Unknown effect: %d",kind);
	}
	return true;
}

/*! Disable animation in table
 */
bool EngineWill::OP4F(){
	Uint16 index=GETWORD(script->buffer+script->index+1);
	script->index+=4;
	if(anim_data){
		anim_data->visible[index]=false;
	}
	return false;
}

/*! Load table
 *
 *  Loads table_data which contains graphics for GUI and menues
 */
bool EngineWill::OP50(){
	aString text;
	while(script->buffer[script->index]){
		text+=script->buffer[script->index++];
	}
	script->index++;
	LogVerbose("Loading Will table: %s",text.c_str());
	if(table_data){
		if(table_data->surface){
			SDL_FreeSurface(table_data->surface);
		}
		delete table_data;
	}
	table_data=LoadTable(text);
	if(!table_data){
		LogError("Will table not found: %s",text.c_str());
	}
	return false;
}

/*! Select table item (From table_data)
 */
bool EngineWill::OP51(){
	Uint16 flag_click=GETWORD(script->buffer+script->index+0);
	Uint16 flag_kind=GETWORD(script->buffer+script->index+2);
	selection->SetVisible(false);
	textview->SetVisible(false);
	table_varclick=flag_click;
	table_varkind=flag_kind;
	state=WILLSTATE_GUI;
	LogTest("Will GUI: waiting clicks (click=var%d, kind=var%d)",
				flag_click,flag_kind);
	script->index+=5;
	return true;
}

/*! Delay sound (Ignored)
 */
bool EngineWill::OP52(){
	//Uint16 delay=GETWORD(script->buffer+script->index+0);
	//LogTest("Delay sound:%d",delay);
	script->index+=2;
	return false;
}

/*! Load transparent image
 */
bool EngineWill::OP54(){
	aString text;
	while(script->buffer[script->index]){
		text+=script->buffer[script->index++];
	}
	script->index++;
	if(effect){
		SDL_FreeSurface(effect);
	}
	if((effect=LoadMaskedImage(text))){
		LogVerbose("Loaded effect image:%s",text.c_str());
	}
	else{
		LogError("Failed to load effect:%s",text.c_str());
	}
	return false;
}

/*! Play video
 */
bool EngineWill::OP61(){
	//Uint8 stoppable=GETBYTE(script->buffer+script->index+0);
	script->index++;
	aString text;
	while(script->buffer[script->index]){
		text+=script->buffer[script->index++];
	}
	script->index++;
	LogVerbose("Loading video: %s",text.c_str());
    PlayVideo(text);
	//QueueVideo(text);
	return true;
}

//!/////////////////////////////////////////////////////////////////////////////
bool EngineWill::OP70()
{
    script->index-=1;
	aString text;
	while(script->buffer[script->index]){
		text+=script->buffer[script->index++];
	}
	script->index++;
	LogTest("play se= %s",text.c_str() );

	Uint8 channel=1;//GETBYTE(script->buffer+script->index+0);
	Uint8 repeat=0;//GETBYTE(script->buffer+script->index+1);
	PlaySound(text,channel,repeat);
	//PlayMusic(text);
	return true;
}

//!//////////////////////////////////////////////////////////////////////////////

/*! Show character info (Not supported)
 */
bool EngineWill::OP64(){
	//Uint8 index=GETBYTE(script->buffer+script->index+0);
	//Uint8 size=GETBYTE(script->buffer+script->index+1);
	//Uint8 rotation=GETBYTE(script->buffer+script->index+2);
	script->index+=4;
	LogTest("Show character info");
	return false;
}

/*! Set background size and position
 */
bool EngineWill::OP68(){
	//Sint16 size=GETBYTE(script->buffer+script->index+0);
	//Sint16 xbase=GETBYTE(script->buffer+script->index+2);
	//Sint16 ybase=GETBYTE(script->buffer+script->index+4);
	script->index+=7;
	return false;
}


/*! Delay execution
 */
bool EngineWill::OP82(){
	Uint16 delay=GETWORD(script->buffer+script->index+0);
	script->index+=3;
	LogTest("OP82:DELAY:%d",delay);
	SDL_Delay(delay);
	return true;
}

/*! Load game
 */
bool EngineWill::OP83(){
	script->index++;
	EventGameDialog(VD_LOAD);
	return true;
}

/*! New game
 */
bool EngineWill::OP84(){
	script->index++;
LogTest("OP84:NEW:SAVE");
	return true;
}

/*! Configuration screen
 */
bool EngineWill::OP8B(){
	script->index++;
	EventGameDialog(VD_OPTIONS);
	return true;
}

/*! Quick load (Autoload save 998)
 */
bool EngineWill::OPE2(){
	script->index++;
	EventLoad(998);
	return false;
}

/*! Text ...
 */
bool EngineWill::OPB6(){
	Uint16 id=GETWORD(script->buffer+script->index+0);
	script->index+=2;
	aString text;
	while(script->buffer[script->index]){
		text+=script->buffer[script->index++];
	}
	script->index++;
	LogTest("OPB6:%d:%s",id,text.c_str());
	return false;
}


/*! End of file
 */
bool EngineWill::OPFF(){
	LogTest("END OF FILE");
	return true;
}

/*! Unknown OP
 */
bool EngineWill::OPXX(int Length){
	script->index+=Length;
	return false;
}

/*! Skip unknown opcode with a trailing name string; used for the
 *  LMM speaker-label family (0x0D/0x11/0x12: <u8> <name\0>)
 */
bool EngineWill::OPXXName(int Length){
	script->index+=Length;
	while(script->index<script->length && script->buffer[script->index]){
		script->index++;
	}
	if(script->index<script->length){
		script->index++;
	}
	return false;
}


