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

#include "evn.h"

EngineVN::EngineVN(int Width,int Height) : EngineVideo(Width,Height){
	// Set default values
	msgdelay_interval=25;
	msgdelay_enabled=true;
	keyok=false;
	keycancel=false;
	skip=false;
	savename="Unknown";

	// Enable default audio channels
	SetMusicEnabled(true);
	SetSoundEnabled(VA_SOUNDS,true);
	SetSoundEnabled(VA_VOICES,true);
#ifndef ANDROID
	// Load sequencer and create logo
//	Sequences=new Sequencer(this);
//	Sequences->LogoRandom();
	// Assign a standard menu
	if(Cfg::System::Mainmenu){
		AddWidget(new StdMenu(this),VL_DIALOG);
	}
#endif
	// Apply standard patches
	uString path=Cfg::Path::game;
	if(Cfg::Path::game.length()){
		r_voices.AddResource(new ArchiveViLE(path+"/patch_voices.pck"));
		r_videos.AddResource(new ArchiveViLE(path+"/patch_videos.pck"));
		r_scripts.AddResource(new ArchiveViLE(path+"/patch_scripts.pck"));
		r_other.AddResource(new ArchiveViLE(path+"/patch_other.pck"));
		r_images.AddResource(new ArchiveViLE(path+"/patch_images.pck"));
		r_bgm.AddResource(new ArchiveViLE(path+"/patch_bgm.pck"));
		r_se.AddResource(new ArchiveViLE(path+"/patch_se.pck"));
	}
	r_voices.AddResource(new ArchiveViLE("patch_voices.pck"));
	r_videos.AddResource(new ArchiveViLE("patch_videos.pck"));
	r_scripts.AddResource(new ArchiveViLE("patch_scripts.pck"));
	r_other.AddResource(new ArchiveViLE("patch_other.pck"));
	r_images.AddResource(new ArchiveViLE("patch_images.pck"));
	r_bgm.AddResource(new ArchiveViLE("patch_bgm.pck"));
	r_se.AddResource(new ArchiveViLE("patch_se.pck"));

    opt = 0;
    Halt = 0;
    Sav = 0;
    Loa = 0;

}

EngineVN::~EngineVN(){
#ifndef ANDROID
	//delete Sequences;
#endif
}

/*! \brief Checks existence of resource file and logs verbosely
 *  \param Path Directory
 *  \param Resource Filename path relative to Path
 *  \return True if file exists
 */
bool EngineVN::ProbeResource(uString Path,uString Resource){
	uString path=EDL_Realname(Path+Resource);
	bool retval=EDL_ReadableFile(path);
	LogVerbose("\t%-40s%s",path.c_str(),retval?"PASS":"FAIL");
	return retval;
}

/*! \brief Checks if two strings matches and logs verbosely
 *  \param S1 String
 *  \param S2 String to compare with
 *  \return True if the strings matches (case insensitive)
 */
bool EngineVN::ProbeString(uString S1,uString S2){
	S1=EDL_Lower(S1);
	S2=EDL_Lower(S2);
	bool retval=(S1==S2);
	uString msg=S1+uString(" == ")+S2;
	LogVerbose("\t%-40s%s",msg.c_str(),retval?"PASS":"FAIL");
	return retval;
}

/*! \brief Gets current state of control key
 *  \returns True if keyboard initiated skip is enabled
 */
bool EngineVN::keyctrl(){
	Uint8 *keystate=(Uint8*)SDL_GetKeyboardState(NULL);
	return keystate[SDL_SCANCODE_RCTRL] || keystate[SDL_SCANCODE_LCTRL];
	//return false;
}

/*! \brief Enumerates a list of voice archives
 *  \param Pointer to list to populate
 *  \return Number of items added to list
 */
int EngineVN::GetVoices(Stringlist *List){
	return r_voices.EnumerateArchives(List);
}

/*! \brief Enumerates a list of archives containing scripts
 *  \param Pointer to list to populate
 *  \return Number of items added to list
 */
int EngineVN::GetScripts(Stringlist *List){
	return r_scripts.EnumerateArchives(List);
}

/*! \brief Enumerates a list of archives with undefined resources
 *  \param Pointer to list to populate
 *  \return Number of items added to list
 */
int EngineVN::GetOther(Stringlist *List){
	return r_other.EnumerateArchives(List);
}

/*! \brief Enumerates a list of image archives
 *  \param Pointer to list to populate
 *  \return Number of items added to list
 */
int EngineVN::GetImages(Stringlist *List){
	return r_images.EnumerateArchives(List);
}

/*! \brief Enumerates a list of music archives
 *  \param Pointer to list to populate
 *  \return Number of items added to list
 */
int EngineVN::GetBGM(Stringlist *List){
	return r_bgm.EnumerateArchives(List);
}

/*! \brief Enumerates a list of sound effect archives
 *  \param Pointer to list to populate
 *  \return Number of items added to list
 */
int EngineVN::GetSE(Stringlist *List){
	return r_se.EnumerateArchives(List);
}

/*! \brief Enumerates a list of video archives
 *  \param Pointer to list to populate
 *  \return Number of items added to list
 */
int EngineVN::GetVideo(Stringlist *List){
	return r_videos.EnumerateArchives(List);
}

/*! \brief Add archive of voice resources
 *  \param Name Path to archive(s)
 */
void EngineVN::AddVoices(ArchiveBase *Archive){
	r_voices.AddResource(Archive);
}

/*! \brief Add archive of video resources
 *  \param Name Path to archive(s)
 */
void EngineVN::AddVideo(ArchiveBase *Archive){
	r_videos.AddResource(Archive);
}

/*! \brief Add archive of game script resources
 *  \param Name Path to archive(s)
 */
void EngineVN::AddScripts(ArchiveBase *Archive){
	r_scripts.AddResource(Archive);
}

/*! \brief Add archive of undefined resources
 *  \param Name Path to archive(s)
 */
void EngineVN::AddOther(ArchiveBase *Archive){
	r_other.AddResource(Archive);
}

/*! \brief Add archive of image resources
 *  \param Name Path to archive(s)
 */
void EngineVN::AddImages(ArchiveBase *Archive){
	r_images.AddResource(Archive);
}

/*! \brief Add archive of music resources
 *  \param Name Path to archive(s)
 */
void EngineVN::AddBGM(ArchiveBase *Archive){
	r_bgm.AddResource(Archive);
}

/*! \brief Add archive of sound effect resources
 *  \param Name Path to archive(s)
 */
void EngineVN::AddSE(ArchiveBase *Archive){
	r_se.AddResource(Archive);
}

/*! \brief Add video resource to animation queue
 *  \param Name Name of video resource
 *  \return True if resource was accepted
 */
bool EngineVN::QueueVideo(uString Name){
	RWops *ops=LoadVideo(Name);
	if(ops){
		AddAnimation(new VideoWidget(this,ops));
	}
	return ops;
}

/*! \brief Add video resource to animation queue
 *  \param Video Blob containing video resource
 *  \return True if resource was accepted
 */
bool EngineVN::QueueVideo(RWops *Video){
	if(Video){
		AddAnimation(new VideoWidget(this,Video));
	}
	return Video;
}

/*! \brief Add music resource to animation queue
 *  \param Name Name of video resource
 *  \return True if resource was accepted
 *
 *  Music track will start playing when previously queued Animation objects
 *  have completed. It will not affect graphics in any way (unless you set
 *  the widget graphic).
 */
bool EngineVN::QueueMusic(uString Name){
	RWops *ops=LoadMusic(Name);
	if(ops){
		AddAnimation(new MusicWidget(this,ops));
	}
	return ops;
}

/*! \brief Add music resource to animation queue
 *  \param Music Blob containing music resource
 *  \return True if resource was accepted
 *
 *  Music track will start playing when previously queued Animation objects
 *  have completed. It will not affect graphics in any way (unless you set
 *  the widget graphic).
 */
bool EngineVN::QueueMusic(RWops *Music){
	if(Music){
		AddAnimation(new MusicWidget(this,Music));
	}
	return Music;
}

/*! \brief Play video resource
 *  \param Name Name of video resource
 *  \return True if resource was accepted
 */
bool EngineVN::PlayVideo(uString Name){
	// Try to load video resource
	RWops *ops=r_videos.GetResource(Name);
	bool retval=false;
	if(ops){
		LogVerbose("Playing video: %s",Name.c_str());
		//retval=EngineVideo::PlayVideo(ops);
		delete ops;
	}
	else{
		LogVerbose("Invalid video resource: %s",Name.c_str());
	}
	return retval;
}

/*! \brief Play video resource
 *  \param Video Blob containing video resource
 *  \return True if resource was accepted
 */
bool EngineVN::PlayVideo(RWops *Video){

	return false;//EngineVideo::PlayVideo(Video);
}

/*! \brief Play music resource by name
 *  \param Name Name of music resource
 *  \return True if resource was accepted
 */
bool EngineVN::PlayMusic(uString Name){
	// Try to load music resource (NULL stops current)
	RWops *ops=r_bgm.GetResource(Name);
	LogVerbose("open music: %s",Name.c_str());
	bool retval=false;
	if(ops){
		LogVerbose("Playing music: %s",Name.c_str());
		retval=EngineMixer::PlayMusic(ops);
		delete ops;
	}
	return retval;
}

/*! \brief Play a preloaded music resource
 *  \param Music Blob containing music resource
 *  \return True if resource was accepted
 */
bool EngineVN::PlayMusic(RWops *Music){
	return EngineMixer::PlayMusic(Music);
}

/*! \brief Plays music track
 *  \param Track Numeric to discern bgm id
 *  \return True if the track was identified and started
 *
 *  Older games are often designed to play music from the original CDROM,
 *  but has been altered to play said track from a mp3/ogg file using the
 *  track index to format a standard name.
 *
 *  This method will poll the standard sources and play the first audio
 *  source to match a identification.
 */
bool EngineVN::PlayTrack(int Track){
	bool retval=PlayCDDA(Track);
	if(!retval)		retval=PlayMusic(EDL_Format("%02d",Track));
	if(!retval)		retval=PlayMusic(EDL_Format("%d",Track));
	if(!retval)		retval=PlayMusic(EDL_Format("TK%02d",Track));
	if(!retval)		retval=PlayMusic(EDL_Format("TK%d",Track));
	if(!retval)		retval=PlayMusic(EDL_Format("TK_%02d",Track));
	if(!retval)		retval=PlayMusic(EDL_Format("TK_%d",Track));
	if(!retval)		retval=PlayMusic(EDL_Format("TK-%02d",Track));
	if(!retval)		retval=PlayMusic(EDL_Format("TK-%d",Track));
	return retval;
}

/*! \brief Play sound effect
 *  \param Name Name of sound resource
 *  \param Channel Audio channel to play sample in
 *  \param Repear Times to repeat track (0=Play once)
 *  \return True if resource was accepted
 */
bool EngineVN::PlaySound(uString Name,int Channel,int Repeat){
	// Try to load sound resource (NULL stops current)
	RWops *ops=r_se.GetResource(Name);
	bool retval=false;
	if(ops){
		LogVerbose("Playing sound: %s@%d",Name.c_str(),Channel);
		retval=EngineMixer::PlaySound(ops,Channel,Repeat);
		delete ops;
	}
	else{
		LogVerbose("Invalid sound resource: %s",Name.c_str());
		delete ops;
	}
	return retval;
}

/*! \brief Play voice resource
 *  \param Voice Audio data
 *  \param Channel Audio channel to play sample in
 *  \return True if resource was accepted
 */
bool EngineVN::PlaySound(RWops *SE,int Channel,int Repeat){
	// Try to load voice resource (NULL stops current)
	return EngineMixer::PlaySound(SE,Channel,Repeat);
}

/*! \brief Play voice resource
 *  \param Name Name of sound resource
 *  \param Channel Audio channel to play sample in
 *  \return True if resource was accepted
 */
bool EngineVN::PlayVoice(uString Name,int Channel){
	// Try to load voice resource (NULL stops current)
    LogVerbose("Playing kkvoice: %s",Name.c_str());

	RWops *ops=r_voices.GetResource(Name);
	bool retval=false;
	if(ops){
		LogVerbose("Playing voice: %s@%d",Name.c_str(),Channel);
		retval=EngineMixer::PlaySound(ops,Channel);
		delete ops;
	}
	else{
		LogVerbose("Invalid sound resource: %s",Name.c_str());
		delete ops;
	}
	return retval;
}

/*! \brief Play voice resource
 *  \param Voice Audio data
 *  \param Channel Audio channel to play sample in
 *  \return True if resource was accepted
 */
bool EngineVN::PlayVoice(RWops *Voice,int Channel){
	// Try to load voice resource (NULL stops current)
	return EngineMixer::PlaySound(Voice,Channel);
}

/*! \brief Loads script resource
 *  \param Name Name of resource to load
 *  \param Ext Extension (In case of name-crashes across types)
 *  \return Pointer to resource that must be freed
 */
RWops *EngineVN::LoadScript(uString Name,uString Ext){
	return r_scripts.GetResource(EDL_DefaultExtension(Name,Ext));
}

/*! \brief Loads undefined resource
 *  \param Name Name of resource to load
 *  \param Ext Extension (In case of name-crashes across types)
 *  \return Pointer to resource that must be freed
 */
RWops *EngineVN::LoadOther(uString Name,uString Ext){
	return r_other.GetResource(EDL_DefaultExtension(Name,Ext));
}

/*! \brief Loads video resource
 *  \param Name Name of resource to load
 *  \param Ext Extension (In case of name-crashes across types)
 *  \return Pointer to resource that must be freed
 */
RWops *EngineVN::LoadVideo(uString Name,uString Ext){
	return r_videos.GetResource(EDL_DefaultExtension(Name,Ext));
}

/*! \brief Loads music resource
 *  \param Name Name of resource to load
 *  \param Ext Extension (In case of name-crashes across types)
 *  \return Pointer to resource that must be freed
 */
RWops *EngineVN::LoadMusic(uString Name,uString Ext){
	return r_bgm.GetResource(EDL_DefaultExtension(Name,Ext));
}

/*! \brief Loads sound resource
 *  \param Name Name of resource to load
 *  \param Ext Extension (In case of name-crashes across types)
 *  \return Pointer to resource that must be freed
 */
RWops *EngineVN::LoadSE(uString Name,uString Ext){
	return r_se.GetResource(EDL_DefaultExtension(Name,Ext));
}

/*! \brief Loads voice resources
 *  \param Name Name of resource to load
 *  \param Ext Extension (In case of name-crashes across types)
 *  \return Pointer to resource that must be freed
 */
RWops *EngineVN::LoadVoice(uString Name,uString Ext){
	return r_voices.GetResource(EDL_DefaultExtension(Name,Ext));
}

/*! \brief Loads image resource
 *  \param Name Name of resource to load
 *  \param Ext Extension (In case of name-crashes across types)
 *  \return Pointer to resource that must be freed
 */
SDL_Surface *EngineVN::LoadImage(uString Name,uString Ext){
	return r_images.GetImage(EDL_DefaultExtension(Name,Ext));
}

/*! \brief Loads animation resource
 *  \param Name Name of resource to load
 *  \return Pointer to resource that must be freed
 */
SDL_Surface **EngineVN::LoadAnimation(uString Name,uString Ext){
	return r_images.GetAnimation(EDL_DefaultExtension(Name,Ext));
}

/*! \brief Registers a textual name to use for savegames
 *  \param Name New savegame name
 */
void EngineVN::SetSavename(uString Name){
	savename=Name;
}

/*! \brief Reads name to use for savegames
 *  \returns Savegame name
 */
uString EngineVN::GetSavename(){
	return savename;
}

void EngineVN::SetSkipmode(bool Skip){
	skip=Skip;
}

bool EngineVN::GetSkipmode(){
	return skip;
}

void EngineVN::SetMessageDelayEnabled(bool Enabled){
	msgdelay_enabled=Enabled;
}

void EngineVN::SetMessageDelayInterval(int Interval){
	msgdelay_interval=Interval;
}

bool EngineVN::GetMessageDelayEnabled(){
	return msgdelay_enabled;
}

int EngineVN::GetMessageDelayInterval(){
	return msgdelay_interval;
}

void EngineVN::EventGameDialog(VN_DIALOGS Dialog){

    DestroyWidget(opt,VL_DIALOG);
    DestroyWidget(Halt,VL_DIALOG);
    DestroyWidget(Sav,VL_DIALOG);
    DestroyWidget(Loa,VL_DIALOG);

	if(Dialog==VD_OPTIONS){
        opt = new Options(this);
		AddWidget(opt,VL_DIALOG);
	}
	else if(Dialog==VD_SHUTDOWN){
        Halt = new StdHalt(this);
		AddWidget(Halt,VL_DIALOG);
	}
	else if(Dialog==VD_SAVE){
        Sav =new StdSave(this);
		AddWidget(Sav,VL_DIALOG);
	}
	else if(Dialog==VD_LOAD){
        Loa = new StdLoad(this);
		AddWidget(Loa,VL_DIALOG);
	}
}

bool EngineVN::EventSaveSystem(){
	// Save system settings
	Savegame save(NativeID(),999);

	save.SaveUint32("skipmode",skip);
	save.SaveString("savename",savename);
	save.SaveUint32("fullscreen",GetFullscreen());
	save.SaveUint32("msgspeed_enabled",msgdelay_enabled);
	save.SaveUint32("msgspeed_delay",msgdelay_interval);
	save.SaveUint32("bgm_enabled",GetMusicEnabled());
	save.SaveUint32("bgm_volume",GetVolumeMusic());
	save.SaveUint32("voice_enabled",GetSoundEnabled(VA_VOICES));
	save.SaveUint32("voice_volume",GetVolumeSound(VA_VOICES));
	save.SaveUint32("se_enabled",GetSoundEnabled(VA_SOUNDS));
	save.SaveUint32("se_volume",GetVolumeSound(VA_SOUNDS));

	// Store unlocked resources
	Uint8 *buffer;
	Uint32 length;
	if(cgflags.GetBuffer(&buffer,&length)){
		save.SaveBuffer("cgflags",buffer,length);
	}
	if(sceneflags.GetBuffer(&buffer,&length)){
		save.SaveBuffer("hflags",buffer,length);
	}
	return save.Write();
}

bool EngineVN::EventLoadSystem(){
	Savegame load(NativeID(),999);
	if(load.Read()){
		// Load settings
		Uint32 tmp;
		load.LoadUint32("skipmode",&tmp);
		skip=tmp;
		load.LoadUint32("fullscreen",&tmp);
		SetFullscreen(tmp);
		load.LoadUint32("msgspeed_enabled",&tmp);
		SetMessageDelayEnabled(tmp);
		load.LoadUint32("msgspeed_delay",&tmp);
		SetMessageDelayInterval(tmp);
		load.LoadUint32("bgm_enabled",&tmp);
		SetMusicEnabled(tmp);
		load.LoadUint32("bgm_volume",&tmp);
		SetVolumeMusic(tmp);
		load.LoadUint32("voice_enabled",&tmp);
		SetSoundEnabled(VA_VOICES,tmp);
		load.LoadUint32("voice_volume",&tmp);
		SetVolumeSound(VA_VOICES,tmp);
		load.LoadUint32("se_enabled",&tmp);
		SetSoundEnabled(VA_SOUNDS,tmp);
		load.LoadUint32("se_volume",&tmp);
		SetVolumeSound(VA_SOUNDS,tmp);
		load.LoadString("savename",&savename);

		// Load previously unlocked features
		Uint8 *buffer;
		Uint32 length;
		if(load.LoadBuffer("cgflags",&buffer,&length)){
			cgflags.SetBuffer(buffer,length);
			delete [] buffer;
		}
		if(load.LoadBuffer("hflags",&buffer,&length)){
			sceneflags.SetBuffer(buffer,length);
			delete [] buffer;
		}

		return true;
	}
	else{
		return false;
	}
}

void EngineVN::EventNew(){
}

bool EngineVN::EventSave(int Index){
	return false;
}

bool EngineVN::EventLoad(int Index){
	return false;
}

void EngineVN::EventSelect(int Selection){
}

bool EngineVN::EventBackgroundMouseLeftDown(int X,int Y){
	if(skip){
		SetSkipmode(false);
	}
	keyok=true;
	return true;
}

bool EngineVN::EventBackgroundMouseRightDown(int X,int Y){
	if(skip){
		SetSkipmode(false);
	}
	keycancel=true;
	return true;
}

bool EngineVN::EventBackgroundKeyDown(SDL_Keycode Key){
	if(skip){
		SetSkipmode(false);
	}
	switch(Key){
		case SDLK_RETURN:
		case SDLK_KP_ENTER:
		case SDLK_SPACE:	keyok=true;			break;
		case SDLK_ESCAPE:	keycancel=true;		break;
		case SDLK_RCTRL:
		case SDLK_LCTRL:						break;
		case SDLK_LEFT:
		case SDLK_UP:
		case SDLK_DOWN:
		case SDLK_RIGHT:						break;
		default:								break;
	}
	return true;
}

bool EngineVN::EventBackgroundKeyUp(SDL_Keycode Key){
	switch(Key){
		case SDLK_RETURN:
		case SDLK_KP_ENTER:
		case SDLK_SPACE:						break;
		case SDLK_ESCAPE:						break;
		case SDLK_RCTRL:
		case SDLK_LCTRL:						break;
		case SDLK_LEFT:
		case SDLK_UP:
		case SDLK_DOWN:
		case SDLK_RIGHT:						break;
		default:								break;
	}
	return true;
}

/*! \brief Registers a lockable resource name for an index
 *  \param Index Index of resource name
 *  \param Name Name of the resource
 */
void EngineVN::RegisterCG(int Index,uString Name){
	cgmap.SetString(Index,Name);
}

/*! \brief Registers a lockable resource name for an index
 *  \param Index Index of resource name
 *  \param Name Name of the resource
 */
void EngineVN::RegisterScene(int Index,uString Name){
	scenemap.SetString(Index,Name);
}

/*! \brief Reads data from a mapped image resource
 *  \param Index Index to get data from
 *  \param Flag True if resource has been unlocked (/seen)
 *  \param Name Name of the resource
 *  \return True if data was found for Index
 *
 *  This method can be used to construct CG-Room features. The resources
 *  should be unlocked by calling UnlockCG() when loading event cgs.
 */
bool EngineVN::MapCG(int Index,bool *Flag,uString *Name){
	bool retval=Index<cgmap.GetCount();
	if(retval){
		*Flag=cgflags.GetBit(Index);
		*Name=cgmap.GetString(Index);
	}
	return retval;
}

/*! \brief Reads data from a mapped script resource
 *  \param Index Index to get data from
 *  \param Flag True if resource has been unlocked (/read)
 *  \param Name Name of the resource
 *  \return True if data was found for Index
 *
 *  This method can be used to construct Memory Room features. The resources
 *  should be unlocked by calling UnlockScene() when loading scripts.
 */

bool EngineVN::MapScene(int Index,bool *Flag,uString *Name){
	bool retval=Index<scenemap.GetCount();
	if(retval){
		*Flag=sceneflags.GetBit(Index);
		*Name=scenemap.GetString(Index);
	}
	return retval;
}

/*! \brief Unlocks a CG (So it can be seen in a cg-room feature)
 *  \param Name Name of the resource to unlock
 */
void EngineVN::UnlockCG(uString Name){
	int index=cgmap.GetString(EDL_Searchname(Name));
	if(index>=0){
		cgflags.SetBit(index,true);
	}
}

/*! \brief Unlocks a CG (So it can be seen in a cg-room feature)
 *  \param Name Name of the resource to unlock
 */
void EngineVN::UnlockScene(uString Name){
	int index=scenemap.GetString(EDL_Searchname(Name));
	if(index>=0){
		sceneflags.SetBit(index,true);
	}
}

