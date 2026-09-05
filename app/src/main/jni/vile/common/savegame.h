/*! \class Savegame
 *  \brief Handles loading and saving to disk
 *  \todo Make Savegame class UNICODE compliant
 *
 *  The base savegame format is designed like a sequential archive. Each
 *  stored resource starts with a header that provides a resource name,
 *  start- and end offsets (Relative to start of file) as well as the
 *  resource itself.
 *
 *  Surfaces were originally stored as bitmaps, but this was changed in
 *  version 0.4.10 to store surfaces with zlib compression in order to
 *  conserve space and alpha data.
 */
#ifndef _SAVEGAME_H_
#define _SAVEGAME_H_

#include <SDL.h>
//#include <SDL_rotozoom.h>
#include <zlib.h>
#include "log.h"
#include "dvector.h"
#include "stringlist.h"
#include "edl_gfx.h"

#define SNLENGTH			32				//!< Length of resource names
#define SHLENGTH			(SNLENGTH+8)	//!< Length of resource header
using namespace std;

class Savegame {
	private:
		uString filename;
		Uint8 *buffer;
		unsigned int length;
		bool write(uString Name,Uint32 Pos,Uint8 *Buffer,Uint32 Length);
		bool search(uString Name,Uint32 *Start,Uint32 *End);
	public:
		Savegame(uString Gamecode,int Index);
		~Savegame();

		// Resolve filenames
		static uString Savename(uString GameID,int Index);

		// File interface
		bool Write();
		bool Read();
		void Clear();
		void Delete();
		bool Exists();

		// Value interface
		void SaveBuffer(uString Name,Uint8 *Buffer,Uint32 Length);
		void SaveSurface(uString Name,SDL_Surface *Surface,int W=0,int H=0);
		void SaveVector(uString Name,DVector *Vector);
		void SaveStringList(uString Name,Stringlist *List);
		void SaveString(uString Name,uString Value);
		void SaveUint8(uString Name,Uint8 Value);
		void SaveUint16(uString Name,Uint16 Value);
		void SaveUint32(uString Name,Uint32 Value);
		bool LoadBuffer(uString Name,Uint8 **Buffer,Uint32 *Length);
		bool LoadSurface(uString Name,SDL_Surface **Surface,int W=0,int H=0);
		bool LoadVector(uString Name,DVector *Vector);
		void LoadStringList(uString Name,Stringlist *List);
		bool LoadString(uString Name,uString *Value);
		bool LoadUint8(uString Name,Uint8 *Value);
		bool LoadUint16(uString Name,Uint16 *Value);
		bool LoadUint32(uString Name,Uint32 *Value);
};

#endif

