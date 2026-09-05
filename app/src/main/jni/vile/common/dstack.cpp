/*
 * ViLE - Visual Library Engine
 * Copyright (c) 2010-2011, ViLE Team (team@vilevn.org)
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "dstack.h"

DStack::DStack(){
	// Set defaults
	Head.Object=0;
	Head.Nextptr=0;
}

DStack::~DStack(){
	Flush();
}

void DStack::Flush(){
	// Delete stack objects (NOT objects!)
	while(Head.Nextptr){
		STACK_TYPE *tmpptr=Head.Nextptr->Nextptr;
		delete Head.Nextptr;
		Head.Nextptr=tmpptr;
	}
}


int DStack::Count(){
	int retval=0;
	STACK_TYPE *tmpptr=&Head;
	while(tmpptr->Nextptr){
		tmpptr=tmpptr->Nextptr;
		retval++;
	}
	return retval;
}

/*! \brief Place an object at the top of the stack
 *  \param Object to put in the stack
 */
void DStack::Push(void *Object){
	STACK_TYPE *tmpptr=Head.Nextptr;
	Head.Nextptr=new STACK_TYPE;
	Head.Nextptr->Nextptr=tmpptr;
	Head.Nextptr->Object=Object;
}

/*! \brief Place an object at the bottom of the stack
 *  \param Object to put in the stack
 */
void DStack::Queue(void *Object){
	STACK_TYPE *tmpptr=&Head;
	while(tmpptr->Nextptr){
		tmpptr=tmpptr->Nextptr;
	}
	tmpptr->Nextptr=new STACK_TYPE;
	tmpptr->Nextptr->Nextptr=0;
	tmpptr->Nextptr->Object=Object;
}

/*! \brief Finds the top object on the stack
 *  \return Found object
 */
void *DStack::Peek(){
	void *retval=0;
	if(Head.Nextptr){
		retval=Head.Nextptr->Object;
	}
	return retval;
}

/*! \brief Finds an indexed object on the stack
 *  \return Found object
 */
void *DStack::Get(int Index){
	void *retval=0;
	STACK_TYPE *tmpptr=Head.Nextptr;
	for(int i=0;i<Index && tmpptr;i++){
		tmpptr=tmpptr->Nextptr;
	}
	if(tmpptr){
		retval=tmpptr->Object;
	}
	return retval;
}

void *DStack::Drop(int Index){
	void *retval=0;
	if(Index){
		STACK_TYPE *tmpptr=Head.Nextptr;
		for(int i=1;i<Index && tmpptr;i++){
			tmpptr=tmpptr->Nextptr;
		}
		if(tmpptr && tmpptr->Nextptr){
			STACK_TYPE *swapptr=tmpptr->Nextptr->Nextptr;
			retval=tmpptr->Nextptr->Object;
			delete tmpptr->Nextptr;
			tmpptr->Nextptr=swapptr;
		}
	}
	else if(Head.Nextptr){
		STACK_TYPE *tmpptr=Head.Nextptr->Nextptr;
		retval=Head.Nextptr->Object;
		delete Head.Nextptr;
		Head.Nextptr=tmpptr;
		
	}
	return retval;
}


/*! \brief Finds and removes the top object on the stack
 *  \return Removed object
 */
void *DStack::Pop(){
	void *retval=0;
	if(Head.Nextptr){
		STACK_TYPE *tmpptr=Head.Nextptr;
		Head.Nextptr=tmpptr->Nextptr;
		retval=tmpptr->Object;
		delete tmpptr;
	}
	return retval;
}

