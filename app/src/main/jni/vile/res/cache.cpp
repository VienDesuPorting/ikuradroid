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

#include "cache.h"

Cache::Cache(int Size){
	root.Surface=0;
	root.NextPtr=0;
	maxsize=Size;
	size=0;
}

Cache::~Cache(){
	while(root.NextPtr){
		CacheItem *tmp=root.NextPtr->NextPtr;
		SDL_FreeSurface(root.NextPtr->Surface);
		delete root.NextPtr;
		root.NextPtr=tmp;
	}
}

/*! \brief Retrieves resource from storage
 *  \param Name Name of the resource
 *  \return Pointer to resource or NULL
 */
SDL_Surface *Cache::GetSurface(uString Name){
	SDL_Surface *retval=0;
	CacheItem *tmpptr=&root;
	while(tmpptr->NextPtr && tmpptr->NextPtr->Name!=Name){
		tmpptr=tmpptr->NextPtr;
	}
	if(tmpptr->NextPtr){
		CacheItem *item=tmpptr->NextPtr;
		retval=item->Surface;
		tmpptr->NextPtr=tmpptr->NextPtr->NextPtr;
		item->NextPtr=root.NextPtr;
		root.NextPtr=item;
	}
	return retval;
}

/*! \brief Stores a resource pointer on the stack
 *  \param Name Name of the resource
 *  \param Surface Pointer to store
 */
void Cache::SetSurface(uString Name,SDL_Surface *Surface){
	CacheItem *tmpptr=&root;
	while(tmpptr->NextPtr && tmpptr->NextPtr->Name!=Name){
		tmpptr=tmpptr->NextPtr;
	}
	if(tmpptr->NextPtr){
		// Update existing item
		CacheItem *item=tmpptr->NextPtr;
		if(item->Surface!=Surface){
			SDL_FreeSurface(item->Surface);
			item->Surface=Surface;
		}
		tmpptr->NextPtr=tmpptr->NextPtr->NextPtr;
		item->NextPtr=root.NextPtr;
		root.NextPtr=item;
	}
	else{
		// Add new item
		CacheItem *item=new CacheItem;
		item->Name=Name;
		item->Surface=Surface;
		item->NextPtr=root.NextPtr;
		root.NextPtr=item;
		size++;
	}

	if(size>maxsize){
		// Purge queue
		tmpptr=&root;
		while(tmpptr->NextPtr->NextPtr){
			tmpptr=tmpptr->NextPtr;
		}
		CacheItem *item=tmpptr->NextPtr;
		tmpptr->NextPtr=0;
		SDL_FreeSurface(item->Surface);
		delete item;
	}
}


