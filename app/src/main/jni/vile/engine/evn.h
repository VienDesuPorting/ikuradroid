/*! \class EngineVN
 *  \brief Encapsulates the base classes into a VN-friendly API
 *
 *  EngineVN forms a common high level API for specialized Game engines to
 *  derive from. It helps manage and using game resources as well as providing
 *  standard VN functionality such as skip mode to derived engines.
 */
#ifndef _EVN_H_
#define _EVN_H_

// Include base classes
#include "evideo.h"
#include "sequencer.h"

// Include resource widgets
#include "../widgets/mwidget.h"
#include "../widgets/vwidget.h"


enum VN_AUDIOCHANNEL {				//!< Default audio channel enumeration
	VA_ALL=-1,						//!< All audio channels (Not music)
	VA_SYSTEM=0,					//!< Default channel for system sounds
	VA_SOUNDS,						//!< Default channel for game effects
	VA_VOICES						//!< Default channel for voices
};

class EngineVN : public EngineVideo {
	private:
		// Game resources
		Resources r_se;				//!< Sound Effect resources
		Resources r_bgm;			//!< Background music resources
		Resources r_scripts;		//!< Script resources
		Resources r_videos;			//!< Video resources
		Resources r_images;			//!< Image resources
		Resources r_voices;			//!< Voice resources
		Resources r_other;			//!< Other resources

		// Variable data
		uString savename;		//!< Current savegame name
		bool skip;					//!< Skipmode state
		Stringlist cgmap;			//!< Maps cg resources for cgroom
		Stringlist scenemap;		//!< Maps story resources for memory
		DVector cgflags;			//!< Flags seen graphics
		DVector sceneflags;			//!< Flags read stories

        Options *opt;
        StdHalt *Halt;
        StdSave *Sav;
        StdLoad *Loa;

		// Message delay
		bool msgdelay_enabled;		//!< Message delay activation
		int msgdelay_interval;		//!< Message delay value
	protected:
		// Track input
		bool keyok;					//!< ok (return etc) is registered
		bool keycancel;				//!< cancel (esc etc) is registered
		bool keyctrl();				//!< skip key is registered

		// Checks for presence of resource files
		static bool ProbeResource(uString Path,uString Resource);
		static bool ProbeString(uString S1,uString S2);
	public:
		// Constructor to keep parameters
		EngineVN(int Width,int Height);
		~EngineVN();

		// Resource handler
		int GetVoices(Stringlist *List);
		int GetScripts(Stringlist *List);
		int GetOther(Stringlist *List);
		int GetImages(Stringlist *List);
		int GetBGM(Stringlist *List);
		int GetSE(Stringlist *List);
		int GetVideo(Stringlist *List);
		void AddVoices(ArchiveBase *Archive);
		void AddScripts(ArchiveBase *Archive);
		void AddOther(ArchiveBase *Archive);
		void AddImages(ArchiveBase *Archive);
		void AddBGM(ArchiveBase *Archive);
		void AddSE(ArchiveBase *Archive);
		void AddVideo(ArchiveBase *Archive);
		void AddPatches(ArchiveBase *Archive);
		RWops *LoadScript(uString Name,uString Ext="");
		RWops *LoadOther(uString Name,uString Ext="");
		RWops *LoadMusic(uString Name,uString Ext="");
		RWops *LoadVoice(uString Name,uString Ext="");
		RWops *LoadSE(uString Name,uString Ext="");
		RWops *LoadVideo(uString Name,uString Ext="");
		SDL_Surface *LoadImage(uString Name,uString Ext="");
		SDL_Surface **LoadAnimation(uString N,uString E="");

		// Media handling
		virtual bool PlayVideo(uString Name);
		virtual bool PlayVideo(RWops *Video);
		virtual bool QueueVideo(uString Name);
		virtual bool QueueVideo(RWops *Video);
		virtual bool PlayMusic(uString Name);
		virtual bool PlayMusic(RWops *Music);
		virtual bool PlayTrack(int Track);
		virtual bool QueueMusic(uString Name);
		virtual bool QueueMusic(RWops *Music);
		virtual bool PlayVoice(uString Name,int Channel=VA_VOICES);
		virtual bool PlayVoice(RWops *Voice,int Channel=VA_VOICES);
		virtual bool PlaySound(uString Name,
				int Channel=VA_SOUNDS,int Repeat=0);
		virtual bool PlaySound(RWops *Voice,
				int Channel=VA_SOUNDS,int Repeat=0);

		// Variable data
		void SetSavename(uString Name);
		uString GetSavename();

		// Skipmode
		virtual void SetSkipmode(bool Skip);
		virtual bool GetSkipmode();

		// Maps seen/unseen resources
		void RegisterCG(int Index,uString Name);
		void RegisterScene(int Index,uString Name);
		bool MapCG(int Index,bool *Flag,uString *Name);
		bool MapScene(int Index,bool *Flag,uString *Name);
		void UnlockCG(uString Name);
		void UnlockScene(uString Name);

		// Non-persistent animation sequences
		Sequencer *Sequences;

		// Message delay
		virtual void SetMessageDelayEnabled(bool Enabled);
		virtual void SetMessageDelayInterval(int Interval);
		virtual bool GetMessageDelayEnabled();
		virtual int GetMessageDelayInterval();

		// Define Standard dialog events
		virtual void EventGameDialog(VN_DIALOGS Dialog);

		// Loading and saving
		virtual bool EventSaveSystem();
		virtual bool EventLoadSystem();
		virtual bool EventSave(int Index);
		virtual bool EventLoad(int Index);
		virtual void EventNew();
		virtual void EventSelect(int Selection);

		// Pick up background events
		virtual bool EventBackgroundMouseLeftDown(int X,int Y);
		virtual bool EventBackgroundMouseRightDown(int X,int Y);
		virtual bool EventBackgroundKeyDown(SDL_Keycode Key);
		virtual bool EventBackgroundKeyUp(SDL_Keycode Key);
};

#endif

