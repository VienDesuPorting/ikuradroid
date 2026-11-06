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

#include "emixer.h"

#include <string.h>

#ifdef ANDROID
extern "C"
{
#include <jni.h>
bool mixOpened = true;
JNIEXPORT void JNICALL Java_org_libsdl_app_SDLActivity_pauseMixer(JNIEnv* env, jclass cls)
{
	if(mixOpened)
	{
		if (Mix_PlayingMusic())
			Mix_PauseMusic();
		for(int i=0;i<AUDIO_CHANNELS;i++)
		{
			if (Mix_Playing(i))
				Mix_Pause(i);
		}
	}
}
JNIEXPORT void JNICALL Java_org_libsdl_app_SDLActivity_resumeMixer(JNIEnv* env, jclass cls)
{
	if(mixOpened)
	{
		if (Mix_PausedMusic())
			Mix_ResumeMusic();

		for(int i=0;i<AUDIO_CHANNELS;i++)
		{
			if (Mix_Paused(i))
				Mix_Resume(i);
		}
	}
}
}
#endif


EngineMixer::EngineMixer(int Width,int Height) : EngineBase(Width,Height){
	// Open the audio device
	if(!Cfg::Audio::Enabled){
		LogVerbose("Audio is disabled");
	}
	else //if(SDL_OpenAudio(&wanted_spec,&hardware)<0){
    if( Mix_OpenAudio( Cfg::Audio::Frequency, MIX_DEFAULT_FORMAT, Cfg::Audio::Channels, Cfg::Audio::Buffersize ) == -1 ){
		LogError("Couldn't init audio: %s",Mix_GetError());
		//Cfg::Audio::Enabled=false;
	}
}

EngineMixer::~EngineMixer(){

	if(Cfg::Audio::Enabled){
		LogVerbose("Closing down audio mixer");
		StopMusic();
		StopSound(-1);
	}
	 Mix_CloseAudio();
#ifdef ANDROID
	 mixOpened = false;
#endif
}

EngineMixer::MusicChannel::MusicChannel(){
	music=0;
	buffer = 0;
	ops= 0;
	volume=1;
	enabled=true;
}

EngineMixer::MusicChannel::~MusicChannel(){
    Mix_HaltMusic();
	Mix_FreeMusic(music);
	free(buffer);
    buffer = 0;
    SDL_FreeRW(ops);
    ops= 0;
}

EngineMixer::SoundChannel::SoundChannel(){
	sound=0;
	buffer = 0;
	ops= 0;
	volume=1;
	enabled=true;
}

EngineMixer::SoundChannel::~SoundChannel(){
//        Mix_HaltChannel(-1);
		Mix_FreeChunk(sound);
        free(buffer);
        buffer = 0;
        SDL_FreeRW(ops);
        ops= 0;
}


/*! \brief Play CDDA track
 *  \param Track Track to play
 *  \return True if track could be played back
 *
 *  This method will play back a track using Cfg::Audio::CDROM. Negative
 *  values effectively disables cdda, null selects the first physical cdrom
 *  drive.
 */
bool EngineMixer::PlayCDDA(int Track){
	bool retval=false;
	return retval;
}

/*! \brief Validates that SDL_mixer can honestly decode a music blob
 *  \param Data Raw audio data (whole resource in memory)
 *  \param Size Data size in bytes
 *  \return True if the bundled SDL_mixer can decode this format
 *
 * SDL_mixer 2.0.1 falls back to SMPEG for any blob that starts with an
 * MPEG audio sync pattern, and SMPEG only decodes MPEG layer I/II. Rips
 * named .wav but actually carrying MPEG layer III (or exotic RIFF
 * formats the wavestream decoder rejects) therefore start playing as
 * loud white noise. Identify the format positively and refuse what
 * cannot be decoded, so the device log reports the file's real format
 * instead of the speakers emitting static.
 */
static bool MusicFormatSupported(const Uint8 *Data,int Size){
	if(Size<8){
		return false;
	}
	// RIFF/WAVE: the wavestream decoder only handles PCM 8/16 bit
	// and IMA ADPCM; anything else falls through to SMPEG and misplays
	if(!memcmp(Data,"RIFF",4) && !memcmp(Data+8,"WAVE",4)){
		int pos=12;
		while(pos+8<=Size){
			Uint32 clen=Data[pos+4]|(Data[pos+5]<<8)|
						(Data[pos+6]<<16)|((Uint32)Data[pos+7]<<24);
			if(!memcmp(Data+pos,"fmt ",4)){
				if(pos+24>Size){
					LogError("BGM: truncated RIFF/WAVE format chunk");
					return false;
				}
				Uint16 tag=Data[pos+8]|(Data[pos+9]<<8);
				Uint16 ch=Data[pos+10]|(Data[pos+11]<<8);
				Uint32 rate=Data[pos+12]|(Data[pos+13]<<8)|
								(Data[pos+14]<<16)|((Uint32)Data[pos+15]<<24);
				Uint16 bits=Data[pos+20]|(Data[pos+21]<<8);
				if((tag==1 && (bits==8||bits==16)) || tag==0x11){
					LogVerbose("BGM: RIFF/WAVE tag=%u bits=%u ch=%u rate=%u",
									tag,bits,ch,rate);
					return true;
				}
				LogError("BGM: RIFF/WAVE fmt=%u bits=%u ch=%u rate=%u "
								"is not PCM 8/16-bit or IMA ADPCM; refusing",
								tag,bits,ch,rate);
				return false;
			}
			pos+=8+(int)((clen+1)&~1);
		}
		LogError("BGM: RIFF/WAVE without a format chunk; refusing");
		return false;
	}
	// Plain formats SDL_mixer identifies by magic on sight
	if(!memcmp(Data,"OggS",4) || !memcmp(Data,"MThd",4) ||
							!memcmp(Data,"IMPM",4) || !memcmp(Data,"PSID",4) ||
							!memcmp(Data,"Extended Module: ",16)){
		return true;
	}
	// Screamtracker files carry their magic at offset 0x2C
	if(Size>0x30 && !memcmp(Data+0x2C,"SCRM",4)){
		return true;
	}
	// Protracker-style modules carry their magic at offset 108
	if(Size>112){
		static const char *mods[]={"M.K.","M!K!","FLT4","FLT8",
					"4CHN","6CHN","8CHN","10CH","12CH","14CH",
					"16CH","18CH","20CH","24CH","28CH","30CH",
					"32CH","CD81","OKTA"};
		for(unsigned int i=0;i<sizeof(mods)/sizeof(mods[0]);i++){
			if(!memcmp(Data+108,mods[i],4)){
				return true;
			}
		}
	}
	// MPEG audio: SMPEG decodes layer I/II only. Layer III and
	// reserved header combinations render as white noise.
	if(Data[0]==0xFF && (Data[1]&0xE0)==0xE0){
		Uint8 version=(Data[1]&0x18)>>3;
		Uint8 layer=(Data[1]&0x06)>>1;
		Uint8 srate=(Data[2]&0x0C)>>2;
		Uint8 bitrate=(Data[2]&0xF0)>>4;
		if(version==1 || layer==0 || bitrate==0 || bitrate==15 || srate==3){
			LogError("BGM: invalid MPEG header (version=%u layer=%u "
							"bitrate=%u rate=%u); refusing",
							version,layer,bitrate,srate);
			return false;
		}
		if(layer==1){
			LogError("BGM: MPEG layer III audio; the bundled SMPEG only "
							"decodes layer I/II and plays this as white noise - "
							"re-rip the track as PCM WAV or OGG");
			return false;
		}
		return true;
	}
	// Unknown container: leave the decision to SDL_mixer (it fails
	// cleanly with an error log when no decoder claims the data)
	return true;
}

/*! \brief Start playing a music score
 *  \param Music Musical score to play back
 *  \return True if track wachannel->s successfully loaded
 *
 *  Currently playing tracks are automatically faded out which will halt
 *  execution for some milliseconds whenever applicable.
 */
bool EngineMixer::PlayMusic(RWops *Music){
	// Try to load music resource

	bool retval=false;
	if(musicbuffer.enabled && Cfg::Audio::Enabled){
		StopMusic();
		if(Music && Music->Size()>8){

            int size=Music->Size();
            musicbuffer.buffer=new Uint8[size];
			Music->Seek(0,SEEK_SET);
			Music->Read(musicbuffer.buffer,size);
			// Refuse formats the mixer stack cannot decode honestly
			// (SMPEG renders unknown data as white noise)
			if(!MusicFormatSupported(musicbuffer.buffer,size)){
				delete[] musicbuffer.buffer;
				musicbuffer.buffer=0;
				return false;
			}
			musicbuffer.ops = SDL_RWFromMem(musicbuffer.buffer,size);
			musicbuffer.music  = Mix_LoadMUS_RW(musicbuffer.ops,0);


           // musicbuffer.music  = Mix_QuickLoad_WAV(musicbuffer.buffer);
           // musicbuffer.music = Mix_LoadMUS_RW(Music->ops);

			if(musicbuffer.music){
				if( Mix_PlayMusic( musicbuffer.music, -1 ) != -1 )
                    retval=true;
			}
			else{
				LogError("Couldn't open music: %s",Mix_GetError());
				// The failed load leaves nothing playing, so the
				// next StopMusic would skip the cleanup and leak
				SDL_FreeRW(musicbuffer.ops);
				musicbuffer.ops=0;
				delete[] musicbuffer.buffer;
				musicbuffer.buffer=0;
			}
		}
	}

	return retval;
}
/*! \brief Stops current music playback
 */
void EngineMixer::StopMusic(){
	if(musicbuffer.music){
        Mix_HaltMusic();
		Mix_FreeMusic(musicbuffer.music);
		musicbuffer.music=0;
        free(musicbuffer.buffer);
        musicbuffer.buffer = 0;
        SDL_FreeRW(musicbuffer.ops);
        musicbuffer.ops= 0;
	}
}

/*! \brief Plays sound in specified channel
 *  \param Sound Sound resource to decode and play
 *  \param Channel Audio channel
 *  \param Replays Number of times to play chunk
 *  \return True if the sound resource was started without problems
 */
bool EngineMixer::PlaySound(RWops *Sound,int Channel,int Replays){
	bool retval=false;
	//*

	if(Channel>=0 && Channel<AUDIO_CHANNELS){

		if(soundbuffer[Channel].enabled && Cfg::Audio::Enabled){

			StopSound(Channel);
            int size=Sound->Size();
            soundbuffer[Channel].buffer=new Uint8[size];
			Sound->Seek(0,SEEK_SET);
			Sound->Read(soundbuffer[Channel].buffer,size);
            soundbuffer[Channel].ops = SDL_RWFromMem(soundbuffer[Channel].buffer,size);
			soundbuffer[Channel].sound = Mix_LoadWAV_RW(soundbuffer[Channel].ops, 0);
           // soundbuffer[Channel].sound =  Mix_QuickLoad_RAW(soundbuffer[Channel].buffer,size);
			if(soundbuffer[Channel].sound){
				if( Mix_PlayChannel(Channel, soundbuffer[Channel].sound, Replays ) != -1 )
                    retval=true;
			}
			else LogError("Couldn't open sound: %s",Mix_GetError());
		}

	}//*/
	return retval;
}

/*! \brief Stops given audio channel
 *  \param Channel Channel to stop
 */
void EngineMixer::StopSound(int Channel){
	if(Channel>=0 && Channel<AUDIO_CHANNELS){
		if(soundbuffer[Channel].sound){
            Mix_HaltChannel(Channel);
			Mix_FreeChunk(soundbuffer[Channel].sound);
			soundbuffer[Channel].sound=0;
			free(soundbuffer[Channel].buffer);
			soundbuffer[Channel].buffer=0;
            SDL_FreeRW(soundbuffer[Channel].ops);
            soundbuffer[Channel].ops= 0;
		}
	}
	else if(Channel<0){
        Mix_HaltChannel(-1);
		for(int i=0;i<AUDIO_CHANNELS;i++){
            Mix_FreeChunk(soundbuffer[i].sound);
			soundbuffer[i].sound=0;
			free(soundbuffer[i].buffer);
			soundbuffer[i].buffer=0;
            SDL_FreeRW(soundbuffer[i].ops);
            soundbuffer[i].ops= 0;
		}
	}
}

/*! \brief Pauses background music
 */
void EngineMixer::PauseMusic(){
	if(musicbuffer.music)
		Mix_PauseMusic();
}

/*! \brief Resumes background music
 */
void EngineMixer::ResumeMusic(){
	if(musicbuffer.enabled && musicbuffer.music){
        if( Mix_PausedMusic() == 1 )
                            Mix_ResumeMusic();

	}
}

/*! \brief Enables or disables background music
 *  \param Enabled True to enable
 */
void EngineMixer::SetMusicEnabled(bool Enabled){

	musicbuffer.enabled=Enabled;
	if (!musicbuffer.enabled)
        PauseMusic();
    else
        ResumeMusic();
}

/*! \brief Wether background music is enabled
 *  \return True if music is enabled
 *
 *  Background music will not load or decode at all unless it is enabled. All
 *  other bgm controls will be ignored if the music mixer is disabled.
 */
bool EngineMixer::GetMusicEnabled(){

	bool retval=musicbuffer.enabled;

	return retval;
}

/*! \brief Enable or disable a given audio channel
 *  \param Channel Audio channel to set
 *  \param Enabled True to enable channel
 *
 *  A disabled channel will refuse to load any audio resources, all other
 *  audio controls will ignore that channel as long as it is disabled.
 */
void EngineMixer::SetSoundEnabled(int Channel,bool Enabled){
	if(Channel>=0 && Channel<AUDIO_CHANNELS){
		soundbuffer[Channel].enabled=Enabled;

		if (!soundbuffer[Channel].enabled)
            PauseSound(Channel);
        else
            ResumeSound(Channel);
	}
}

void EngineMixer::PauseSound(int Channel)
{
	if(soundbuffer[Channel].sound)
		Mix_Pause(Channel);
}

void EngineMixer::ResumeSound(int Channel)
{
	if(soundbuffer[Channel].enabled && soundbuffer[Channel].sound){
        if( Mix_Paused(Channel) == 1 )
                Mix_Resume(Channel);
	}
}
/*! \brief Wether an audio channel is enabled
 *  \param Channel Audio channel to check
 *  \return True if channel is enabled
 */
bool EngineMixer::GetSoundEnabled(int Channel){
	bool retval=false;
	if(Channel>=0 && Channel<AUDIO_CHANNELS){
		retval=soundbuffer[Channel].enabled;
	}
	return retval;
}

/*! \brief Set volume for background music
 *  \param Volume Integer from 0 to 100
 *  \return Always true
 */
bool EngineMixer::SetVolumeMusic(int Volume){
	musicbuffer.volume=Volume/100.0;
	Mix_VolumeMusic(Volume);
	return true;
}

/*! \brief Set volume for a given audio channel
 *  \param Channel Audio channel to set
 *  \param Volume Integer from 0 to 100
 *  \return True if channel was set
 */
bool EngineMixer::SetVolumeSound(int Channel,int Volume){
	bool retval=false;
	if(Channel>=0 && Channel<AUDIO_CHANNELS){
		soundbuffer[Channel].volume=Volume/100.0;
		Mix_Volume(Channel,Volume);
		retval=true;
	}
	return retval;
}

/*! \brief Get volume for background music
 *  \return Integer from 0 to 100
 */
int EngineMixer::GetVolumeMusic(){
	int retval=0;
	retval=musicbuffer.volume*100;
	return retval;
}

/*! \brief Get volume for a given audio channel
 *  \param Channel Audio channel to set
 *  \return Integer from 0 to 100 (0 if channel was invalid)
 */
int EngineMixer::GetVolumeSound(int Channel){
	int retval=0;
	if(Channel>=0 && Channel<AUDIO_CHANNELS){
		retval=soundbuffer[Channel].volume*100;
	}
	return retval;
}
