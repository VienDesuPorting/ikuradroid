/*! \class DArray
 *  \brief Dynamic array that grows as needed
 */
#ifndef _DARRAY_H_
#define _DARRAY_H_

#include "dvector.h"

class DArray {
	private:
		DVector **vectors;
		int len;
		void assertvector(int Index);
	public:
		DArray(const DArray &Src);
		DArray(int Width=0,int Height=0);
		~DArray();
		DArray& operator=(const DArray &Src);
		void SetUint32(int Index,int SubIndex,Uint32 Value);
		void SetUint16(int Index,int SubIndex,Uint16 Value);
		void SetUint8(int Index,int SubIndex,Uint8 Value);
		void SetSint32(int Index,int SubIndex,Sint32 Value);
		void SetSint16(int Index,int SubIndex,Sint16 Value);
		void SetSint8(int Index,int SubIndex,Sint8 Value);
		void SetBit(int Index,int SubIndex,bool Value);
		Uint32 GetUint32(int Index,int SubIndex);
		Uint16 GetUint16(int Index,int SubIndex);
		Uint8 GetUint8(int Index,int SubIndex);
		Sint32 GetSint32(int Index,int SubIndex);
		Sint16 GetSint16(int Index,int SubIndex);
		Sint8 GetSint8(int Index,int SubIndex);
		bool GetBit(int Index,int SubIndex);
};

#endif
