/*! \class EngineMixer
 *  \brief Embeds audio playback and decoding to the engine object
 *
 *  This class encapsulates SDL_mixer functionality and facilitates music,
 *  voice and sfx clips to be played. You can use as many channels as you
 *  want to, but only one clip per channel at a time.
 *
 *  This class is basically a driver class for the decoder implementations
 *  in the media/ folder (See Audio class). We currently support FFMPEG and
 *  Fluidsynth which should serve most purposes on desktop computers atleast.
 */

#ifndef _EMIXER_H_
#define _EMIXER_H_

#include "ebase.h"
#include "../media/audio.h"

#define AUDIO_CHANNELS	32

#include "../common/config.h"

#include "SDL_mixer.h"

class EngineMixer : public EngineBase
{
	private:
		// Declare Audio Channels
		struct MusicChannel {
			MusicChannel();
			~MusicChannel();
			Mix_Music *music;
			SDL_RWops *ops;
			Uint8 *buffer;
			bool enabled;
			double volume;
		}musicbuffer;


		struct SoundChannel {
			SoundChannel();
			~SoundChannel();
			Mix_Chunk *sound;
			SDL_RWops *ops;
			Uint8 *buffer;
			bool enabled;
			double volume;
		}soundbuffer[AUDIO_CHANNELS];


	public:
		EngineMixer(int Width,int Height);
		~EngineMixer();

		// Mixer API
		bool PlayCDDA(int Track);
		bool PlayMusic(RWops *Music);
		void PauseMusic();
		void ResumeMusic();
        void PauseSound(int Channel=-1);
		void ResumeSound(int Channel=-1);
		void StopMusic();
		bool SetVolumeMusic(int Volume);
		int GetVolumeMusic();
		bool PlaySound(RWops *Sound,int Channel,int Replays=0);
		void StopSound(int Channel=-1);
		bool SetVolumeSound(int Channel,int Volume);
		int GetVolumeSound(int Channel);

		// Disable or enable audio (ie. muting)
		void SetMusicEnabled(bool Enabled);
		bool GetMusicEnabled();
		void SetSoundEnabled(int Channel,bool Enabled);
		bool GetSoundEnabled(int Channel);
};

#endif


