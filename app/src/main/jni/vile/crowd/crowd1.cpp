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

#include "crowd1.h"

EngineCrowd1::EngineCrowd1(int Width,int Height) : EngineVN(Width,Height) {
	// Set defaults
	state=C1_GUI;
	csbuffer=0;
	cslength=0;
	csindex=0;
	seldata=0;
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

EngineCrowd1::~EngineCrowd1(){
	if(seldata){
		delete seldata;
	}
	if(csbuffer){
		delete [] csbuffer;
	}
	delete bgcache;
	delete fgcache;
}

/*! \brief Parses input text string, formats those with names and strips undesired meta-characters
 *	\param input pointer to input string
 *	\return True if altered by this function.
 *	Any parser characters found before anything else will be ignored.
 *	I'll at least change the @? case to handle italics later.
 *	Technically, this function should always return true for now...I'll fix that issue later.
 */
bool EngineCrowd1::CleanText(char *input){
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
bool EngineCrowd1::ShowSelection(Stringlist List){
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
bool EngineCrowd1::LoadCrowdScript(uString Script){
	bool retval=false;
	RWops *ops=0;
	if((ops=LoadScript(Script))){
		// Extract and verify data
		LogTest("Loading Crowd script: %s",Script.c_str());
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
				state=C1_NORMAL;
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

bool EngineCrowd1::EventLoad(int Index){
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

bool EngineCrowd1::EventSave(int Index){
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
void EngineCrowd1::EventSelect(int Selection){
	// Get subselection
	int subsel=1;
	if(seldata){
		subsel=seldata->GetSint8(Selection,0);
		if(subsel<seldata->GetSint8(Selection,1)){
			seldata->SetSint8(Selection,0,subsel+1);
		}
	}

	// Mini interpreter to seek out jump position
	csindex=selindex;
	while(csbuffer && cslength){
		// Read line
		uString line;
		for(;csindex<cslength;csindex++){
			if(csbuffer[csindex]=='\r' || csbuffer[csindex]=='\n'){
				break;
			}
			else{
				line+=csbuffer[csindex];
			}
		}
		while(csindex<cslength){
			if(csbuffer[csindex]=='\r' || csbuffer[csindex]=='\n'){
				csindex++;
			}
			else{
				break;
			}
		}

		// Interpret command
		uString cmd=EDL_Format("QA%d-%d",Selection+1,subsel);
		while(line.length() && line[0]==' '){
			line=line.substr(1);
		}

		if(line.length()){
			if(!strcmp(line.c_str(),cmd.c_str())){
				selection->SetVisible(false);
				state=C1_NORMAL;
				break;
			}
			if(!strncmp(line.c_str(),"QA",2)){
				line=line.substr(2);
				int p1=ParseWord(line,'-',0).to_int();
				int p2=ParseWord(line,'-',1).to_int();
				if(p1==(Selection+1)){
					selection->SetVisible(false);
					state=C1_NORMAL;
					break;
				}
			}
		}
	}
}

uString EngineCrowd1::ParseWord(uString Line,char Parser,int Index){
	// Read line
	uString retval;
	bool hit=(Index==0);
	int index=0;
	for(unsigned int i=0;i<Line.length();i++){
		if(Line[i]==Parser){
			if(hit){
				break;
			}
			hit=(++index==Index);
		}
		else if(hit){
			retval+=Line[i];
		}
	}
	return retval;
}

/*! \brief Controls gameflow
 *  \return True if EventGameProcess() should be called
 */
bool EngineCrowd1::EventGameTick(){
	if(state==C1_WAITCLICK){
		bool skip=keyok || keyctrl() || GetSkipmode();
		if(skip && textview->GetRemainingText()){
			textview->CompleteText();
			keyok=false;
		}
		else if(skip){
			StopSound(VA_VOICES);
			textview->ClearText();
			state=C1_NORMAL;
			keyok=false;
		}
	}
	return !(state==C1_NORMAL && !textview->GetRemainingText());
}

/*! \brief Parses and interpret next scenario command
 *  \return False if processing should continue
 *
 *	This method gets called automatically by EngineVN callbacks
 */
bool EngineCrowd1::EventGameProcess(){
	bool retval=false;
	if(state==C1_NORMAL && csbuffer && cslength){
		// Read line
		uString line;
		for(;csindex<cslength;csindex++){
			if(csbuffer[csindex]=='\r' || csbuffer[csindex]=='\n'){
				break;
			}
			else{
				line+=csbuffer[csindex];
			}
		}
		while(csindex<cslength){
			if(csbuffer[csindex]=='\r' || csbuffer[csindex]=='\n'){
				csindex++;
			}
			else{
				break;
			}
		}

		// Interpret command
		while(line.length() && line[0]==' '){
			line=line.substr(1);
		}
		if(line.length()){
			//LogVerbose("Parsing: %s",line.c_str());
			if(line[0]=='$' || line[0]=='G'){
				// Goto script
				line=line.substr(1);
				if(line!=csname){
					LoadCrowdScript(line);
				}
			}
			else if(line[0]=='\"' || line[0]==','){
				// Print text
				line=line.substr(1);
				textview->PrintText(line);
				state=C1_WAITCLICK;
				retval=true;
			}
			else if(line[0]=='V'){
				// Voiced text
				uString voice=ParseWord(line,'!',0);
				line=ParseWord(line,'!',1);
				uString name=ParseWord(line,'\"',0);
				uString text=ParseWord(line,'\"',1);
				voice[0]='w';
				PlayVoice(voice);
				textview->ClearText();
				textview->PrintText(name,text);
				state=C1_WAITCLICK;
				retval=true;
			}
			else if(line[0]=='!'){
				// Print captioned text
				line=line.substr(1);
				uString caption=ParseWord(line,'\"',0);
				uString text=ParseWord(line,'\"',1);
				textview->ClearText();
				textview->PrintText(caption,text);
				state=C1_WAITCLICK;
				retval=true;
			}
			else if(line[0]=='['){
				// Name of character
				line=line.substr(1);
				textview->ClearText();
				textview->PrintText(line);
				textview->PrintNewline();
			}
			else if(line[0]=='('){
				// Comment
			}
			else if(line[0]=='L'){
				// Set game information
				line=line.substr(1);
				uString time=ParseWord(line,',',0);
				uString location=ParseWord(line,',',1);
				uString action=ParseWord(line,',',2);
			}
			else if(line[0]=='T'){
				// Go to title screen
				//EventGameDialog(VD_TITLE);
				title->SetVisible(true);
				state=C1_GUI;
				retval=true;
			}
			else if(!strncmp(line.c_str(),"CB",2)){
				// Black background?
				SetBackground("BLACK");
			}
			else if(!strncmp(line.c_str(),"CF",2)){
				// Bg graphics
				if(ParseWord(line,',',1).length()){
					// Xchange 1
					line=line.substr(2);
					int index=line.to_int();
					line=ParseWord(line,',',1);
					SetBackground(line);
				}
				else{
					// Tokimeki
					line=line.substr(2);
					SetBackground(line);
				}
			}
			else if(!strncmp(line.c_str(),"CM",2)){
				// Character graphics
				line=line.substr(2);
				int p1=ParseWord(line,',',0).to_int();
				int p2=ParseWord(line,',',1).to_int();
				int p3=ParseWord(line,',',2).to_int();
				int p4=ParseWord(line,',',3).to_int();
				uString p5=ParseWord(line,',',4);
				LogTest("Foreground: %d %d %d %d %s",p1,p2,p3,p4,p5.c_str());
				SetForeground(p5,p4,p1,p2);
			}
			else if(!strncmp(line.c_str(),"CC",2)){
				// Clear character graphics?
				line=line.substr(2);
				int p1=ParseWord(line,',',0).to_int();
				int p2=ParseWord(line,',',1).to_int();
				display_cg[p1]->Free();
			}
			else if(!strncmp(line.c_str(),"EF",2)){
				// Graphic effect
				SetTransition();
				retval=true;
			}
			else if(!strncmp(line.c_str(),"MP",2)){
				// Play music
				line=line.substr(2);
				int index=line.to_int();
				line=line.substr(2);
				PlayTrack(index);
			}
			else if(!strncmp(line.c_str(),"MS",2)){
				// Stop music
				StopMusic();
			}
			else if(!strncmp(line.c_str(),"MW",2)){
				// Pause music?
				PauseMusic();
			}
			else if(!strncmp(line.c_str(),"QS",2)){
				// Configure selection box
				line=line.substr(2);
				if(seldata){
					delete seldata;
					seldata=0;
				}
				int n=ParseWord(line,',',0).to_int();
				if(n>0){
					seldata=new DArray(n,2);
					for(int i=0;i<n;i++){
						int v=ParseWord(line,',',i+1).to_int();
						seldata->SetSint8(i,0,1);
						seldata->SetSint8(i,1,v);
					}
				}
			}
			else if(!strncmp(line.c_str(),"QP",2)){
				// Show selection box
				line=line.substr(2);
				Stringlist commands;
				for(int i=0;true;i++){
					uString cmd=ParseWord(line,',',i);
					if(cmd.length()){
						commands.AddString(cmd);
					}
					else{
						break;
					}
				}
				if(commands.GetCount()){
					selection->Clear();
					selection->SetText(&commands);
					selection->SetVisible(true);
					selindex=csindex;
					state=C1_SELECTION;
				}
			}
			else if(!strncmp(line.c_str(),"QE",2)){
				// Redisplay selection
				selection->SetVisible(true);
				state=C1_SELECTION;
			}
			else if(!strncmp(line.c_str(),"QX",2)){
				// Conditional jump
				line=line.substr(2);
				bool locked=true;
				for(int i=1;true;i++){
					uString cmd=ParseWord(line,',',i);
					if(cmd.length()){
						int v=cmd.to_int();
						if(v>seldata->GetSint8(i-1,0)){
							break;
						}
					}
					else{
						locked=false;
						break;
					}
				}
				if(locked){
LogError("QX:FAIL");
					selection->SetVisible(true);
					state=C1_SELECTION;
				}
				else{
LogError("QX:OK");
					LoadCrowdScript(ParseWord(line,',',0));
				}
			}

			/*
			else if(!strncmp(line.c_str(),"QA",2)){
				// Selection box target
			}
			*/
			else if(line=="{"){
				// Start of compound
			}
			else if(line=="}"){
				// End of compound
				retval=true;
			}
			else{
				LogError("Unrecognized command: %s",line.c_str());
			}
		}
	}
	return retval;
}

bool EngineCrowd1::SetBackground(uString Resource){
	SDL_Surface *surface=bgcache->GetSurface(Resource);
	if(!surface){
		surface=LoadImage(Resource);
		if(!surface && !strncasecmp(Resource.c_str(),"BLACK",5)){
			surface=EDL_CreateSurface(NativeWidth(),NativeHeight());
			EDL_SetBox(surface,0,0,surface->w,surface->h,0,0,0,0xFF);
		}
		if(!surface && !strncasecmp(Resource.c_str(),"WHITE",5)){
			surface=EDL_CreateSurface(NativeWidth(),NativeHeight());
			EDL_SetBox(surface,0,0,surface->w,surface->h,0xFF,0xFF,0xFF,0xFF);
		}
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

bool EngineCrowd1::SetEventCG(uString Resource){
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

bool EngineCrowd1::SetForeground(uString Resource,int Index,int X,int Y){
	SDL_Surface *surface=fgcache->GetSurface(Resource);
	if(!surface){
		surface=LoadImage(Resource);
		fgcache->SetSurface(Resource,surface);
	}
	if(surface){
		display_cg[Index]->Free();
		display_cg[Index]->Move(X,NativeHeight()-surface->h);
		display_cg[Index]->Resize(surface->w,surface->h);
		display_cg[Index]->Blit(surface);
	}
	return surface;
}


