/*! \class ArchiveViLE
 *	\brief Extracts resources from the common ViLE format
 */
/*
 *  IkuraDroid modification notice - 2026
 *
 *  This file is modified for IkuraDroid (su.viende.ikuradroid) - the
 *  ViLE-based Android visual novel player - and differs from the
 *  original ViLE 0.4.13 source.
 *
 *  Base: ViLE 0.4.13 by the ViLE Team <team@vilevn.org>. The Android
 *  port continues the original VILE for Android (2015-2016) and is
 *  modified for IkuraDroid by the VienDesu! Porting Team
 *  <github.com/VienDesuPorting/ikuradroid>.
 *
 *  Distributed under the same GNU GPL (version 3) as the original
 *  ViLE; this notice supplements the original copyright.
 */
#ifndef _AVILE_H_
#define _AVILE_H_

#include "abase.h"

class ArchiveViLE : public ArchiveBase {
	public:
		ArchiveViLE(uString Path);
		ArchiveViLE(const unsigned char *Data,int Size);
		virtual RWops *GetResource(uString Name);
	private:
		void IndexPack(const unsigned char *Data,int Size);
		const unsigned char *memdata;
};

#endif

