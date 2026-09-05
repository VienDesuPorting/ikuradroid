/*! \class RWops
 *  \brief Manages a blob of data while obfuscating the underlying hardware
 */
#ifndef _RWOPS_H_
#define _RWOPS_H_

#include "../common/log.h"
#include "../common/edl_fs.h"

class RWops {
	private:
		enum RWOPS_TYPE {			//!< Defines type of data
			RW_NONE=0,				//!< No valid data
			RW_RWBUFFER,			//!< Data is in R/W ram
			RW_ROBUFFER,			//!< Data is in R/O ram
			RW_FILE					//!< Data is stored in a file
		} type;						//!< Type of this object
		void *buffer;				//!< Buffer for memory blobs
		int offset;					//!< Offset in stubbed blobs
		int size;					//!< Size of the available data
		uString filename;			//!< Filename for fileblobs
	public:
		// Oh constructors, thou art why I love oop.
		RWops();
		~RWops();

		// Standard RW interface
		bool OpenRW(RWops *Ops,bool FreeSource);
		bool OpenRW(RWops *Ops,int Offset,int Size,bool FreeSource);
		bool OpenFile(const char *Filename,int Offset,int Size);
		bool OpenFile(const char *Filename,const char *Filemode);
		bool OpenROM(const char *Memory,int Size,bool FreeSource);
		bool OpenRAM(char *Memory,int Size,bool FreeSource);
		int Read(void *Data,int Size);
		int Write(void *Data,int Size);
		int Seek(int Offset,int Whence);
		int Size();
		int Tell();
		void Close();

		//! Underlying rwops structure
		SDL_RWops *ops;
};

#endif

