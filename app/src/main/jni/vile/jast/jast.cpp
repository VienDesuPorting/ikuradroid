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

#include "jast.h"


EngineJAST::EngineJAST(int Width,int Height) : EngineVN(Width,Height){
	// Set defaults
	scriptbuffer=0;
	scriptlength=0;
	scriptindex=0;
	choices=0;
	labels=0;
	filenames=0;
	s_background=0;
	state=JASTSTATE_NORMAL;
	choicemain=choicesub=0;
	EDL_SETRECT(r_display,80,16,480,296);

	// Set fixed black background
	Widget *bg=new Widget();
	bg->Resize(640,480);
	bg->Fill(0,0,0,0xFF);
	AddWidget(bg,VL_BACKGROUND);

	// Prepp widgets
	w_display=new Widget();
	w_display->Move(0,40);
	w_display->Resize(640,400);
	AddWidget(w_display,VL_BACKGROUND);
	w_tv=new Textview(this);
	w_tv->SetTextPosition(0,0,500,62);
	w_tv->MoveDialog(70,368);
	w_tv->Resize(500,66);
	AddWidget(w_tv,VL_TEXTVIEW);
	w_sel=0;
}

EngineJAST::~EngineJAST(){
	ClearFilenames();
	ClearChoice();
	ClearLabel();
	if(s_background){
		SDL_FreeSurface(s_background);
	}
}

/*! \brief Counts parameters in a line (Including command)
 *  \param Line Script line to scan
 *  \return Number of parameters including command
 */
int EngineJAST::GetParamCount(uString Line){
	int retval=0;
	if(Line.length()){
		retval++;
		for(unsigned int i=0;i<Line.length();i++){
			if(Line[i]=='\"' || Line[i]=='\''){
				uString breaker=Line[i++];
				while(i<Line.length() &&
						Line[i]!=breaker &&
						Line[i+1]!='\r' &&
						Line[i+1]!='\n'){
					i++;
				}
			}
			else if(Line[i]==',' || Line[i]==' '){
				retval++;
			}
		}
	}
	return retval;
}

/*! \brief Extracts single parameter from script line (0=Command)
 *  \param Line Script line to scan
 *  \param Index Parameter to extract (0=Command)
 *  \return Extracted command or parameter
 */
uString EngineJAST::GetParam(uString Line,int Index){
	uString retval;
	if(Line.length()){
		if(!Index){
			// Get command only
			for(unsigned int i=0;i<Line.length() && Line[i]!=' ';i++){
				retval+=Line[i];
			}
		}
		else{
			// Skip unwanted parameters
			unsigned int i=0;
			for(int j=0;j<Index;j++){
				if(Line[i]=='\"' || Line[i]=='\''){
					uString breaker=Line[i++];
					while(i<Line.length() &&
							Line[i]!=breaker &&
							Line[i+1]!='\r' &&
							Line[i+1]!='\n'){
						i++;
					}
				}

				while(i<Line.length() && Line[i]!=',' && Line[i]!=' '){
					i++;
				}
				while(i<Line.length() && (Line[i]==',' || Line[i]==' ')){
					i++;
				}
			}

			// Get parameter
			for(;i<Line.length() && Line[i]!=',' && Line[i]!=' ';i++){
				if(Line[i]=='\"' || Line[i]=='\''){
					uString breaker=Line[i++];
					retval+='\"';
					while(i<Line.length() &&
							Line[i]!=breaker &&
							Line[i+1]!='\r' &&
							Line[i+1]!='\n'){
						retval+=Line[i++];
					}
					retval+='\"';
				}
				else{
					retval+=Line[i];
				}
			}
		}

	}
	return retval;
}

/*! \brief Seeks the script to a labeled line and retrieves first line of code
 *  \param Label Label to seek to
 *  \return First line of code after the label (Empty if failed)
 */
bool EngineJAST::GetLine(uString Label){
	LABEL_TYPE *tmpptr=labels;
	uString label=EDL_Lower(Label);
	while(tmpptr && tmpptr->caption!=label){
		tmpptr=tmpptr->next;
	}
	if(tmpptr){
		scriptlabel=Label;
		scriptindex=tmpptr->index;
	}
	return tmpptr;
}

/*! \brief Extracts and trims the current script line (if any!)
 *  \return Line of code (Blank if failed)
 */
uString EngineJAST::GetCurrentLine(){
	uString retval;
	if(scriptbuffer && scriptindex && scriptlength){
		// Skip to the real meat
		while(scriptindex<scriptlength &&
				(scriptbuffer[scriptindex]=='\t' ||
				scriptbuffer[scriptindex]==' ' ||
				scriptbuffer[scriptindex]=='\r' ||
				scriptbuffer[scriptindex]=='\n')){
			scriptindex++;
		}

		// Skip commented lines
		if(scriptbuffer[scriptindex]==';'){
			return GetNextLine();
		}

		// Add command
		int i=scriptindex;
		while(i<scriptlength &&
				(scriptbuffer[i]==' ' &&
				 scriptbuffer[i]=='\t'));
		for(;i<scriptlength &&
				scriptbuffer[i]!='\r' &&
				scriptbuffer[i]!='\n';i++){
			if(scriptbuffer[i]==';'){
				// Drop comments
				//LogTest("LINE:%s",retval.c_str());
				return retval;
			}
			if(scriptbuffer[i]=='\"'){
				// Copy quoted text
				i++;
				aString tmp="\"";
				while(i<scriptlength &&
						scriptbuffer[i]!='\"' &&
						scriptbuffer[i]!='\n' &&
						scriptbuffer[i]!='\r'){
					tmp+=scriptbuffer[i++];
				}
				tmp+="\"";
				retval+=tmp;
			}
			else if(scriptbuffer[i]==' ' && scriptbuffer[i-1]==','){
				// Ignore trailing spaces
			}
			else if(scriptbuffer[i]=='\t' && scriptbuffer[i-1]==','){
				// Ignore trailing spaces
			}
			else if((scriptbuffer[i]==' ' || scriptbuffer[i]=='\t') &&
					(scriptbuffer[i+1]==' ' || scriptbuffer[i+1]=='\t')){
				// Ignore double spaces
			}
			else if(scriptbuffer[i]=='\t'){
				retval+=" ";
			}
			else if(scriptbuffer[i]!='\n' &&
					scriptbuffer[i]!='\r'){
				// Accumelate trimmed text
				retval+=(char)tolower(scriptbuffer[i]);
			}
		}
	}
	//LogTest("LINE:%s",retval.c_str());
	return retval;
}

/*! \brief Extracts and trims the next script line (if any!)
 *  \return Line of code (Blank if failed)
 */
uString EngineJAST::GetNextLine(){
	uString retval;
	if(scriptbuffer && scriptindex && scriptlength){
		while(scriptindex<scriptlength &&
				scriptbuffer[scriptindex]!='\r' &&
				scriptbuffer[scriptindex]!='\n'){
			scriptindex++;
		};
		retval=GetCurrentLine();
	}
	return retval;
}

/*! \brief Registers a position and associates it with a textual label
 *  \param Caption Textual label (Typicall M1, M2, etc)
 *  \param Index Position in the script for this label
 */
void EngineJAST::AddFilename(uString Name,uString Type){
	FILENAME_TYPE *newptr=new FILENAME_TYPE;
	newptr->name=EDL_Trim(EDL_Upper(Name),true,true);
	newptr->type=EDL_Trim(EDL_Upper(Type),true,true);
	newptr->next=0;
	if(filenames){
		FILENAME_TYPE *tmp=filenames;
		while(tmp->next){
			tmp=tmp->next;
		}
		tmp->next=newptr;
	}
	else{
		filenames=newptr;
	}
}

/*! \brief Flushes any buffered labels and positions
 *
 *  This method should be called whenever you are loading a new script
 */
void EngineJAST::ClearFilenames(){
	while(filenames){
		FILENAME_TYPE *tmp=filenames->next;
		delete filenames;
		filenames=tmp;
	}
}

/*! \brief Registers a position and associates it with a textual label
 *  \param Caption Textual label (Typicall M1, M2, etc)
 *  \param Index Position in the script for this label
 */
void EngineJAST::AddLabel(uString Caption,int Index){
	LABEL_TYPE *newptr=new LABEL_TYPE;
	newptr->caption=EDL_Lower(Caption);
	newptr->index=Index;
	newptr->next=0;
	if(labels){
		LABEL_TYPE *tmp=labels;
		while(tmp->next){
			tmp=tmp->next;
		}
		tmp->next=newptr;
	}
	else{
		labels=newptr;
	}
}

/*! \brief Flushes any buffered labels and positions
 *
 *  This method should be called whenever you are loading a new script
 */
void EngineJAST::ClearLabel(){
	while(labels){
		LABEL_TYPE *tmp=labels->next;
		delete labels;
		labels=tmp;
	}
}

/*! \brief Adds a main menu item
 *  \param ID Choice label in script (Typically VV1, VV2, etc)
 */
void EngineJAST::AddChoice(uString ID){
	CHOICE_TYPE *checkptr=choices;
	while(checkptr && checkptr->id!=ID){
		checkptr=checkptr->next;
	}
	if(!checkptr && GetLine(ID)){
		uString vv=GetCurrentLine();
		CHOICE_TYPE *newptr=new CHOICE_TYPE;
		newptr->index=GetParam(vv,1).to_int();
		newptr->caption=EDL_Trim(EDL_Upper(GetParam(vv,2)),true,false);
		newptr->id=ID;
		newptr->visible=true;
		newptr->sub=0;
		newptr->next=0;
		if(choices){
			CHOICE_TYPE *tmp=choices;
			while(tmp->next){
				tmp=tmp->next;
			}
			tmp->next=newptr;
		}
		else{
			choices=newptr;
		}
	}
}

/*! \brief Adds a submenu item
 *  \param ID Choice label in script (Typically VV1, VV2, etc)
 *  \param SubID Choice sub label in script (Typically NN1, NN2, etc)
 */
void EngineJAST::AddChoice(uString ID,uString SubID){
	// Get main menu object
	CHOICE_TYPE *vvptr=choices;
	while(vvptr && vvptr->id!=ID){
		vvptr=vvptr->next;
	}

	// Attach a sub menu
	if(vvptr){
		CHOICE_TYPE *checkptr=vvptr->sub;
		while(checkptr && checkptr->id!=ID){
			checkptr=checkptr->next;
		}
		if(!checkptr && GetLine(SubID)){
			uString nn=GetCurrentLine();
			CHOICE_TYPE *newptr=new CHOICE_TYPE;
			newptr->index=GetParam(nn,1).to_int();
			newptr->caption=EDL_Trim(EDL_Upper(GetParam(nn,2)),true,false);
			newptr->id=SubID;
			newptr->visible=true;
			newptr->sub=0;
			newptr->next=0;
			if(vvptr->sub){
				CHOICE_TYPE *tmp=vvptr->sub;
				while(tmp->next){
					tmp=tmp->next;
				}
				tmp->next=newptr;
			}
			else{
				vvptr->sub=newptr;
			}
		}
		else{
			LogError("Invalid SubID:%s",SubID.c_str());
		}
	}
	else{
		LogError("Could not find ID:%s",ID.c_str());
	}
}

void EngineJAST::ClearChoice(){
	CHOICE_TYPE *tmp;
	while(choices){
		while(choices->sub){
			tmp=choices->sub->next;
			delete choices->sub;
			choices->sub=tmp;
		}
		tmp=choices->next;
		delete choices;
		choices=tmp;
	}
}

bool EngineJAST::LoadJASTScript(uString Script){
	bool retval=false;
	RWops *blob=0;
	uString sname=EDL_Searchname(Script);
	if((blob=LoadScript(Script))){
		// Extract and verify data
		LogVerbose("Loading JAST script: %s",Script.c_str());
		int tlength=blob->Seek(0,SEEK_END);
		if(tlength>(int)0){
			// Replace blob
			blob->Seek(0,SEEK_SET);
			Uint8 *tbuffer=new Uint8[tlength];
			if(blob->Read(tbuffer,tlength)>(int)0){
				// Apply game patches
				if(Script=="SK_102.OVL"){
					LogVerbose("Applying patch:%c",tbuffer[1120]);
					tbuffer[1120]='6';
				}

				// Store buffer
				if(scriptbuffer){
					delete [] scriptbuffer;
				}
				scriptbuffer=tbuffer;
				scriptlength=tlength;
				scriptindex=0;

				// Unset variables
				for(int i=0;i<256;i++){
					vars.SetUint8(i,0);
				}

				// Rig down old stuff
				if(w_sel){
					DestroyWidget(w_sel);
					w_sel=0;
				}
				w_tv->ClearText();
				ClearChoice();
				ClearLabel();
				ClearFilenames();

				// Parse labels
				int linestart=0;
				for(int i=0;i<tlength;i++){
					if(tbuffer[i]=='\n' || tbuffer[i]=='\r'){
						// Skip trailing control characters
						for(;i+1<tlength &&
								(tbuffer[i]=='\t' ||
								tbuffer[i]==' ' ||
								tbuffer[i]=='\r' ||
								tbuffer[i]=='\n');i++);

						// Register start of line
						linestart=i;
					}
					else if(tbuffer[i]==':' && i>linestart && i-20<linestart){
						// Register label
						uString label;
						for(int j=linestart;j<i;j++){
							label+=(char)tbuffer[j];
						}
						if(label.length()){
							AddLabel(label,i+1);
						}
					}
					else if(tbuffer[i]=='\"'){
						// Seek to end of quote (Watch for broken quotes!)
						i++;
						for(;i+1<tlength &&
								tbuffer[i]!='\"' &&
								tbuffer[i+1]!='\r' &&
								tbuffer[i+1]!='\n';i++);
					}
					else if(tbuffer[i]==';'){
						// Seek to end of line
						for(;i+1<tlength &&
								tbuffer[i+1]!='\r' &&
								tbuffer[i+1]!='\n';i++);
					}
				}

				// Rebuild choices
				int tablecount=0;
				uString table;
				if(GetLine("SCTBL")){
					table=GetCurrentLine();
				}
				for(int j=0;table.length();j++){
					// Parse commaseparated labels
					tablecount=GetParamCount(table);
					for(int i=1;i<tablecount;i++){
						if(GetLine(GetParam(table,i))){
							uString sx=GetCurrentLine();
							uString vvid=GetParam(sx,1);
							AddChoice(vvid);
							uString nnid=GetParam(sx,2);
							if(nnid!="-1"){
								AddChoice(vvid,nnid);
							}
						}
					}

					// Retrace table until label
					GetLine("SCTBL");
					for(int k=0;k<=j;k++){
						table=GetNextLine();
					}
					table=GetNextLine();
					for(unsigned int k=0;k<table.length();k++){
						if(table[k]==':'){
							table="";
						}
					}
				}

				// Rearrange choices
				CHOICE_TYPE *newtopchoices=0;
				tablecount=0;
				table="";
				if(GetLine("COMTBL")){
					table=GetCurrentLine();
				}
				tablecount=GetParamCount(table);
				for(int i=1;i<tablecount;i++){
					if(GetLine(GetParam(table,i))){
						// Find item and remove it from the stack
						uString sx=GetCurrentLine();
						uString vvid=GetParam(sx,1);
						CHOICE_TYPE *lastchoice=0;
						CHOICE_TYPE *topchoice=choices;
						while(topchoice && topchoice->id!=vvid){
							lastchoice=topchoice;
							topchoice=topchoice->next;
						}
						if(topchoice && lastchoice){
							lastchoice->next=topchoice->next;
						}
						if(topchoice && !lastchoice){
							choices=choices->next;
						}


						// Add to new stack
						if(newtopchoices){
							CHOICE_TYPE *tmpchoice=newtopchoices;
							while(tmpchoice && tmpchoice->next){
								tmpchoice=tmpchoice->next;
							}
							tmpchoice->next=topchoice;
							topchoice->next=0;
						}
						else{
							newtopchoices=topchoice;
							topchoice->next=0;
						}

						// Rearrange sublevel choice in the same manner
						if(topchoice){
							CHOICE_TYPE *newsubchoices=0;
							CHOICE_TYPE *subchoice=0;
							for(int index=2;true;index++){
								uString nnid=GetParam(sx,index);
								if(nnid=="-1" || nnid==""){
									break;
								}
								lastchoice=0;
								subchoice=topchoice->sub;
								while(subchoice && subchoice->id!=nnid){
									lastchoice=subchoice;
									subchoice=subchoice->next;
								}
								if(subchoice && lastchoice){
									lastchoice->next=subchoice->next;
								}
								if(subchoice && !lastchoice){
									topchoice->sub=topchoice->sub->next;
								}
								if(newsubchoices){
									CHOICE_TYPE *tmpchoice=newsubchoices;
									while(tmpchoice->next){
										tmpchoice=tmpchoice->next;
									}
									tmpchoice->next=subchoice;
									subchoice->next=0;
								}
								else{
									newsubchoices=subchoice;
									subchoice->next=0;
								}
							}

							// Drop any remaining subchoices
							while(topchoice->sub){
								LogError("Cannot arrange choice: %s",
										topchoice->sub->caption.c_str());
								CHOICE_TYPE *tmpchoice=topchoice->sub->next;
								delete topchoice->sub;
								topchoice->sub=tmpchoice;
							}
							topchoice->sub=newsubchoices;
						}
						else{
							LogError("Could not find topchoice: %s",
									vvid.c_str());
						}
					}
				}
				while(choices){
					LogError("Cannot arrange choice: %s",
							choices->caption.c_str());
					CHOICE_TYPE *tmpchoice=choices->next;
					delete choices;
					choices=tmpchoice;
				}
				choices=newtopchoices;


				// Parse file resources
				if(GetLine("FILETBL")){
					table=GetCurrentLine();
					tablecount=GetParamCount(table);
				}
				for(int i=1;i<tablecount;i++){
					if(GetLine(GetParam(table,i))){
						uString fnx=GetCurrentLine();
						uString ftype=GetParam(fnx,2);
						uString fname=GetParam(fnx,3);
						AddFilename(fname,ftype);
					}
				}


				// Autoload first resource as background
				if(filenames){
					SDL_Surface *tmp=LoadImage(filenames->name);
					if(tmp){
						if(filenames->type=="P"){
							LogDebug("Autoloading background: %s",
									filenames->name.c_str());
							SetBackground(tmp);
							SDL_FreeSurface(tmp);
						}
						else if(filenames->type=="N"){
							LogDebug("Autosetting background: %s",
									filenames->name.c_str());
							if(s_background){
								SDL_FreeSurface(s_background);
							}
							s_background=tmp;
							SetBackground(tmp);
						}
						else{
							LogError("Invalid autoload resource: %s",
									filenames->type.c_str());
							SDL_FreeSurface(tmp);
						}
					}
				}

				// Get startindex
				if(GetLine("MSGTBL")){
					table=GetCurrentLine();
					GetLine(GetParam(table,1));
					state=JASTSTATE_NORMAL;
				}

				// Store name
				scriptname=Script;
			}
			else{
				delete [] tbuffer;
			}
		}
	}
	else{
		LogError("Invalid script: %s",Script.c_str());
	}
	return retval;
}

/*! \brief Tags a main menu item as visible/hidden
 *  \param Index Index of menu item
 *  \param Visible Wheter to hide or show the item
 *  \param True if the item was found and configured
 */
bool EngineJAST::ShowMain(int Index,bool Visible){
	CHOICE_TYPE *vvptr=choices;
	for(int i=1;vvptr && i<Index;i++){
		vvptr=vvptr->next;
	}
	if(vvptr){
		vvptr->visible=Visible;
	}
	return vvptr;
}

/*! \brief Tags a main menu item as visible/hidden
 *  \param Index Index of parent menu item
 *  \param SubIndex Index of menu item
 *  \param Visible Wheter to hide or show the item
 *  \param True if the item was found and configured
 */
bool EngineJAST::ShowSub(int Index,int SubIndex,bool Visible){
	bool retval=false;
	CHOICE_TYPE *vvptr=choices;
	for(int i=1;vvptr && i<Index;i++){
		vvptr=vvptr->next;
	}
	if(vvptr){
		CHOICE_TYPE *nnptr=vvptr->sub;
		for(int i=1;nnptr && i<SubIndex;i++){
			nnptr=nnptr->next;
		}
		if(nnptr){
/*
if(Visible){
LogTest("SHOW:%d.%d:%s",Index,SubIndex,nnptr->caption.c_str());
}
else{
LogTest("HIDE:%d.%d:%s",Index,SubIndex,nnptr->caption.c_str());
}
*/

			nnptr->visible=Visible;
			retval=true;
		}
	}
	return retval;
}

/*! \brief Show choice/command selection
 *  \param Index Index of parent menu item (0 to show main menu)
 *  \return True if choices were successfully shown
 */
bool EngineJAST::ShowSelection(int Index){
	bool retval=false;
	SDL_Rect rect=w_tv->GetPosition();
	rect.w=rect.w/3;
	rect.h=rect.h/3;
	Uint32 black=0x000000FF;
	Uint32 white=0xFFFFFFFF;
	w_sel=new Selection(this);
	w_sel->Move(rect);
	w_sel->Resize(500,66);
	w_sel->SetFontSize(18);
	w_sel->SetColors(white,black,black,white);
	AddWidget(w_sel,VL_CHOICES);
	if(Index<1){
		// Show main menu items
		CHOICE_TYPE *vvptr=choices;
		while(vvptr){
			if(vvptr->visible){
				retval=true;
				w_sel->SetText(Unquote(vvptr->caption),rect,vvptr->index);
				rect.x+=rect.w;
				if(rect.x+rect.w>=w_tv->GetX()+w_tv->GetWidth()){
					rect.x=w_tv->GetX();
					rect.y=rect.y+rect.h;
				}
			}
			vvptr=vvptr->next;
		}
		w_sel->SetFocusItem(choicemain);
	}
	else{
		// Show submenu items
		CHOICE_TYPE *vvptr=choices;
		while(vvptr){
			if(vvptr->index==Index){
				CHOICE_TYPE *nnptr=vvptr->sub;
				if(!vvptr->visible){
					nnptr=0;
				}
				while(nnptr){
					if(nnptr->visible){
						retval=true;
						uString c=Unquote(nnptr->caption);
						w_sel->SetText(c,rect,(nnptr->index<<8)|vvptr->index);
						rect.x+=rect.w;
						if(rect.x+rect.w>=w_tv->GetX()+w_tv->GetWidth()){
							rect.x=w_tv->GetX();
							rect.y=rect.y+rect.h;
						}
					}
					nnptr=nnptr->next;
				}
			}
			vvptr=vvptr->next;
		}
		w_sel->SetFocusItem(choicesub);
	}
	w_tv->ClearText();
	state=JASTSTATE_SELECT;
	return retval;
}

void EngineJAST::EventSelect(int Selection){
	// Get menu name in order to identify storyline
	uString mname;
	uString sname;
	if(Selection>0xFF){
		// Submenu
		choicesub=Selection;
		mname=EDL_Format("vv%d",Selection&0xFF);
		sname=EDL_Format("nn%d",Selection>>8);
	}
	else{
		// Main menu
		choicemain=Selection;
		mname=EDL_Format("vv%d",Selection);
		sname="-1";
	}

	// Get main menu object
	CHOICE_TYPE *object=choices;
	while(object && object->id!=mname){
		object=object->next;
	}

	// Get sub menu object
	if(object && sname!="-1"){
		object=object->sub;
		while(object && object->id!=sname){
			object=object->next;
		}
	}

	// Assert a proper menu object
	if(!object){
		LogError("Invalid menu selection: %d",Selection);
		return;
	}

	// Scan storylines for named items
	uString msg;
	int tablecount=0;
	uString table;
	if(GetLine("SCTBL")){
		table=GetCurrentLine();
	}
	for(int j=0;!msg.length() && table.length();j++){
		// Parse commaseparated labels
		tablecount=GetParamCount(table);
		for(int i=1;!msg.length() && i<tablecount;i++){
			if(GetLine(GetParam(table,i))){
				uString sx=GetCurrentLine();
				uString vvid=GetParam(sx,1);
				if(vvid==mname){
					uString nnid=GetParam(sx,2);
					if(nnid==sname){
						int scount=GetParamCount(sx);
						int sindex=GetParam(sx,3).to_int();
						int smsg=EDL_LIMIT(vars.GetUint8(sindex)+4,4,scount);
						msg=GetParam(sx,smsg);
					}
				}
			}
		}

		// Retrace table until label
		if(!msg.length()){
			GetLine("SCTBL");
			for(int k=0;k<=j;k++){
				table=GetNextLine();
			}
			table=GetNextLine();
			for(unsigned int k=0;k<table.length();k++){
				if(table[k]==':'){
					table="";
				}
			}
		}
	}

	// Check result
	if(!msg.length() && Selection<0xFF){
		// Show sub menu items
		DestroyWidget(w_sel);
		w_sel=0;
		ShowSelection(Selection);
	}
	else if(msg.length() && GetLine(msg)){
		// Execute selected block
		state=JASTSTATE_NORMAL;
		DestroyWidget(w_sel);
		w_sel=0;
	}
	else{
		// Oooops
		LogError("Invalid message label: %s",msg.c_str());
	}
}

bool EngineJAST::SetCharacter(SDL_Surface *Resource,int X){
	if(Resource){
		Uint8 r,g,b,a;
		EDL_GetPixel(Resource,1,1,&r,&g,&b,&a);
		Uint32 c=(r<<24)|(g<<16)|(b<<8)|0xFF;
		SDL_Surface *surface=EDL_ColorkeySurface(Resource,c,0);
		SDL_Rect s={0,0,r_display.w-(X*8),r_display.h};
		SDL_Rect d={r_display.x+(X*8),r_display.y,s.w,s.h};
		w_display->Blend(surface,&s,&d);
		SDL_FreeSurface(surface);
	}
	return Resource;
}

bool EngineJAST::SetBackground(SDL_Surface *Resource){
	if(Resource){
		w_display->Blit(Resource,0,&r_display);
	}
	return Resource;
}

bool EngineJAST::SetFrame(SDL_Surface *Resource){
	if(Resource){
		w_display->Blit(Resource);
	}
	return Resource;
}

bool EngineJAST::EventBackgroundMouseRightUp(int X,int Y){
	// Show options dialog
	EventGameDialog(VD_OPTIONS);
	return true;
}

bool EngineJAST::EventGameTick(){
	//if(state!=JASTSTATE_SELECT){
	if(state==JASTSTATE_WAITCLICK || state==JASTSTATE_PRESELECT){
		bool skip=keyok || keyctrl() || GetSkipmode();
		if(skip && w_tv->GetRemainingText()){
			w_tv->CompleteText();
			keyok=false;
		}
		else if(skip){
			w_tv->ClearText();
			if(state==JASTSTATE_PRESELECT){
				// Skip last message and show selection
				state=JASTSTATE_SELECT;
				ShowSelection(0);
			}
			else{
				// Normal text skip
				state=JASTSTATE_NORMAL;
			}
			keyok=false;
		}
	}
	return !(state==JASTSTATE_NORMAL);
}

bool EngineJAST::EventGameProcess(){
	bool retval=true;
	if(state==JASTSTATE_NORMAL && scriptindex && scriptindex<scriptlength){
		// Parse current line and prepare the next
		uString line=GetCurrentLine();
		GetNextLine();

		// Stack up parameters
		DStack stack;
		int c=GetParamCount(line);
		for(int i=0;i<c;i++){
			stack.Queue(new uString(GetParam(line,i)));
		}

		// Process line
		while(stack.Peek()){
			retval&=Interpret(&stack);
		}
	}
	return retval;
}

bool EngineJAST::EventSave(int Index){
	// Save data
	Savegame *save=new Savegame(NativeID(),Index);
	save->SaveString("scriptname",scriptname);
	save->SaveString("scriptlabel",scriptlabel);
	save->SaveUint32("scriptindex",scriptindex);
	save->SaveVector("variables",&vars);

	// Store graphics
	SDL_Surface *screen=EDL_CreateSurface(NativeWidth(),NativeHeight());
	Paint(screen,VL_CHOICES);
	save->SaveSurface("screen-thumb",screen,96,72);
	Paint(screen,VL_CHARACTERS);
	SDL_Rect rdisplay=r_display;
	rdisplay.y+=w_display->GetY();
	SDL_Surface *sdisplay=EDL_CreateSurface(rdisplay.w,rdisplay.h);
	EDL_BlitSurface(screen,&rdisplay,sdisplay,0);
	save->SaveSurface("screen-display",sdisplay);
	SDL_FreeSurface(screen);
	SDL_FreeSurface(sdisplay);

	// Close up shop
	save->Write();
	delete save;
	return true;
}

bool EngineJAST::EventLoad(int Index){
	bool retval=false;
	Savegame *load=new Savegame(NativeID(),Index);
	if(load->Read()){
		// Reload script
		load->LoadString("scriptname",&scriptname);
		LoadJASTScript(scriptname);

		// Reset data states
		load->LoadVector("variables",&vars);
		load->LoadString("scriptlabel",&scriptlabel);
		load->LoadUint32("scriptindex",(Uint32*)&scriptindex);

		// Load graphics
		SDL_Surface *tmps;
		if(load->LoadSurface("screen-display",&tmps)){
			SetBackground(tmps);
			SDL_FreeSurface(tmps);
		}
	}
	return retval;
}

uString EngineJAST::Unquote(uString Text){
	uString retval=Text;
	if(Text.length()){
		if(Text[0]=='\'' && Text[Text.length()-1]=='\''){
			retval=Text.substr(1,Text.length()-2);
		}
		if(Text[0]=='\"' && Text[Text.length()-1]=='\"'){
			retval=Text.substr(1,Text.length()-2);
		}
	}
	return retval;
}

uString EngineJAST::PopString(DStack *Stack){
	uString retval;
	uString *popped=(uString*)Stack->Pop();
	if(popped){
		retval=*popped;
		delete popped;
	}
	return retval;
}

bool EngineJAST::Interpret(DStack *Stack){
	bool retval=true;
	uString cmd=PopString(Stack);
	if(cmd=="" || cmd=="db" || cmd=="dw"){
		// Can be anything ...
	}
	else if(cmd[0]=='\"' || cmd[0]=='\''){
		// Print quoted text
		cmd=Unquote(cmd);
		for(unsigned int i=0;i<cmd.length();i++){
			if(cmd[i]=='%'){
				uString p1=cmd.substr(0,i);
				uString p2=cmd.substr(i+4);
				cmd=p1+uString("Shuji")+p2;
				i=0;
			}
		}
		w_tv->PrintText(cmd);
	}
	else if(cmd=="cr"){
		// Newline
		w_tv->PrintNewline();
	}
	else if(cmd=="next"){
		// Wait for user input
		state=JASTSTATE_WAITCLICK;
		retval=false;
	}
	else if(cmd=="pause"){
		// Not supported for now (Should have a timed delay in the text)
	}
	else if(cmd=="bgm"){
		// Start BGM track
		cmd=PopString(Stack);
		if(cmd.length()==1){
			cmd=uString("0")+cmd;
		}
		PlayMusic(uString("SOS")+cmd);
	}
	else if(cmd=="flash"){
		// Flash a white
		retval=false;
		int cnt=PopString(Stack).to_int();
		SDL_Rect dst=w_display->GetPosition();
		SDL_Rect src={0,0,w_display->GetWidth(),w_display->GetHeight()};
		for(int i=0;i<cnt;i++){
			Fade *white=new FadeWhite(dst,100);
			Fade *real=new Fade(dst,w_display->GetSurface(),src,100);
			AddAnimation(white);
			AddAnimation(real);
		}
	}
	else if(cmd=="gata"){
		// The classic, and seemingly quintessential, shake screen effect..
		retval=false;
		int cnt=PopString(Stack).to_int();
		/*SDL_Surface *display=SDL_GetVideoSurface();
		SDL_Rect start={0,0,display->w,display->h};
		SDL_Rect stop={0,0,display->w,display->h};
		for(int i=0;i<cnt;i++){
			stop.x=EDL_Random(-50,50);
			stop.y=EDL_Random(-50,50);
			Slide *away=new Slide(display,start,stop,100);
			Slide *back=new Slide(display,stop,start,100);
			AddAnimation(away);
			AddAnimation(back);
		}*/
	}
	else if(cmd=="erase"){
		cmd=PopString(Stack);
		if(cmd=="e_gflg"){
			while(Stack->Peek()){
				vars.SetUint8(PopString(Stack).to_int(),0);
			}
		}
		else{
			LogError("Invalid erase operation: %s",cmd.c_str());
			Stack->Flush();
		}
	}
	else if(cmd=="flg"){
		// Flag operation (Volatile flags)
		cmd=PopString(Stack);
		if(cmd=="f_copyi"){
			// Copy integer
			int d=PopString(Stack).to_int();
			Uint8 v=PopString(Stack).to_int();
			vars.SetUint8(d,v);
		}
		else if(cmd=="f_and"){
			// And two operators
			int d=PopString(Stack).to_int();
			int s1=PopString(Stack).to_int();
			int s2=PopString(Stack).to_int();
			int v1=vars.GetUint8(s1);
			int v2=vars.GetUint8(s2);
			vars.SetUint8(d,v1&v2);
		}
		else if(cmd=="f_mand"){
			// And multiple operators
			PopString(Stack);
			int d=PopString(Stack).to_int();
			int s=vars.GetUint8(d);
			while(Stack->Peek()){
				int t=PopString(Stack).to_int();
				s&=vars.GetUint8(t);
			}
			vars.SetUint8(d,s);
		}
		else if(cmd=="f_exit"){
			// Stop executing block if var=0
			int s=PopString(Stack).to_int();
			s=vars.GetUint8(s);
			if(s==0){
				if(scriptlabel=="M_EXIT" || !GetLine("M_EXIT")){
					state=JASTSTATE_PRESELECT;
					retval=false;
				}
			}
		}
		else if(cmd=="f_jeq"){
			// Execute following line if NULL
			int s=PopString(Stack).to_int();
			Uint8 v=vars.GetUint8(s);
			if(v!=0){
				GetNextLine();
			}
		}
		else{
			LogError("Invalid flag operation: %s (%s)",cmd.c_str());
			Stack->Flush();
		}
	}
	else if(cmd=="g_flg"){
		// Flag operation (Persistent flags)
		cmd=PopString(Stack);
		if(cmd=="g_copyi"){
			// Copy intermediate
			int d=PopString(Stack).to_int();
			Uint8 v=PopString(Stack).to_int();
			vars.SetUint8(d,v);
		}
		else if(cmd=="g_copyf"){
			// Copy flag
			int d=PopString(Stack).to_int();
			Uint8 s=PopString(Stack).to_int();
			Uint8 v=vars.GetUint8(s);
			vars.SetUint8(d,v);
		}
		else{
			LogError("Invalid gflag operation: %s (%s)",cmd.c_str());
			Stack->Flush();
		}
	}
	else if(cmd=="c_bld"){
		// Build choices
		cmd=PopString(Stack);
		int main=PopString(Stack).to_int();
		int sub=PopString(Stack).to_int();
		if(cmd=="c_v_on"){
			ShowMain(main,false);
		}
		else if(cmd=="c_v_off"){
			ShowMain(main,true);
		}
		else if(cmd=="c_n_on"){
			ShowSub(main,sub,false);
		}
		else if(cmd=="c_n_off"){
			ShowSub(main,sub,true);
		}
		else{
			LogError("Invalid choice operation: %s",cmd.c_str());
			Stack->Flush();
		}
	}
	else if(cmd=="buf"){
		// Blit cached background
		cmd=PopString(Stack);
		SetBackground(s_background);
	}
	else if(cmd=="sinario"){
		// Load next script
		cmd=PopString(Stack);
		cmd=Unquote(cmd);
		LoadJASTScript(cmd);
		Stack->Flush();
	}
	else if(cmd=="0000h"){
		// Go to default exit or show command options
		if(scriptlabel=="M_EXIT" || !GetLine("M_EXIT")){
			state=JASTSTATE_PRESELECT;
			retval=false;
		}
	}
	else if(cmd=="ani"){
		// We dont fucking *care* about eye animations!!
		Stack->Flush();
	}
	else if(cmd=="bak_init"){
		// Cache a background image
		cmd=PopString(Stack);
		cmd=Unquote(cmd);
		if(s_background){
			SDL_FreeSurface(s_background);
		}
		s_background=LoadImage(cmd);
		if(!s_background){
			LogError("Invalid backgound image: [%s]",cmd.c_str());
			Stack->Flush();
		}
	}
	else if(cmd=="b_o"){
		// Fill background with black
		cmd=PopString(Stack);
		SDL_Surface *s=EDL_CreateSurface(r_display.w,r_display.h);
		SetBackground(s);
		SDL_FreeSurface(s);
		r_display.y+=w_display->GetY();
		SetTransition(&r_display);
		r_display.y-=w_display->GetY();
	}
	else if(cmd=="b_p1"){
		// Use effect to show cached background
		cmd=PopString(Stack);
		SetBackground(s_background);
		r_display.y+=w_display->GetY();
		SetTransition(&r_display);
		r_display.y-=w_display->GetY();
	}
	else if(cmd=="e_cg"){
		// Load background graphics without caching it
		uString fx=PopString(Stack);
		uString fn=PopString(Stack);
		fn=Unquote(fn);
		SDL_Surface *s=LoadImage(fn);
		SetBackground(s);
		SDL_FreeSurface(s);
		r_display.y+=w_display->GetY();
		SetTransition(&r_display);
		r_display.y-=w_display->GetY();
	}
	else if(cmd=="t_cg" || cmd=="t_cg1" || cmd=="k_cg" || cmd=="tu_cg"){
		// Load character graphics
		uString base=PopString(Stack);
		uString op1=PopString(Stack);
		uString op2=PopString(Stack);
		int x=PopString(Stack).to_int();
		PopString(Stack);

		LogTest("CG: %s %s %s %s %d",base.c_str(),
				cmd.c_str(),op1.c_str(),op2.c_str(),x);

		// Debug paramers
		if(cmd=="k_cg"){
			x=op2.to_int();
			op2="";
		}

		// Load base graphics
		base=Unquote(base);
		uString name=base;
		op1=Unquote(op1);
		if(op1!="0" && op1!="-1"){
			name=base+op1;
		}
		SDL_Surface *tmp=LoadImage(name);
		if(tmp){
			SetCharacter(tmp,x);
			SDL_FreeSurface(tmp);
		}
		else{
			LogError("Invalid character image: [%s]",name.c_str());
			Stack->Flush();
		}

		// Load overlay graphics
		if(op2.length() && op2!="0" && op2!="-1"){
			op2=Unquote(op2);
			name=base+op2;
			tmp=LoadImage(name);
			if(tmp){
				SetCharacter(tmp,x);
				SDL_FreeSurface(tmp);
			}
			else{
				LogError("Invalid character image: [%s]",name.c_str());
				Stack->Flush();
			}
		}
	}
	else if(cmd=="cg"){
		// Use graphics from resource list
		cmd=PopString(Stack).c_str();
		int index=cmd.to_int();
		FILENAME_TYPE *filename=filenames;
		for(int i=0;i<index && filename;i++){
			filename=filename->next;
		}
		if(filename){
			SDL_Surface *tmp=LoadImage(filename->name);
			if(tmp){
				if(filename->type=="P"){
					// Temporary background?
					SetBackground(tmp);
					SDL_FreeSurface(tmp);
				}
				else if(filename->type=="N"){
					// Consistent background?
					if(s_background){
						SDL_FreeSurface(s_background);
					}
					s_background=tmp;
					SetBackground(tmp);
				}
				else if(filename->type=="8"){
					// CG?
					SetCharacter(tmp,0);
					SDL_FreeSurface(tmp);
				}
				else{
					LogError("Invalid cg type: %s",filename->type.c_str());
					SDL_FreeSurface(tmp);
				}
			}
			else{
				LogError("Invalid cg reference: %s",filename->name.c_str());
				Stack->Flush();
			}
		}
		else{
				LogError("Invalid cg index: %s",cmd.c_str());
		}
	}
	else if(cmd=="ef_set"){
		// Set flag
		int d=PopString(Stack).to_int();
		int v=vars.GetUint8(d);
		if(!v){
			vars.SetUint8(d,1);
		}
	}
	else{
		if(GetLine(cmd)){
			// Check for labeled jumps
			LogTest("Jumping to %s",cmd.c_str());
		}
		else{
			LogError("UNKNOWN PARAMETER: %s",cmd.c_str());
			Stack->Flush();
		}
	}
	return retval;
}


