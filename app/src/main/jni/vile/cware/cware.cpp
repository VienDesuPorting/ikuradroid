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

#include "cware.h"


EngineCWare::EngineCWare(int Width,int Height) : EngineVN(Width,Height){
	// Set defaults
	scriptbuffer=0;
	scriptlength=0;
	scriptindex=0;
	saveindex=0;
	state=CWARESTATE_NORMAL;

	// Load gui interface
	EDL_SETRECT(background,32,8,576,376);
	display=new Widget(0,0,640,480);
	textview=new Textview(this);
	selection=new Selection(this);
	map=new CWareMap(this);
	AddWidget(display,VL_BACKGROUND);
	AddWidget(textview,VL_TEXTVIEW);
	AddWidget(selection,VL_CHOICES);
	AddWidget(map,VL_CHOICES);

	// Preset widget configuration
	map->SetVisible(false);
}

EngineCWare::~EngineCWare(){
	if(scriptbuffer){
		delete [] scriptbuffer;
	}
}

/*! \brief Pauses the game
 */
void EngineCWare::Pause(){
	if(Running()){
		scriptindex=saveindex;
		state=CWARESTATE_PAUSED;
	}
}

/*! \brief Unpauses the game
 */
void EngineCWare::Resume(){
	if(state==CWARESTATE_PAUSED){
		state=CWARESTATE_NORMAL;
	}
}

/*! \brief Tells wether a script is loaded
 *  \returns True if a script is properly loaded
 */
bool EngineCWare::Running(){
	return scriptbuffer;
}

/*! \brief Stops the engine and flushes any script data
 */
void EngineCWare::Stop(){
	if(scriptbuffer){
		delete [] scriptbuffer;
	}
	scriptbuffer=0;
	scriptlength=0;
	scriptindex=0;
}

/*! \brief Jumps to an absolute position in the current script
 *  \param Byteposition in the script
 *
 *  This method is typically used from the CWareMap class to resume the
 *  game from the point the user selects. Note that this involves asserting
 *  the processing state of the engine.
 */
bool EngineCWare::Jump(unsigned int Position){
	if(Position<scriptlength){
		scriptindex=Position;
		state=CWARESTATE_NORMAL;
	}
	return (Position<scriptlength);
}

bool EngineCWare::LoadCWareScript(uString Script){
	bool retval=false;
	RWops *blob=0;
	if((blob=LoadScript(Script))){
		// Extract and verify data
		LogVerbose("Loading CWare script: %s",Script.c_str());
		int tlength=blob->Seek(0,SEEK_END);
		if(tlength>(int)0){
			// Replace blob
			blob->Seek(0,SEEK_SET);
			unsigned char *tbuffer=new unsigned char[tlength];
			if(blob->Read(tbuffer,tlength)>(int)0){
				// Register new script data
				if(scriptbuffer){
					delete [] scriptbuffer;
				}
				scriptname=Script;
				scriptbuffer=tbuffer;
				scriptlength=tlength;
				scriptindex=0;
				saveindex=0;
				retval=true;

				// Force new start
				textview->ClearText();
				state=CWARESTATE_NORMAL;
			}
		}
		delete blob;
	}
	return retval;
}

bool EngineCWare::EventLoad(int Index){
	bool retval=false;
	Savegame *load=new Savegame(NativeID(),Index);
	if(load->Read()){
		// Clear existing data
		selection->SetVisible(false);
		EventNew();

		// Load script data
		load->LoadString("scriptname",&scriptname);
		LoadCWareScript(scriptname);
		load->LoadVector("variables",&vars);
		load->LoadUint32("saveindex",(Uint32*)&saveindex);
		load->LoadUint32("saveindex",(Uint32*)&scriptindex);
		state=CWARESTATE_NORMAL;

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

bool EngineCWare::EventSave(int Index){
	// Gather date string
	uString date=EDL_DateString(EDL_UnixTime());
	uString time=EDL_TimeString(EDL_UnixTime());
	uString datetime=date+uString(" ")+time;

	// Store script data
	Savegame *save=new Savegame(NativeID(),Index);
	save->SaveString("scriptname",scriptname);
	save->SaveUint32("saveindex",saveindex);
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

void EngineCWare::EventGameDialog(VN_DIALOGS Dialog){
	if(Dialog==VD_EXTRAS){
		// Stop game features
		textview->ClearText();
		StopMusic();
		StopSound();
		Pause();
	}
	else{
		EngineVN::EventGameDialog(Dialog);
	}
}

void EngineCWare::EventSelect(int Selection){
	if((unsigned int)Selection<scriptlength){
		scriptindex=Selection;
		state=CWARESTATE_NORMAL;
		selection->SetVisible(false);
	}
}

bool EngineCWare::EventGameTick(){
	if(state==CWARESTATE_WAITCLICK){
		bool skip=keyok || keyctrl() || GetSkipmode();
		if(skip && textview->GetRemainingText()){
			textview->CompleteText();
			keyok=false;
		}
		else if(skip){
			StopSound(VA_VOICES);
			state=CWARESTATE_NORMAL;
			textview->ClearText();
			keyok=false;
		}
	}
	return !(state==CWARESTATE_NORMAL && !textview->GetRemainingText());
}

bool EngineCWare::EventGameProcess(){
	bool retval=true;
	if(scriptbuffer && scriptindex<scriptlength){
		Uint16 opcode=GETWORD(scriptbuffer+scriptindex);
		scriptindex+=2;
		if(opcode==0x0000)			retval=OP0000();
		else if(opcode==0x0001)		retval=OP0001();
		else if(opcode==0x0002)		retval=OP0002();
		else if(opcode==0x0003)		retval=OP0003();
		else if(opcode==0x0004)		retval=OP0004();
		else if(opcode==0x0006)		retval=OP0006();
		else if(opcode==0x0007)		retval=OP0007();
		else if(opcode==0x000A)		retval=OP000A();
		else if(opcode==0x0010)		retval=OP0010();
		else if(opcode==0x0011)		retval=OP0011();
		else if(opcode==0x0014)		retval=OP0014();
		else if(opcode==0x0016)		retval=OP0016();
		else if(opcode==0x0018)		retval=OP0018();
		else if(opcode==0x0019)		retval=OP0019();
		else if(opcode==0x001E)		retval=OP001E();
		else if(opcode==0x001F)		retval=OP001F();
		else if(opcode==0x0026)		retval=OP0026();
		else if(opcode==0x0028)		retval=OP0028();
		else if(opcode==0x002B)		retval=OP002B();
		else if(opcode==0x0030)		retval=OP0030();
		else if(opcode==0x0035)		retval=OP0035();
		else if(opcode==0x0036)		retval=OP0036();
		else if(opcode==0x0037)		retval=OP0037();
		else if(opcode==0x0038)		retval=OP0038();
		else if(opcode==0x0040)		retval=OP0040();
		else if(opcode==0x0041)		retval=OP0041();
		else if(opcode==0x0046)		retval=OP0046();
		else if(opcode==0x0047)		retval=OP0047();
		else if(opcode==0x004A)		retval=OP004A();
		else if(opcode==0x004B)		retval=OP004B();
		else if(opcode==0x004C)		retval=OP004C();
		else if(opcode==0x004D)		retval=OP004D();
		else if(opcode==0x004E)		retval=OP004E();
		else if(opcode==0x004F)		retval=OP004F();
		else if(opcode==0x0050)		retval=OP0050();
		else{
			LogMessage("Invalid opcode: 0x%04x",opcode);
		}
	}
	return retval;
}

/*! Print text
 */
bool EngineCWare::OP0000(){
	saveindex=scriptindex-2;
	aString text;
	while(scriptbuffer[scriptindex]){
		if(scriptbuffer[scriptindex]=='@'){
			scriptindex++;
			text+="\"";
		}
		else{
			text+=scriptbuffer[scriptindex++];
		}
	}
	scriptindex++;
	state=CWARESTATE_WAITCLICK;
	textview->PrintText(text);
	return true;
}

bool EngineCWare::OP0001(){
	Uint32 index=GETDWORD(scriptbuffer+scriptindex);
	scriptindex+=4;
	aString text;
	while(scriptbuffer[scriptindex]){
		if(scriptbuffer[scriptindex]=='@'){
			scriptindex++;
			text+="\"";
		}
		else{
			text+=scriptbuffer[scriptindex++];
		}
	}
	scriptindex++;
	int selindex=selection->GetWidgetCount();
	SDL_Rect r=selection->GetPosition();
	r.h=20;
	r.y+=r.h*selindex;
	selection->SetText(text,r,index);
	return false;
}

bool EngineCWare::OP0002(){
	scriptindex=GETDWORD(scriptbuffer+scriptindex);
	return false;
}

/*! Sets multiple variables to the same value
 */
bool EngineCWare::OP0003(){
	Uint16 start=GETWORD(scriptbuffer+scriptindex+0);
	Uint16 end=GETWORD(scriptbuffer+scriptindex+2);
	Uint16 value=GETWORD(scriptbuffer+scriptindex+4);
	scriptindex+=6;
	for(int i=start;i<=end;i++){
		vars.SetUint16(i,value);
	}
	return false;
}

/*! Sets a variable
 */
bool EngineCWare::OP0004(){
	Uint16 index=GETWORD(scriptbuffer+scriptindex+0);
	Uint8 op=GETBYTE(scriptbuffer+scriptindex+2);
	Uint16 v2=GETWORD(scriptbuffer+scriptindex+3);
	scriptindex+=5;

	// Calculate value
	Uint16 v1=vars.GetUint16(index);
	if(op=='=')			v1=v2;
	else if(op=='+')	v1+=v2;
	else if(op=='-')	v1-=v2;
	else{
		LogError("Unknown operator: %c",op);
	}

	// Write back value
	vars.SetUint16(index,v1);
	return false;
}

/*! \brief Clear existing options
 */
bool EngineCWare::OP0006(){
	selection->SetVisible(false);
	selection->Clear();
	return false;
}

/*! \brief Show choices
 */
bool EngineCWare::OP0007(){
	if(selection->GetWidgetCount()){
		selection->SetVisible(true);
		state=CWARESTATE_CHOICE;
	}
	else{
		LogError("Cannot display choices");
	}
	return true;
}

/*! \brief Redisplay choices
 */
bool EngineCWare::OP000A(){
	if(selection->GetWidgetCount()){
		selection->SetVisible(true);
		state=CWARESTATE_CHOICE;
	}
	else{
		LogError("Cannot redisplay old choices");
	}
	return true;
}

bool EngineCWare::OP0010(){
	Uint16 index=GETWORD(scriptbuffer+scriptindex+0);
	Uint8 op=GETBYTE(scriptbuffer+scriptindex+2);
	Uint16 v2=GETWORD(scriptbuffer+scriptindex+3);
	Uint32 pos=GETDWORD(scriptbuffer+scriptindex+5);
	scriptindex+=9;

	// Calculate value
	Uint16 v1=vars.GetUint16(index);
	bool result=false;
	if(op=='=')			result=(v1==v2);
	else if(op=='}')	result=(v1>v2);
	else if(op=='{')	result=(v1<v2);
	else{
		LogError("Unknown operator: %c",op);
	}

	// Jump if NOT equal
	if(!result){
		scriptindex=pos;
	}
	return false;

}

bool EngineCWare::OP0011(){
	//Uint16 delay=GETWORD(scriptbuffer+scriptindex+0);
	//SDL_Delay(delay);
	scriptindex+=2;
	return true;
}

/*! Repaint screen
 */
bool EngineCWare::OP0014(){
	//Uint16 arg=GETWORD(scriptbuffer+scriptindex+0);
	scriptindex+=2;
	return true;
}

/*! Load overlay image
 */
bool EngineCWare::OP0016(){
	// Parse filename
	aString aname;
	while(scriptbuffer[scriptindex]){
		aname+=scriptbuffer[scriptindex++];
	}
	scriptindex++;

	// Load as overlay
	uString name(aname);
	SDL_Surface *overlay=LoadImage(name);
	if(overlay){
		SDL_Surface *surface=EDL_ColorkeySurface(overlay,0);
		display->Blend(surface,0,&background);
		SDL_FreeSurface(surface);
		SDL_FreeSurface(overlay);
		UnlockCG(name);
	}
	else{
		LogError("Failed to load overlay: %s",name.c_str());
	}
	return false;
}

/*! Load script
 */
bool EngineCWare::OP0018(){
	aString name;
	while(scriptbuffer[scriptindex]){
		name+=scriptbuffer[scriptindex++];
	}
	scriptindex++;
	if(!LoadCWareScript(name)){
		LogError("Failed to load %s",name.c_str());
	}
	return true;
}

bool EngineCWare::OP0019(){
	EventGameDialog(VD_TITLE);
	return true;
}

/*! Fade out background to black
 */
bool EngineCWare::OP001E(){
	AddAnimation(new FadeBlack(background,1000));
	return true;
}

/*! Fade out background to white
 */
bool EngineCWare::OP001F(){
	AddAnimation(new FadeWhite(background,1000));
	return true;
}

/*! Start background music
 */
bool EngineCWare::OP0026(){
	aString bgm;
	while(scriptbuffer[scriptindex]){
		bgm+=scriptbuffer[scriptindex++];
	}
	scriptindex++;
	PlayMusic(bgm);
	return false;
}

/*! Stop background music
 */
bool EngineCWare::OP0028(){
	StopMusic();
	return true;
}

/*! Start voice
 */
bool EngineCWare::OP002B(){
	aString sound;
	while(scriptbuffer[scriptindex]){
		sound+=scriptbuffer[scriptindex++];
	}
	scriptindex++;
	PlaySound(sound,VA_VOICES);
	return false;
}

/*! Set screen clipping (Which is handled internally in ViLE)
 */
bool EngineCWare::OP0030(){
	//Uint16 x1=GETWORD(scriptbuffer+scriptindex+0);
	//Uint16 y1=GETWORD(scriptbuffer+scriptindex+2);
	//Uint16 x2=GETWORD(scriptbuffer+scriptindex+4);
	//Uint16 y2=GETWORD(scriptbuffer+scriptindex+6);
	scriptindex+=8;
	return false;
}

/*! Start sound
 */
bool EngineCWare::OP0035(){
	aString sound;
	while(scriptbuffer[scriptindex]){
		sound+=scriptbuffer[scriptindex++];
	}
	scriptindex++;
	PlaySound(sound,VA_SOUNDS);
	return false;
}

/*! Stop sound effect
 */
bool EngineCWare::OP0036(){
	StopSound(VA_SOUNDS);
	return true;
}

/*! Configures/loads graphics for the map
 */
bool EngineCWare::OP0037(){
	// Parse image names
	aString name1;
	aString name2;
	while(scriptbuffer[scriptindex]){
		name1+=scriptbuffer[scriptindex++];
	}
	scriptindex++;
	while(scriptbuffer[scriptindex]){
		name2+=scriptbuffer[scriptindex++];
	}
	scriptindex++;

	// Replace current images
	map->SetBackground(name1);
	map->SetForeground(name2);
	return false;
}

/*! Flush locations from the map
 */
bool EngineCWare::OP0038(){
	map->Flush();
	return false;
}

/*! Add locations to the map
 */
bool EngineCWare::OP0040(){
	Uint32 p=GETDWORD(scriptbuffer+scriptindex+0);
	Uint16 x1=GETWORD(scriptbuffer+scriptindex+4);
	Uint16 y1=GETWORD(scriptbuffer+scriptindex+6);
	Uint16 x2=GETWORD(scriptbuffer+scriptindex+8);
	Uint16 y2=GETWORD(scriptbuffer+scriptindex+10);
	map->AddLocation(x1,y1,x2,y2,p);
	scriptindex+=12;
	return false;
}

/*! Show map
 */
bool EngineCWare::OP0041(){
	map->SetVisible(true);
	state=CWARESTATE_CHOICE;
	return true;
}

/*! Load foreground image
 */
bool EngineCWare::OP0046(){
	aString name;
	while(scriptbuffer[scriptindex]){
		name+=scriptbuffer[scriptindex++];
	}
	scriptindex++;
	SDL_Surface *tmps=LoadImage(name);
	if(tmps){
		display->Blit(tmps);
		SDL_FreeSurface(tmps);
	}
	return false;
}

/*! Load background image
 */
bool EngineCWare::OP0047(){
	aString name;
	while(scriptbuffer[scriptindex]){
		name+=scriptbuffer[scriptindex++];
	}
	scriptindex++;
	SDL_Surface *surface=LoadImage(name);
	if(surface){
		display->Blit(surface,0,&background);
		SDL_FreeSurface(surface);
		UnlockCG(name);
	}
	return false;
}

/*! \brief Repaints clipped screen rect (Not used in ViLE)
 */
bool EngineCWare::OP004A(){
	//Uint16 arg=GETWORD(scriptbuffer+scriptindex+0);
	scriptindex+=2;
	return true;
}

/*! Load single character image
 */
bool EngineCWare::OP004B(){
	// Get name of image and mask
	aString name;
	while(scriptbuffer[scriptindex]){
		name+=scriptbuffer[scriptindex++];
	}
	scriptindex++;
	uString mname=name;
	for(unsigned int i=0;i<name.length();i++){
		if(mname[i]=='_'){
			mname=mname.substr(0,i+1);
			mname+='0';
		}
	}

	// Load as foreground
	SDL_Surface *cg=LoadImage(name);
	SDL_Surface *mask=LoadImage(mname);
	if(cg && mask){
		SDL_Surface *surface=EDL_MaskSurface(cg,mask,0);
		int x=(NativeWidth()-surface->w)/2;
		int y=(background.y+background.h)-surface->h;
		SDL_Rect t={x,y,surface->w,surface->h};
		display->Blend(surface,0,&t);
		SDL_FreeSurface(surface);
		SDL_FreeSurface(mask);
		SDL_FreeSurface(cg);
	}
	else if(cg){
		LogError("Failed to load character mask: %s",mname.c_str());
		SDL_FreeSurface(cg);
	}
	else{
		LogError("Failed to load character image: %s",name.c_str());
	}
	return false;
}

/*! Load two character images
 */
bool EngineCWare::OP004C(){
	// Parse names for first character image set
	aString name1;
	while(scriptbuffer[scriptindex]){
		name1+=scriptbuffer[scriptindex++];
	}
	scriptindex++;
	aString mname1=name1;
	for(unsigned int i=0;i<name1.length();i++){
		if(mname1[i]=='_'){
			mname1=mname1.substr(0,i+1);
			mname1+='0';
		}
	}

	// Parse names for second character image set
	aString name2;
	while(scriptbuffer[scriptindex]){
		name2+=scriptbuffer[scriptindex++];
	}
	scriptindex++;
	uString mname2=name2;
	for(unsigned int i=0;i<name2.length();i++){
		if(mname2[i]=='_'){
			mname2=mname2.substr(0,i+1);
			mname2+='0';
		}
	}

	// Load first as foreground
	SDL_Surface *cg=LoadImage(name1);
	SDL_Surface *mask=LoadImage(mname1);
	if(cg && mask){
		SDL_Surface *surface=EDL_MaskSurface(cg,mask,0);
		int x=((NativeWidth()/2)-surface->w)/2;
		int y=(background.y+background.h)-surface->h;
		SDL_Rect t={x,y,surface->w,surface->h};
		display->Blend(surface,0,&t);
		SDL_FreeSurface(surface);
		SDL_FreeSurface(mask);
		SDL_FreeSurface(cg);
	}
	else if(cg){
		LogError("Failed to load character mask: %s",mname1.c_str());
		SDL_FreeSurface(cg);
	}
	else{
		LogError("Failed to load character image: %s",name1.c_str());
	}

	// Load second as foreground
	cg=LoadImage(name2);
	mask=LoadImage(mname2);
	if(cg && mask){
		SDL_Surface *surface=EDL_MaskSurface(cg,mask,0);
		int x=(((NativeWidth()/2)-surface->w)/2)+(NativeWidth()/2);
		int y=(background.y+background.h)-surface->h;
		SDL_Rect t={x,y,surface->w,surface->h};
		display->Blend(surface,0,&t);
		SDL_FreeSurface(surface);
		SDL_FreeSurface(mask);
		SDL_FreeSurface(cg);
	}
	else if(cg){
		LogError("Failed to load character mask: %s",mname1.c_str());
		SDL_FreeSurface(cg);
	}
	else{
		LogError("Failed to load character image: %s",name1.c_str());
	}
	return false;
}

bool EngineCWare::OP004D(){
	LogTest("ANIMATE BACKGROUND");
	return false;
}

bool EngineCWare::OP004E(){
	LogTest("SCROLL DOWN");
	return false;
}

bool EngineCWare::OP004F(){
	LogTest("SCROLL UP");
	return false;
}

bool EngineCWare::OP0050(){
	aString savename;
	while(scriptbuffer[scriptindex]){
		savename+=scriptbuffer[scriptindex++];
	}
	scriptindex++;
	SetSavename(savename);
	return false;
}

