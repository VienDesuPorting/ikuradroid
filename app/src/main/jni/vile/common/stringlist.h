/*! \class Stringlist
 *  \brief Manages an indexed list of strings
 */
#ifndef _STRINGLIST_H_
#define _STRINGLIST_H_

#include <SDL.h>
#include "log.h"

class Stringlist {
	private:
		uString *stringlist;
		int count;
	public:
		Stringlist(int Preload=0);
		Stringlist(const Stringlist &Source);
		Stringlist &operator=(const Stringlist &Source);
		~Stringlist();
		void SetString(int Pos,uString Text);
		void AddString(uString Text);
		void AddStringlist(Stringlist List);
		uString GetString(int Pos);
		int GetString(uString Text);
		bool GetString(int Pos,uString *Text);
		int GetCount() const;
		void Clear();
		uString Enumerate();
};

#endif

