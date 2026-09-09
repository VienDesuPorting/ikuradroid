#ifndef _VILE_H_
#define _VILE_H_

// Include systemstuff
#include "engine/evn.h"
#include "common/config.h"


#ifdef VILE_FEATURE_UNITTEST
#include "testing/coretest.h"
#include "testing/rwtest.h"
#include "testing/filetest.h"
#include "testing/misctest.h"
#include "testing/miscbench.h"
#include "testing/gfxbench.h"
#endif
#ifdef VILE_BUILD_W32CONSOLE
#include "common/guicon.h"
#endif

// Include games
#ifdef VILE_SUPPORT_CROWD
#include "crowd/tokimeki/tokimeki.h"
#include "crowd/xchange1/xchange1.h"
#include "crowd/xchange3/xchange3.h"
#endif
#ifdef VILE_SUPPORT_IKURA
#include "ikura/crescendo/crescendo.h"
#include "ikura/Heartdr/Heartdr.h"
#include "ikura/virgin/virgin.h"
#include "ikura/sagara/sagara.h"
#include "ikura/snow/snow.h"
#include "ikura/kana/kana.h"
#include "ikura/kanaoka/kanaoka.h"
#include "ikura/hitomi/hitomi.h"
#include "ikura/catgirl/catgirl.h"
#include "ikura/idols/idols.h"
#endif
#ifdef VILE_SUPPORT_WINDY
#include "windy/mayclub/mayclub.h"
#include "windy/nocturnal/nocturnal.h"
#endif
#ifdef VILE_SUPPORT_JAST
#include "jast/jumc.h"
#endif
#ifdef VILE_SUPPORT_CWARE
#include "cware/dividead/dividead.h"
#endif
#ifdef VILE_SUPPORT_WILL
#include "will/yume/yume.h"
#include "will/princess/princess.h"
#include "will/critical/critical.h"
#include "will/lmm/littlemaid.h"
#include "will/starry/starry.h"
#endif
#ifdef VILE_SUPPORT_TLOVE
#include "tlove/tlove.h"
#endif

// Default window size
#define DEFAULT_WIDTH		640
#define DEFAULT_HEIGHT		480


// Application error codes
#define ERROR_NONE          0
#define ERROR_PARAMETER     10
#define ERROR_INITSYSTEM    20
#define ERROR_INITVIDEO     30
#define ERROR_INITGAME		40

class ViLE {
	private:
        SDL_Window* window;
     	SDL_Surface* screenSurface;
     	SDL_Renderer *renderer;
		// Probers
		bool ProbeNocturnal(uString Path);
		bool ProbeDiviDead(uString Path);
		bool ProbeMayclub(uString Path);
		bool ProbeCrescendo(uString Path);
		bool ProbeVirgin(uString Path);
		bool ProbeHDR(uString Path);
		bool ProbeSagara(uString Path);
		bool ProbeSnow(uString Path);
		bool ProbeYumeMiruKusuri(uString Path);
		bool ProbePrincessWaltz(uString Path);
		bool ProbeStarrySky(uString Path);
		bool ProbeCriticalPoint(uString Path);
		bool ProbeLittleMyMaid(uString Path);
		bool ProbeTrueLove(uString Path);
		bool ProbeKanaOkaeri(uString Path);
		bool ProbeKana(uString Path);
		bool ProbeHitomi(uString Path);
		bool ProbeCatGirl(uString Path);
		bool ProbeIdols(uString Path);
		bool ProbeJUMC(uString Path);
		bool ProbeTokimeki(uString Path);
		bool ProbeXChange1(uString Path);
		bool ProbeXChange3(uString Path);

		// Probing methods
		bool ProbeSUF(uString Path,uString Key);
		bool ProbeResource(uString Path,uString Resource);
		bool ProbeString(uString S1,uString S2);
		bool ProbeSize(uString Path,unsigned int Size);
	public:
		EngineVN *LoadEngine(uString Path);
		void RunEngine(EngineVN *Engine);
		bool InitSystem(Uint32 Flags);
		bool InitVideo();
		void Quit();

		void About();
		void Help();
		void InfoRuntime();
		void InfoTitle(EngineVN *Engine);
		void InfoResources(EngineVN *Engine);
		bool List(uString Archive);
		bool Archive(uString Archive,Stringlist *List);
		bool XDec(uString Archive);
		bool XDec(uString Archive,Stringlist *List);
		bool Extract(uString Archive);
		bool Extract(uString Archive,Stringlist *List);
		bool Decode(uString Resource);
		void Error(uString Title,uString Message);
};

#endif

