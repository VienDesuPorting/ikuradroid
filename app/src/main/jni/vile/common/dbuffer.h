/*! \class DBuffer
 *  \brief Stores and manages indexed binary buffers
 *
 *  This ugly little duckling is only used in Ikura GDLs IParser class, the
 *  real need should be reconsidered and reimplemented directly in said class.
 *
 *  To be deprecated.
 */

#ifndef _DBUFFER_H_
#define _DBUFFER_H_

#include <SDL.h>
#include "log.h"

class DBuffer {
	private:
		Uint8 **buffers;		//!< Holds stored buffers
		int *sizes;				//!< Holds sizes of stored buffers
		int size;				//!< Number of stored buffers
	public:
		DBuffer();
		~DBuffer();
		void SetBuffer(int Index,Uint8 *Buffer,int Size);
		bool GetBuffer(int Index,Uint8 **Buffer,int *Size);
};

#endif

