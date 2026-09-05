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

#include "whisper.h"

Whisper::Whisper(EngineVN *Engine){
	engine=Engine;
	head=0;
}

Whisper::~Whisper(){
	ClearVoices();
}

/*! \brief Skews the voice queue by dropping a voice reference
 */
void Whisper::DropVoice(int Position){
	WhisperItem *tmpptr=head;
	for(int i=0;tmpptr && i<Position;i++){
		tmpptr=tmpptr->next;
	}
	while(tmpptr){
		if(tmpptr->next){
			tmpptr->name=tmpptr->next->name;
		}
		else{
			tmpptr->address=0xFFFF;
		}
		tmpptr=tmpptr->next;
	}
}

/*
void Whisper::PushVoice(int Position){
}
*/

void Whisper::AddResource(ArchiveBase *Archive){
	scripts.AddResource(Archive);
}

uString Whisper::GetVoice(Uint16 Address){
	WhisperItem *tmpptr=head;
	while(tmpptr){
		if(tmpptr->address==Address){
			return tmpptr->name;
		}
		else{
			tmpptr=tmpptr->next;
		}
	}
	return "";
}

void Whisper::SetVoice(Uint16 Address,uString Name){
	WhisperItem *newitem=new WhisperItem;
	newitem->address=Address;
	newitem->name=Name;
	newitem->next=0;

	if(head){
		WhisperItem *tmpptr=head;
		while(tmpptr->next){
			tmpptr=tmpptr->next;
		}
		tmpptr->next=newitem;
	}
	else{
		head=newitem;
	}
}

/*! \brief Removes all currently indexed voice resources
 */
void Whisper::ClearVoices(){
	while(head){
		WhisperItem *tmpptr=head->next;
		delete head;
		head=tmpptr;
	}
}


