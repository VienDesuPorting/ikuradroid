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

#include "mcwhisper.h"

MCWhisper::MCWhisper(EngineVN *Engine) : Whisper(Engine){
	datbuffer=0;
	datsize=0;
	pakbuffer=0;
	paksize=0;
	pakindex=0;
	s21buffer=0;
	s21size=0;
	s21index=0;
}

void MCWhisper::ParseScript(){
	for(;pakindex<paksize;pakindex++){
		if(pakbuffer[pakindex]=='S' && 
				pakbuffer[pakindex+1]=='n' && 
				pakbuffer[pakindex+2]=='r'){
			// Trace to first letter of script name
			pakindex+=3;
			while(pakbuffer[pakindex]<'A' || pakbuffer[pakindex]>'Z'){
				pakindex++;
			}

			// Extract script name
			uString name;
			while((pakbuffer[pakindex]>='A' && pakbuffer[pakindex]<='Z') ||
					(pakbuffer[pakindex]>='0' && pakbuffer[pakindex]<='9')){
				name+=(char)pakbuffer[pakindex++];
			}

			// Open new s21 script
			RWops *s21blob=scripts.GetResource(name);
			if(s21blob){
				if(s21buffer){
					delete [] s21buffer;
				}
				s21blob->Seek(0,SEEK_END);
				s21size=s21blob->Tell();
				s21blob->Seek(0,SEEK_SET);
				s21buffer=new Uint8[s21size];
				s21index=0;
				s21blob->Read(s21buffer,s21size);
				delete s21blob;
				break;
			}
		} 
	}
}

bool MCWhisper::ParseVoice(Uint16 Start){
	// Find next voice reference in current s21 script
	bool retval=false;
	uString voice;
	for(int i=s21index;i+4<s21size;i++){
		if(s21buffer[i]=='v' &&
				s21buffer[i+1]=='o' &&
				s21buffer[i+2]=='i' &&
				s21buffer[i+3]=='c' &&
				s21buffer[i+4]=='e'){
			s21index=i+4;
			for(int j=i+4;j+3<s21size;j++){
				if(s21buffer[j]>='0' && s21buffer[j]<='9'){
					voice="V";
					voice+=datname[0];
					voice+=(char)s21buffer[j++];
					voice+=(char)s21buffer[j++];
					voice+=(char)s21buffer[j++];
					s21index=j;
					//LogTest("ADDING:%d:%s",Start,voice.c_str());
					break;
				}
			}
			break;
		}
	}

	if(voice.length()){
		// Register voice
		SetVoice(Start,voice);
		retval=true;
	}
	else if(pakindex<paksize){
		// Traverse to next subscript
		ParseScript();
		retval=ParseVoice(Start);
	}
	return retval;
}

/*! \brief Processes stubs from english legacy scripts and requests voices
 *  \param Start Start of datbuffer
 *  \param End End position of datbuffer
 */
void MCWhisper::ProcessStub(Uint16 Start,Uint16 End){
	if(Start<End && End-Start<1000){
		// Confirm name heading
		for(int i=Start;i<End;i++){
			if(datbuffer[i]=='['){
				// Extract name
				uString name;
				for(i++;i<End;i++){
					if(datbuffer[i]==']'){
						break;
					}
					else if(datbuffer[i]==' '){
					}
					else{
						name+=(char)tolower(datbuffer[i]);
					}
				}

				// Confirm non-player name
				if(name!="hajime"){
					// Confirm quotes and letters
					bool quote=false;
					bool letters=false;
					for(int j=i;j<End/* && (!quote || !letters)*/;j++){
						quote|=datbuffer[j]=='\"';
						letters|=(datbuffer[j]>='A' && datbuffer[j]<='z');
					}
					if(quote && letters){
						//LogTest("REQUESTING:%s:%d",name.c_str(),Start);
						ParseVoice(Start);
					}
				}
				break;
			}
		}
	}
}

/*! \brief Processes english legacy script byte-by-byte
 *  \param Index Current index of the windy script
 *  \param Name Name of current windyscript
 */
void MCWhisper::ProcessScript(Uint16 Index){
	// Scan for text items in legacy source
	bool processastext=true;
	for(int i=Index;i<datsize;i++){
		Uint16 opcode=(datbuffer[i+1]<<8)|datbuffer[i];

		// Enable/disable selection mode
		if(opcode==0x0022){
			Uint16 p1=(datbuffer[i+3]<<8)|datbuffer[i+2];
			if(p1==0x03E8){
				processastext=false;
			}
			else if(p1==0x0120){
				processastext=true;
			}
		}

		// Load textstubs
		if(opcode==0x0015){
			//Uint16 p1=(datbuffer[i+3]<<8)|datbuffer[i+2];
			Uint16 p2=(datbuffer[i+5]<<8)|datbuffer[i+4];
			int index=p2;
			if(index && index<datsize && 
					(datbuffer[index]>=20 && datbuffer[index]<=126) &&
					(datbuffer[index-1]<20 || datbuffer[index-1]>126)){
				if(datbuffer[index]==0x0F){
					index+=3;
				}
				if(datbuffer[index]==0x03){
					index+=2;
				}

				if(processastext){
					// Extract strings
					int start=index;
					int end=start;
					for(;end<datsize;end++){
						if(datbuffer[end]==0x04){
							end+=2;
						}
						else if(datbuffer[end]=='~'){
							end+=2;
						}
						else if(datbuffer[end]<0x04){
							ProcessStub(start,end);
							/*
							if(datbuffer[end]==2){
								LogTest("KEEEEEEEEPING:%s",datbuffer+start);
							}
							else{
								start=end+1;
							}
							*/
							start=end+1;
							if(datbuffer[++end]<0x04){
								break;
							}
						}
					}

				}
				else{
					// Recursively parse options as files
				}
			}
		}
	}

	// Queue up any remaining
	while(ParseVoice(0xFFFF));
}

/*! \brief Syncronize voices to a legacy Windy script
 *  \param Name Name of script to syncronize to
 */
void MCWhisper::Synchronize(uString Name,Stringlist List){
	// Read legacy resource we want to sync to
	RWops *datblob=engine->LoadScript(Name);
	RWops *pakblob=scripts.GetResource(Name);
	s21buffer=0;
	s21size=0;
	s21index=0;

	if(datblob && !pakblob){
		// flat input
		if((pakblob=scripts.GetResource(Name+'a'))){
			// Open new s21 script
			RWops *s21blob=scripts.GetResource(Name+'a');
			if(s21blob){
				if(s21buffer){
					delete [] s21buffer;
				}
				s21blob->Seek(0,SEEK_END);
				s21size=s21blob->Tell();
				s21blob->Seek(0,SEEK_SET);
				s21buffer=new Uint8[s21size];
				s21index=0;
				s21blob->Read(s21buffer,s21size);
				delete s21blob;
			}
		}
	}

	if(datblob && pakblob){
		// Flush existing data
		ClearVoices();

		// Read blobs to ram
		datname=Name;
		datblob->Seek(0,SEEK_END);
		datsize=datblob->Tell();
		datblob->Seek(0,SEEK_SET);
		datbuffer=new Uint8[datsize];
		pakblob->Seek(0,SEEK_END);
		paksize=pakblob->Tell();
		pakblob->Seek(0,SEEK_SET);
		pakbuffer=new Uint8[paksize];
		pakindex=0;

		// Recursively syncronize data to new scripts
		if(datblob->Read(datbuffer,datsize)>0 && 
		   pakblob->Read(pakbuffer,paksize)>0){
			// Decode windy script and process it
			bool toggle=true;
			for(int i=0;i<datsize;i++){
				toggle=!toggle;
				if(toggle){
					datbuffer[i]=datbuffer[i]+1;
				}
				else{
					datbuffer[i]=datbuffer[i]-1;
				}
			}
			ProcessScript(0);

			// Postprocess
			if(EDL_Searchname(Name)=="b01"){
				// First voice is in previous script?
				DropVoice(0);
			}
		}
		else{
			LogError("Failed to read syncronization: %s",Name.c_str());
		}

		// Close up shop
		delete datblob;
		delete [] datbuffer;
		delete pakblob;
		delete [] pakbuffer;
		if(s21buffer){
			delete [] s21buffer;
		}
	}
	else{
		LogError("Failed to syncronize: %s",Name.c_str());
	}
}


