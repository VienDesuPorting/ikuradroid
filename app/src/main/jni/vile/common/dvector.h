/*! \class DVector
 *  \brief Vector that grows to whatever size you want it to
 *
 *  Please note that each vector object have a single datatype, if you
 *  use it to store 8bit variables you should be very carefull extracting
 *  32bit values from it as you might not get what you expect.
 */
#ifndef _DVECTOR_H_
#define _DVECTOR_H_

#include <SDL.h>
#include "log.h"

class DVector {
	private:
		Uint8 *reg;
		Uint32 len;
	public:
		DVector(const DVector &Src);
		DVector(int Preload=0);
		~DVector();

		// Normal value API
		void SetUint32(Uint32 Pos,Uint32 Value);
		void SetUint16(Uint32 Pos,Uint16 Value);
		void SetUint8(Uint32 Pos,Uint8 Value);
		void SetSint32(Uint32 Pos,Sint32 Value);
		void SetSint16(Uint32 Pos,Sint16 Value);
		void SetSint8(Uint32 Pos,Sint8 Value);
		void SetBit(Uint32 Pos,bool Value);
		Uint32 GetUint32(Uint32 Pos);
		Uint16 GetUint16(Uint32 Pos);
		Uint8 GetUint8(Uint32 Pos);
		Sint32 GetSint32(Uint32 Pos);
		Sint16 GetSint16(Uint32 Pos);
		Sint8 GetSint8(Uint32 Pos);
		bool GetBit(Uint32 Pos);
		void Clear();

		// Special operations API
		DVector & operator=(const DVector &Src);
		bool GetBuffer(Uint8 **Buffer,Uint32 *Length);
		bool SetBuffer(Uint8 *Buffer,Uint32 Length);

};

#endif

