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

#include "media.h"

Resources *Media::resman=0;

/*! \brief Closes the internal resources
 *
 *  This method should be called when shutting down the application to keep
 *  valgrind happy :)
 */
void Media::Close(){
	if(resman){
		delete resman;
		resman=0;
	}
}

/*! \brief Loads an internal resource
 *  \param Name Name of the resources you want to load
 *  \return RWops containing the extracted resource
 */
RWops *Media::GetResource(uString Name){
	if(!resman){
		resman=new Resources();
		resman->AddResource(new ArchiveViLE(Cfg::Path::resource));
	}
	return resman->GetResource(Name);
}

/*! \brief Loads an image from the internal resource
 *  \param Name Name of the image you want to load
 *  \return SDL_surface with the decoded imagedata
 */
SDL_Surface *Media::GetImage(uString Name){
	if(!resman){
		resman=new Resources();
		resman->AddResource(new ArchiveViLE(Cfg::Path::resource));
	}
	return resman->GetImage(Name);
}

/*! \brief Loads an animation from the internal resource
 *  \param Name Name of the animation you want to load
 *  \return Array of SDL_surfaces with decoded imagedata
 */
SDL_Surface **Media::GetAnimation(uString Name){
	if(!resman){
		resman=new Resources();
		resman->AddResource(new ArchiveViLE(Cfg::Path::resource));
	}
	return resman->GetAnimation(Name);
}

