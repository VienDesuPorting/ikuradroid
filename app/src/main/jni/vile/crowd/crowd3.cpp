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

#include "crowd3.h"

EngineCrowd3::EngineCrowd3(int Width,int Height) : EngineVN(Width,Height) {
	// Set defaults
	state=C3_NORMAL;
	csbuffer=0;
	cslength=0;
	csindex=0;
	selcount=0;

	// Load cache objects
	bgcache=new Cache(4);
	fgcache=new Cache(8);

	// Load default gui interface
	display_bg=new Widget(0,0,Width,Height);
	display_event=new Widget(0,0,Width,Height);
	display_cg[0]=new Widget((Width/4)*0,0,Width,Height);
	display_cg[1]=new Widget((Width/4)*1,0,Width,Height);
	display_cg[2]=new Widget((Width/4)*2,0,Width,Height);
	textview=new Textview(this);
	selection=new Selection(this);
	title=new StdTitle(this);
	AddWidget(display_bg,VL_BACKGROUND);
	AddWidget(display_event,VL_BACKGROUND);
	AddWidget(display_cg[0],VL_BACKGROUND);
	AddWidget(display_cg[1],VL_BACKGROUND);
	AddWidget(display_cg[2],VL_BACKGROUND);
	AddWidget(textview,VL_TEXTVIEW);
	AddWidget(selection,VL_CHOICES);
	AddWidget(title,VL_DIALOG);
}

EngineCrowd3::~EngineCrowd3(){
	if(csbuffer){
		delete [] csbuffer;
	}
	delete bgcache;
	delete fgcache;
}

/*! \brief Parses next word using given parser character
 *	\param Parser Which character to look for
 *	\returns Parsed text
 *
 *	Any parser characters found before anything else will be ignored
 */
uString EngineCrowd3::ParseWord(char Parser){
	if(csbuffer && cslength){
		// Find the word
		int start=csindex;
		while(start<cslength && csbuffer[start]==Parser && ++start);
		int end=start;
		while(end<cslength && csbuffer[end]!=Parser && ++end);

		// Copy data to a nullterminated buffer
		char tmp[(end-start)+1];
		tmp[end-start]=0;
		for(int i=0;i<end-start;i++){
			tmp[i]=csbuffer[start+i];
		}
		return tmp;
	}
	return "";
}

/*! \brief Parses next word and moves the position beyond it
 *	\param Parser Which character to look for
 *
 *	Any parser characters found before anything else will be ignored
 */
void EngineCrowd3::SkipWord(char Parser){
	if(csbuffer && cslength){
		// Find the word
		while(csindex<cslength && csbuffer[csindex]==Parser && ++csindex);
		while(csindex<cslength && csbuffer[csindex++]!=Parser);
	}
}

/*! \brief Parses input text string, formats those with names and strips undesired meta-characters
 *	\param input pointer to input string
 *	\return True if altered by this function.
 *	Any parser characters found before anything else will be ignored. 
 *	I'll at least change the @? case to handle italics later.
 *	Technically, this function should always return true for now...I'll fix that issue later.
 */
bool EngineCrowd3::CleanText(char *input){
	bool retval = false;
	int m, n;
	if (input[0] == '@' && input[1] == '!') {						// If we start the string with @! it means that we are starting with a name (it technically means bold, but we can take advantage of the habit since it is consistent)
		for (m=3, n=0; input[m] != 0 && input[m] != 10 && input[m] != '@'; m++, n++)	// so we parse every individual character starting from the 4th until we run into our inevitable @N after the name
			input[n] = input[m];
		if (input[m] == '@' && input[m+1] == 'N') {					// Once we reach that @N
			input[n] = 10;											// replace @N after NAME with LF
			n++;
			m += 3;													// skip additional "@N " characters in the line, keep everything else
			for(m=m; input[m] != 0 && input[m] != 10; m++) {
					if (input[m] == '@' && input[m+1] == 'N')		// remove @N
						m++;
					else {
						input[n] = input[m];
						n++;
					}
			}
			retval = true;
		}
		input[n] = 0;											// terminate in case LF was terminator
	}
	else if (input[0] == '@' && input[1] == '?') {					// There is always that chance we start with @?, which would normally indicate italics, but I'm too lazy to look up how to set to italics at the moment
		for(m=3,n=0; input[m] != 0 && input[m] != 10; m++) {		// Now lets make sure we clear out the leading @? and then ditch our @Ns
			if (input[m] == '@' && input[m+1] == 'N')				// remove @Ns
				m++;
			else {
				input[n] = input[m];
				n++;
			}
			retval = true;
		}
		input[n] = 0;											// terminate in case LF was terminator
	}
	else{
		for(m=0,n=0; input[m] != 0 && input[m] != 10; m++) {		// If neither is the case, just blow up the newlines
			if (input[m] == '@' && input[m+1] == 'N')				// remove @Ns
				m++;
			else {
				input[n] = input[m];
				n++;
			}
			retval = true;
		}
		input[n] = 0;											// terminate in case LF was terminator
	}
	return retval;
}

// Show Selections
bool EngineCrowd3::ShowSelection(Stringlist List){
	LogError("Unhandled selections:");
	for(int i=0;i<List.GetCount();i++){
		LogError("\t%s",List.GetString(i).c_str());
	}
	return false;
}

/*! \brief Loads a script and prepare it for parsing
 *	\param Script Name of the script resource to load
 *	\return TRUE if script was found and loaded
 */
bool EngineCrowd3::LoadCrowdScript(uString Script){
	bool retval=false;
	RWops *ops=0;
	if((ops=LoadScript(Script))){
		// Extract and verify data
		int tlength=ops->Seek(0,SEEK_END);
		if(tlength>(int)0){
			// Replace ops
			ops->Seek(0,SEEK_SET);
			char *tbuffer=new char[tlength];
			if(ops->Read(tbuffer,tlength)>(int)0){
				if(csbuffer){
					delete [] csbuffer;
				}
				csname=Script;
				csbuffer=tbuffer;
				cslength=tlength;
				csindex=0;
				saveindex=0;
				retval=true;
			}
			else{
				delete [] tbuffer;
			}

			// Free resource ops
			delete ops;
		}
	}
	else{
		LogError("Cant find script: %s",Script.c_str());
	}
	return retval;
}

bool EngineCrowd3::EventLoad(int Index){
	bool retval=false;
	Savegame *load=new Savegame(NativeID(),Index);
	if(load->Read()){
		// Clear existing data
		selection->SetVisible(false);

		// Load script data
		load->LoadString("csname",&csname);
		LoadCrowdScript(csname);
		load->LoadVector("variables",&vars);
		load->LoadUint32("csindex",(Uint32*)&saveindex);
		state=C3_NORMAL;

		// Load graphics
		SDL_Surface *tmps;
		if(load->LoadSurface("screen-display",&tmps)){
			display_bg->Blit(tmps);
			SDL_FreeSurface(tmps);
			display_event->Free();
			display_cg[0]->Free();
			display_cg[1]->Free();
			display_cg[2]->Free();
		}

		// Close dialog
		retval=true;
	}
	delete load;
	return retval;
}

bool EngineCrowd3::EventSave(int Index){
	// Gather date string
	uString date=EDL_DateString(EDL_UnixTime());
	uString time=EDL_TimeString(EDL_UnixTime());
	uString datetime=date+uString(" ")+time;

	// Store script data
	Savegame *save=new Savegame(NativeID(),Index);
	save->SaveString("csname",datetime);
	save->SaveUint32("csindex",saveindex);
	save->SaveVector("variables",&vars);
	save->SaveString("savedate",datetime);
	save->SaveString("savemsg",GetSavename());

	// Store graphics
	SDL_Surface *screen=EDL_CreateSurface(NativeWidth(),NativeHeight());
	Paint(screen,VL_TEXTVIEW);
	save->SaveSurface("screen-thumb",screen,96,72);
	save->SaveSurface("screen-display",screen);
	SDL_FreeSurface(screen);

	// Close dialogs
	save->Write();
	delete save;
	return true;
}

/*! \brief Set selection result
 *  \param Selection Selection index
 *
 *  This moves the script position to the byte after the SD INDEX instruction,
 *  where index matches the Selection index.
 */
void EngineCrowd3::EventSelect(int Selection){
	// Mini interpreter to seek out jump position
	while(csbuffer && cslength && csindex<cslength){
		if(csbuffer[csindex]==' ' ||
				csbuffer[csindex]=='\t' ||
				csbuffer[csindex]=='\r' ||
				csbuffer[csindex]=='\n'){
			// Ignore whitespace and linefeeds
			csindex++;
		}
		else if(csbuffer[csindex]=='w'){
			// Ignore voices
			SkipWord('"');
			csindex--;
		}
		else if(csbuffer[csindex]=='"'){
			// Ignore strings
			SkipWord('"');
		}
		else{
			// Extract textual command
			uString opcode=ParseWord(' ');
			SkipWord(' ');
			if(opcode=="SD"){
				int index=ParseWord(' ').to_int();
				SkipWord(' ');
				if(index==Selection){
					LogVerbose("Jumping to SD %d: %d",index,csindex);
					selection->SetVisible(false);
					state=C3_NORMAL;
					break;
				}
			}
		}
	}
}

/*! \brief Controls gameflow
 *  \return True if EventGameProcess() should be called
 */
bool EngineCrowd3::EventGameTick(){
	if(state==C3_WAITCLICK){
		bool skip=keyok || keyctrl() || GetSkipmode();
		if(skip && textview->GetRemainingText()){
			textview->CompleteText();
			keyok=false;
		}
		else if(skip){
			StopSound(VA_VOICES);
			state=C3_NORMAL;
			keyok=false;
		}
	}
	return !(state==C3_NORMAL && !textview->GetRemainingText());
}

/*! \brief Parses and interpret next scenario command
 *  \return False if processing should continue
 *
 *	This method gets called automatically by EngineVN callbacks
 */
bool EngineCrowd3::EventGameProcess(){
	bool retval=false;
	if(state==C3_NORMAL && csbuffer && cslength){
		if(csindex>=cslength){
			// Ooops!
			LogError("Script ended abruptly (%d/%d)!",csindex,cslength);
			csindex=0;
		}
		else if(csbuffer[csindex]==' ' ||
				csbuffer[csindex]=='\t' ||
				csbuffer[csindex]=='\r' ||
				csbuffer[csindex]=='\n')
		{
			// Ignore whitespace and linefeeds
			csindex++;
		}
		else if(csbuffer[csindex]=='w')
		{
			// Play voice file before printing associated text
			// Ex: w000005w"English text"
			// Ex: w000005wXXXXXXXXXXXXX
			uString sename=ParseWord('\"');
			SkipWord('"');
			csindex--;

			// Open weird crowd format by skipping the custom header
			RWops *real=new RWops();
			RWops *funny=LoadVoice(sename);
			if(funny && real->OpenRW(funny,8,funny->Size()-8,false)){
				PlayVoice(real);
			}
			delete funny;
			delete real;
		}
		else if(csbuffer[csindex]=='"')
		{
			// Print a quoted string
			// Ex: "Text goes here" 100
			saveindex=csindex;
			uString text=ParseWord('"');
			SkipWord('"');
			
			// Some neat stuff that formats the text how I want it...
			const char *casttext = text.c_str();
			char parsetext[strlen(casttext)];
			strcpy(parsetext,casttext);
			if(CleanText(parsetext)){
				if(parsetext!=NULL)
					text=parsetext;
				else
					LogDebug("Well, we dodged a segfault, but somehow ended up with a null string from our cleaning function.");
			}
			
			textview->ClearText();
			textview->PrintText(text);

			// Look for (decimal) line number
			text=ParseWord(' ');
			bool numeric=true;
			for(unsigned int i=0;numeric && i<text.length();i++){
				if(text[i].to_code()<='0' && text[i].to_code()>='9'){
					numeric=false;
				}
			}

			// Register line
			if(numeric){
				rflags.SetBit(text.to_int(),true);
				SkipWord(' ');
			}

			// Wait for user input
			state=C3_WAITCLICK;
			retval=true;
		}
		else{
			// Extract textual command
			uString opcode=ParseWord(' ');
			if(opcode=="SS"){
				// Set scene title
				// Ex: SS "Prologue"
				SkipWord(' ');
				uString name=ParseWord('"');
				LogVerbose("Set scene title: %s",name.c_str());
				SetSavename(name);
				SkipWord('"');
			}
			else if(opcode=="CF"){
				// Set Foreground (should be rendered on a layer above CB)
				// Ex: CF N00-001
				SkipWord(' ');
				uString name=ParseWord(' ');
				LogVerbose("Set background:%s",name.c_str());
				SetEventCG(name);
				SkipWord(' ');
			}
			else if(opcode=="CB"){
				// Set Background (should be rendered on a layer below CF)
				// Ex: CB H-170
				SkipWord(' ');
				uString name=ParseWord(' ');
				LogVerbose("Set background:%s",name.c_str());
				SetBackground(name);
				SkipWord(' ');
			}
			else if(opcode=="CMA"){
				// Set Character Image (Character position from 1-3)
				// Ex: CMA k_ass2001 1
				SkipWord(' ');
				uString resource=ParseWord(' ');
				SkipWord(' ');
				int pos=ParseWord(' ').to_int();
				SkipWord(' ');
				LogVerbose("Setting CG:%s (%d)",resource.c_str(),pos);
				SetForeground(resource,pos);
			}
			else if(opcode=="CFILL"){
				// Fill background with R G B (color order is assumed for now...
				// only seen black and white so its hard to determine)
				// Ex: CFill 0 0 0
				SkipWord(' ');
				int R=ParseWord(' ').to_int();
				SkipWord(' ');
				int G=ParseWord(' ').to_int();
				SkipWord(' ');
				int B=ParseWord(' ').to_int();
				SkipWord(' ');
				LogVerbose("Fill background: %d %d %d",R,G,B);
				display_bg->Fill(R,G,B,0xFF);
				display_event->Free();
				display_cg[0]->Free();
				display_cg[1]->Free();
				display_cg[2]->Free();
			}
			else if(opcode=="DFG"){
				// Delete Foreground
				// Ex: DFG
				// In Doushin, it is used to completely expose the ever-present backdrop 
				// that only changes when you swap characters.
				SkipWord(' ');
				LogVerbose("Delete Foreground");
				display_cg[0]->Free();
				display_cg[1]->Free();
				display_cg[2]->Free();
			}
			else if(opcode=="V"){
				// Visual Effects. I've cataloged a few of them...
				// Ex: V 1
				SkipWord(' ');
				int thiseffect=ParseWord(' ').to_int();
				if(thiseffect==1){
					LogVerbose("Visual Effect: Simple Blit");
				}
				else if(thiseffect==18){
					LogVerbose("Visual Effect: Linear Fade");
				}
				else if(thiseffect==21){
					LogVerbose("Visual Effect: Shake Screen");
				}
				else if(thiseffect==39){
					LogVerbose("Visual Effect: Vertical Shutters from left");
				}
				else if(thiseffect==40){
					LogVerbose("Visual Effect: Vertical Shutters from right");
				}
				else{
					LogTest("UNKNOWN: V Effect #%d",thiseffect);
				}
				SkipWord(' ');
				if(thiseffect>1){
					SetTransition();
				}
				retval=true;
			}
			else if(opcode=="EF"){
				// Visual Effects (From Doushin). I've cataloged a few of them..
				// Ex: EF 1
				SkipWord(' ');
				int thiseffect=ParseWord(' ').to_int();
				if(thiseffect==0){
					LogVerbose("Visual Effect: Simple Blit");
				}
				else if(thiseffect==1){
					LogVerbose("Visual Effect: Fade in 5x5 blocks");
				}
				else if(thiseffect==10){
					LogVerbose("Visual Effect: Linear Fade");
				}
				else if(thiseffect==30){
					LogVerbose("Visual Effect: Vertical Shutters from left");
				}
				else if(thiseffect==52){
					LogVerbose("Visual Effect: Screen Flare");
				}
				else{
					LogTest("UNKNOWN: EF Effect #%d",thiseffect);
				}
				SkipWord(' ');
				if(thiseffect>1){
					SetTransition();
				}
				retval=true;
			}
			else if(opcode=="MP"){
				// Music Play
				// Ex: MP 20
				SkipWord(' ');
				uString name=ParseWord(' ');
				SkipWord(' ');

				// Open weird crowd format by skipping the custom header
				RWops *real=new RWops();
				RWops *funny=LoadMusic(name,"eog");
				if(funny && real->OpenRW(funny,8,funny->Size()-8,false)){
					EngineMixer::PlayMusic(real);
				}
				delete funny;
				delete real;
			}
			else if(opcode=="G"){
				// Go to script index
				// Ex: G A101-21
				SkipWord(' ');
				uString idxlabel=ParseWord(' ');
				LogVerbose("Go to:%s",idxlabel.c_str());
				// Insert Script goto function (not tested yet, no idea if this works properly)
				SkipWord(' ');
				LoadCrowdScript(idxlabel);
			}
			else if(opcode=="$"){
				// Set script index label
				// Ex: $ A101-21
				SkipWord(' ');
				uString idxlabel=ParseWord(' ');
				LogVerbose("Current script label:%s",idxlabel.c_str());
				LoadCrowdScript(idxlabel);
			}
			else if(opcode=="SZ"){
				// Set alternate scenario (Doushin) labels
				// Ex: SZ B101-10 C101-10 SZS 0 0
				// Used in Doushin to tell the game what scenario to load if you switch characters.
				// The purpose of the two numbers at the end is questionable,
				// but they tend to be different for alternate versions of the same scene.
				SkipWord(' ');
				uString idxlabel=ParseWord(' ');
				while(idxlabel!="SZS"){
					LogVerbose("Alternate Scenario:%s",idxlabel.c_str());
					SkipWord(' ');
					idxlabel=ParseWord(' ');
				}
				SkipWord(' ');
				int par1=ParseWord(' ').to_int();
				SkipWord(' ');
				int par2=ParseWord(' ').to_int();
				LogVerbose("Unknown SZS Values: %d %d",par1,par2);
				// This info probably also needs to be indexed prior to running the script.
				SkipWord(' ');
			}
			else if(opcode=="ST"){
				// Set time (Day Hour Minute)
				// Ex: ST 1 12 55
				SkipWord(' ');
				int stday=ParseWord(' ').to_int();
				SkipWord(' ');
				int sthour=ParseWord(' ').to_int();
				SkipWord(' ');
				int stminute=ParseWord(' ').to_int();
				LogVerbose("Current Time: Day %d, %d:%d",stday,sthour,stminute);
				// Function to set day and time on-screen. We'll make some widgets for that later.
				SkipWord(' ');
			}
			else if(opcode=="RS"){
				// Conditional used in Doushin. 
				// May represent arousal or mental state. 
				// Negative numbers seem to indicate arousal or perhaps something more generic.
				// Ex: RS -50
				SkipWord(' ');
				uString rsstate=ParseWord(' ');
				LogTest("Variable RS:%s",rsstate.c_str());
				// Conditional test to compare current "state" to the required "state" to trigger a scenario,
				// else move on to the next scenario?
				SkipWord(' ');
			}
			else if(opcode=="SR"){
				// Set Variable (Doushin Form - disabled for now)
				// while the second is a value to set it to.
				// Ex: SR 210 2

				// Set-up Selection (X-Change 3)
				// Used to set the number of dialogue choices to be displayed.
				// Ex. SR 2 
				SkipWord(' ');
				selcount=ParseWord(' ').to_int();
				SkipWord(' ');
			}
			else if(opcode=="#"){
				// In-script label
				// Ex. # SELECT_BASE_001 
				SkipWord(' ');
				SkipWord(' ');
			}
			else if(opcode=="SP"){
				// Ex. SP "choice1" * "choice2" * SD 1 ... SD 2 ...
				Stringlist choices;
				SkipWord(' ');
				for(int cnt=0;cnt<selcount;){
					uString p=ParseWord(' ');
					if(p=="*"){
						SkipWord(' ');
						cnt++;
					}
					else if(p[0]=='\"'){
						choices.SetString(cnt,ParseWord('\"'));
						SkipWord('\"');
					}
					else{
						LogError("Unknown selection construct: {%s}",p.c_str());
						break;
					}
				}
				if(choices.GetCount()){
					ShowSelection(choices);
					state=C3_SELECTION;
					retval=true;
				}
			}
			else if(opcode=="JPZ"){
				// Jump if Zero/not set (HUGE assumption, but it seems to fit...)
				// Used to jump based on whether you've encountered a certain event.
				// Ex: JPZ 52 1 A106-25
				// If you've encountered an event that sets a variable,
				// the variable will be set to something other than 0,
				// so the jump should not happen in those cases
				// The above example is likely read as:
				// if variable 52>=1 then goto script A106-25
				// It is also possible that the logic is supposed to be:
				// if variable 52>=1 then continue; else goto script A106-25
				SkipWord(' ');
				int jpzvar=ParseWord(' ').to_int();
				SkipWord(' ');
				int jpzval=ParseWord(' ').to_int();
				SkipWord(' ');
				uString jpzidx=ParseWord(' ');
				LogTest("JPZ Jump: Variable %d Value: %d Jump Index: %s",jpzvar,jpzval,jpzidx.c_str());
				// Check the value of the listed variable
				// Perform conditional check
				// If true, jump
				SkipWord(' ');
			}
			else if(opcode=="GR"){
				// Holds a script name/index label for some unknown purpose
				// Ex: GR B304-20
				// Often shows up before conditional statements...perhaps a default
				// in case no conditionals are met.
				SkipWord(' ');
				uString gridx=ParseWord(' ');
				LogTest("GR Script Label: %s",gridx.c_str());
				// Still need to determine its purpose.
				SkipWord(' ');
			}
			else if(opcode=="MM"){
				// Stop/Mute Music
				StopMusic();
				SkipWord(' ');
			}
			else{
				// Report error
				LogError("Unknown opcode: %s",opcode.c_str());
				SkipWord(' ');
			}
		}
	}
	return retval;
}

bool EngineCrowd3::SetBackground(uString Resource){
	SDL_Surface *surface=bgcache->GetSurface(Resource);
	if(!surface){
		surface=LoadImage(Resource);
		if(surface){
			bgcache->SetSurface(Resource,surface);
		}
	}
	if(surface){
		display_bg->Blit(surface);
		display_event->Free();
		display_cg[0]->Free();
		display_cg[1]->Free();
		display_cg[2]->Free();
	}
	return surface;
}

bool EngineCrowd3::SetEventCG(uString Resource){
	SDL_Surface *surface=bgcache->GetSurface(Resource);
	if(!surface){
		surface=LoadImage(Resource);
		if(surface){
			bgcache->SetSurface(Resource,surface);
		}
	}
	if(surface){
		display_event->Blit(surface);
		display_cg[0]->Free();
		display_cg[1]->Free();
		display_cg[2]->Free();
	}
	return surface;
}

bool EngineCrowd3::SetForeground(uString Resource,int Index){
	SDL_Surface *surface=fgcache->GetSurface(Resource);
	if(!surface){
		// Open a crowd-style masked image
		SDL_Surface *sfull=LoadImage(Resource);
		if(sfull && Index<3){
			SDL_Rect rimage={0,0,sfull->w/2,sfull->h};
			SDL_Rect rmask={rimage.w,0,rimage.w,rimage.h};
			SDL_Surface *smask=EDL_CopySurface(sfull,&rmask);
			surface=EDL_MaskSurface(sfull,smask,&rimage);
			SDL_FreeSurface(sfull);
			SDL_FreeSurface(smask);
			fgcache->SetSurface(Resource,surface);
		}
	}
	if(surface){
		display_cg[Index]->Free();
		display_cg[Index]->Blit(surface);
	}
	return surface;
}


