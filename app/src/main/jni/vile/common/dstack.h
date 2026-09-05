/*! \class DStack
 *  \brief Simple stacking mechanism
 */
#ifndef _DSTACK_H_
#define _DSTACK_H_

#include <SDL.h>
#include "log.h"

class DStack {
	private:
		struct STACK_TYPE {
			void *Object;
			STACK_TYPE *Nextptr;
		}Head;
	public:
		DStack();
		~DStack();
		int Count();
		void Push(void *Object);
		void Queue(void *Object);
		void *Get(int Index);
		void *Drop(int Index);
		void *Pop();
		void *Peek();
		void Flush();
};

#endif

