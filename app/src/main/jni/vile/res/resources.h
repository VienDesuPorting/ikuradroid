/*! \class Resources
 *  \brief Resource Manager
 *
 *  The resource manager keeps track of archives, and helps index and access the
 *  resources within them. It also collects the format conversion routines and
 *  allows extracting the resources as formatted, native datatypes.
 *
 *  The resource manager is usually not accessed directly but through a driver
 *  class such as Media or EngineVN which defines types of resources (Like
 *  voices, sound effects, music, etc) and distributes them accordingly.
 */

#ifndef _RESOURCES_H_
#define _RESOURCES_H_

// Include necessary datatypes
#include <SDL.h>
#include <SDL_image.h>

// Include archive classes
#include "archives/afiles.h"
#include "archives/avile.h"
#include "archives/awindy.h"
#include "archives/as21.h"
#include "archives/ajast.h"
#include "archives/acrowdsce.h"
#include "archives/acrowdpck.h"
#include "archives/acrowdsnn.h"
#include "archives/aikura.h"
#include "archives/acwaredl1.h"
#include "archives/acwarearc2.h"
#include "archives/awillarc.h"
#include "archives/atlove.h"

// Include converters
#include "converters/cikura.h"
#include "converters/ccrowd.h"
#include "converters/cwindy.h"
#include "converters/ccware.h"
#include "converters/cwill.h"
#include "converters/ctlove.h"


class Resources
{
	private:
		Stringlist list;		//!< Tracks source archives
		ArchiveBase *stack;		//!< Tracks indexed archives
	public:
		// We need a constructor to set defaults
		Resources();
		~Resources();

		// Getters and setters for resources
		int AddResource(uString Path);
		int AddResource(ArchiveBase *Archive);
		int EnumerateArchives(Stringlist *List);
		int EnumerateResources(Stringlist *List);

		// Resource and conversion interface
		RWops *GetResource(uString Name);
		SDL_Surface *GetImage(uString Name);
		SDL_Surface **GetAnimation(uString Name);

		// Write all resources as a ViLE cabinet
		bool Write(uString Archive);
};

#endif


