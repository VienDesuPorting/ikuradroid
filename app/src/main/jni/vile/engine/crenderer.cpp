#include "crenderer.h"

CachedRenderer::CachedRenderer(Group *Widgets,Group *Animations,int Width,
		int Height) : Renderer(Widgets,Animations,Width,Height) {
	clean=0;
	cleanhash=0;
}

CachedRenderer::~CachedRenderer() {
	if(clean){
		SDL_FreeSurface(clean);
	}
}

/*! \brief Renders tainted widgets on top of a hashed cache
 *  \param Surface Surface to render to
 */
void CachedRenderer::UpdateWidgets(SDL_Surface *Surface){
	// Reset global refresh while checking it
	if(!Widget::GetGlobalRefresh()){
		LogError("Renderer called unneccessary");
	}

	// Find first visible layer
	int layer=widgets->GetGroupCount()-1;
	Group *gptr=widgets->GetGroup(layer);
	Widget *wptr=0;
	Group *bottomg=0;
	Widget *bottomw=0;
	while(gptr){
		wptr=gptr->GetWidget(gptr->GetWidgetCount()-1);
		while(wptr){
			if(wptr->GetVisible()){
				SDL_Rect r=wptr->GetPosition();
				if(!r.x && !r.y && r.w>=width && r.h>=height){
					if(wptr->TestSolid()){
						bottomg=gptr;
						bottomw=wptr;
					}
				}
			}
			wptr=wptr->PrevPtr;
		}
		gptr=gptr->PrevPtr;
		layer--;
	}
	if(!bottomg){
		bottomg=widgets->GetGroup(widgets->GetGroupCount()-1);
		bottomw=bottomg->GetWidget(bottomg->GetWidgetCount()-1);
	}

	// Calculate hash and validate cache
	if(clean){
		Uint32 hash=0;
		gptr=bottomg;
		wptr=bottomw;
		do{
			while(wptr){
				if(wptr->GetVisible() && !wptr->PeekLocalRefresh()){
					hash=EDL_HashUpdate(hash,wptr);
				}
				wptr=wptr->PrevPtr;
			}
			gptr=gptr->PrevPtr;
			if(gptr){
				wptr=gptr->GetWidget(gptr->GetWidgetCount()-1);
			}
		}while(gptr);
		if(hash!=cleanhash){
LogError("HASH:%08X:%08X");
			cleanhash=hash;
			SDL_FreeSurface(clean);
			clean=0;
		}
	}

	// Assert cache and paint it to the surface
	if(!clean){
		gptr=bottomg;
		wptr=bottomw;
		do{
			while(wptr){
				if(wptr->GetVisible() && !wptr->PeekLocalRefresh()){
					if(!clean){
						clean=EDL_CreateSurface(width,height);
					}
LogTest("Caching clean widget: %dx%d",wptr->GetWidth(),wptr->GetHeight());
EDL_SetAlpha(clean,0,0xFF);
//					wptr->Copy(clean);
				}
				wptr=wptr->PrevPtr;
			}
			gptr=gptr->PrevPtr;
			if(gptr){
				wptr=gptr->GetWidget(gptr->GetWidgetCount()-1);
			}
		}while(gptr);
	}
	if(clean){
		EDL_BlitSurface(clean,0,Surface,0);
EDL_SetAlpha(Surface,0,0xFF);
	}

	// Paint dirty widgets to the surface
	gptr=bottomg;
	wptr=bottomw;
	do{
		while(wptr){
			if(wptr->GetVisible() && wptr->GetLocalRefresh()){
LogTest("Blitting dirty widget: %u=%dx%d",wptr,wptr->GetWidth(),wptr->GetHeight());
EDL_SetAlpha(Surface,0,0xFF);
//				wptr->Copy(Surface);
			}
			wptr=wptr->PrevPtr;
		}
		gptr=gptr->PrevPtr;
		if(gptr){
			wptr=gptr->GetWidget(gptr->GetWidgetCount()-1);
		}
	}while(gptr);
EDL_SetAlpha(Surface,0,0xFF);
}

