/*! \class Cfg
 *  \brief Global configuration
 *
 *  Collects and categorizes the common configuration (eg. across titles).
 *  The defaults are set in Cfg.cpp and can typically be overridden by the 
 *  commandline handler.
 *
 *  Game options such as volume settings are stored in the savegames.
 *
 */
#ifndef _CFG_H_
#define _CFG_H_

#include <SDL.h>
#include "ustring.h"

class Cfg {
	public:
		// Settings for the physical display
		class Display {
			public:
				static int Width;					//!< Display width
				static int Height;					//!< Display height
				static int Depth;					//!< Display depth
				static int Flags;					//!< SDL flags for display
		};

		// Audio settings
		class Audio {
			public:
				static bool Enabled;				//!< Wether to load audio
				static int Frequency;				//!< Samples per second
				static int Buffersize;				//!< Samples per buffer
				static int Buffercount;				//!< Frames in buffer
				static int Channels;				//!< Number of channels
				static int CDROM;					//!< CD drive (-1 for none)
				static uString Soundfont;		//!< Default soundfont
		};

		// Video configuration
		class Video {
			public:
				static bool Enabled;				//!< Wether to load video
				static bool Overlay;				//!< Use overlayed video
				static int Framecount;				//!< Frames in buffer
		};

		// Generic system settings
		class System {
			public:
				static uString Logfile;				//!< Output file
				static uString Keyfile;				//!< Script encryption file
				static bool Logcolor;				//!< Print colored messages
				static bool Verbose;				//!< Print verbose messages
				static bool Mainmenu;				//!< Load main menu
				static int Framerate;				//!< Limits fps
		};

		// Paths to key files and directories
		class Path {
			public:
				static uString cwd;					//!< Working directory
				static uString game;				//!< Game resource folder
				static uString save;				//!< Savegame folder
				static uString config;				//!< Configuration file
				static uString resource;			//!< Resource file
		};

		// Font configuration
		class Font {
			public:
				// Default for anything else
				static uString default_face;		//!< Default font file
				static int default_size;			//!< Default font size
				static int default_style;			//!< Default font style
		};

		class Color {
			public:
				static Uint32 DialogBackground;		//!< Dialog background
				static Uint32 DialogHeader;			//!< Header font color
				static Uint32 DialogTopic;			//!< Subheader font color
				static Uint32 WidgetBackground;		//!< widget background
				static Uint32 WidgetFont;			//!< widget font color
		};
};

#endif

