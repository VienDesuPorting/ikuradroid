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

#include "ebase.h"
#include "../common/config.h"

/*! \brief Prepares game surface and other primitives
 *	\param Width The games native width
 *	\param Height The games native height
 */
EngineBase::EngineBase(int Width,int Height){
	// Store metrics
	width=Width;
	height=Height;

	// Load core group
	widget_head=new Group(0);
	animations=new Group(0);
	widget_focus=0;

	// Set defaults
	hidemouse=0;
	fullscreen=false;
	shutdown=false;
	transition_type=VT_NONE;
	renderer=new Renderer(widget_head,animations,Width,Height);
	EDLrendererTex = new EDLTexture();
	EDLrendererTex->createTarget(Width,Height);

	EDLrendererTex->setAsRenderTarget();
	boxRGBA(EDLRenderer, 0, 0, 640,480,0x00,0x00,0x00,0xFF);
	SDL_SetRenderTarget( EDLRenderer, NULL );

	mx=400;my=300;
}

/*! \brief Free objects from ram
 */
EngineBase::~EngineBase(){
	LogVerbose("Closing managed widgets");
	delete renderer;
	delete EDLrendererTex;
	widget_head->DestroyGroup();
	delete widget_head;
	animations->DestroyGroup();
	delete animations;
	LogVerbose("Engine is terminated");
}

/*! \brief Asserts an indexed layer
 *  \param Layer Indexed layer
 */
Group *EngineBase::GetLayer(int Layer){
	// Assert layer
	while(!widget_head->GetGroup(Layer)){
		new Group(widget_head,Layer);
	}
	return widget_head->GetGroup(Layer);
}

/*! \brief Gets an indexed widget
 *  \param Layer Index of layer to get widget from
 *  \param Index Index of widget to retrieve
 *  \returns Indexed widget or NULL if failed
 */
Widget *EngineBase::GetWidget(int Layer,int Index){
	Group *gptr=widget_head->GetGroup(Layer);
	if(gptr){
		return gptr->GetWidget(Index);
	}
	else{
		return 0;
	}
}

/*! \brief Checks wether a widget is still active
 *  \param WidgetPtr Pointer to widget
 *  \return True if the widget still exists
 */
bool EngineBase::CheckWidget(Widget *WidgetPtr){
	return widget_head->CheckWidget(WidgetPtr);
}

/*! \brief Checks wether a shutdown has been confirmed by the user
 *  \return True if the user has verified a shutdown request
 */
bool EngineBase::GetShutdown(){
	return shutdown;
}

/*! \brief Checks wether game is running in fullscreen mode
 *  \return True if fullscreen mode is enabled
 */
bool EngineBase::GetFullscreen(){
	return fullscreen;
}

/*! \brief Toggles fullscreen mode
 */
void EngineBase::SetFullscreen(){

	SetFullscreen(!fullscreen);
}

void EngineBase::updateRenderWindow(SDL_Window* win)
{
    window = win;
    renderer->setWindow(win);
    renderer->UpdateResolution();
}

/*! \brief Sets fullscreen mode
 *  \param Fullscreen True for fullscreen mode
 */
void EngineBase::SetFullscreen(bool Fullscreen){

 SDL_SetWindowFullscreen( window, Fullscreen );

}

/*! \brief Registers a transition to use for the next refresh
 *  \param Transition Transition to use
 *  \param Duration Length of transition in milliseconds
 */
void EngineBase::SetTransition(VN_TRANSITIONS Transition,
		Uint32 Duration,SDL_Rect *Rect){
	transition_type=Transition;
	transition_time=Duration;
	if(Rect){
		transition_rect=*Rect;
	}
	else{
		EDL_SETRECT(transition_rect,0,0,width,height);
	}
}

/*! \brief Registers the default transition to use for the next refresh
 */
void EngineBase::SetTransition(SDL_Rect *Rect){
	transition_type=VT_RANDOM;
	transition_time=500;
	if(Rect){
		transition_rect=*Rect;
	}
	else{
		EDL_SETRECT(transition_rect,0,0,width,height);
	}
}

/*! \brief Drops and deletes currently running animation or effect
 *  \return True if an animation was dropped from the queue
 */
bool EngineBase::DestroyAnimation(){
	bool retval=false;
	if(animations->GetWidgetCount()){
		// Cancel running animation
		int n=animations->GetWidgetCount()-1;
		Animation *aptr=(Animation*)animations->GetWidget(n);
		if(aptr){
			animations->DestroyWidget(aptr);
			retval=true;
		}
	}
	return retval;
}

/*! \brief Skips currently running animation or effect
 *  \return True if an animation was skipped
 *
 *  No widgets will be freed or deleted by this call, it will only affect
 *  the topmost object which will remain on top until the next repaint. This
 *  asserts that the last frame in the animation is painted onto the screen
 *  before any subsequent animations. Use FlushAnimations() to drop animations
 *  entirely.
 */
bool EngineBase::SkipAnimation(){
	bool retval=false;
	if(animations->GetWidgetCount()){
		// Cancel running animation
		int n=animations->GetWidgetCount()-1;
		Animation *aptr=(Animation*)animations->GetWidget(n);
		if(aptr){
			aptr->Skip();
			retval=true;
		}
	}
	return retval;
}

/*! \brief Halt normal processing while animating a widget
 *  \param WidgetPtr Widget to be animated
 *
 *  Several widgets can be queued for animation. Graphic is not persistent
 *  and will disappear when the animation has completed unless it has been
 *  drawn into the parent widget.
 *
 *  Other widgets will not be repainted until all animation has completed.
 */
void EngineBase::AddAnimation(Animation *WidgetPtr){
	animations->AddWidget(WidgetPtr);
}

/*! \brief Adds a widget to a specified layer
 *  \param WidgetPtr GUI component to add to the engine
 *  \param Layer Logical layer to add to (0=Top layer)
 *  \param Position Logical position to add to (0=Top position)
 *
 *  The layer index will be asserted, but the position will not be padded
 */
void EngineBase::AddWidget(Widget *WidgetPtr,int Layer,int Position){
	// Assert layer
	while(!widget_head->GetGroup(Layer)){
		new Group(widget_head,Layer);
	}

	// Get layer and iterate to the right position
	Group *wg=GetLayer(Layer);
	wg->AddWidget(WidgetPtr,Position);
	if(widget_head->CheckWidget(widget_focus)){
		if(WidgetPtr->GetVisible() && WidgetPtr->GetHittable()){
			widget_focus->FocusLeave();
			widget_focus=WidgetPtr;
			widget_focus->FocusEnter();
		}
	}
}

/*! \brief Destroy a specified widget within a specific layer
 *  \param Pointer of Widget to destroy
 *  \param Layer Index of parent layer
 */
void EngineBase::DestroyWidget(Widget *WidgetPtr,int Layer){
	Group *wg=GetLayer(Layer);
	if(wg){
		wg->DestroyWidget(WidgetPtr);
	}
}

/*! \brief Destroy a specified widget within any layer
 *  \param Pointer of Widget to destroy
 */
void EngineBase::DestroyWidget(Widget *WidgetPtr){
	for(int i=0;widget_head->GetGroup(i);i++){
		for(int j=0;GetWidget(i,j);j++){
			Widget *wptr=GetWidget(i,j);
			if(wptr==WidgetPtr){
				Group *gptr=widget_head->GetGroup(i);
				if(gptr){
					gptr->DestroyWidget(WidgetPtr);
					return;
				}
			}
		}
	}
}

/*! \brief Destroys all the widgets in a given layer
 *  \param Layer Layer to flush
 */
void EngineBase::DestroyLayer(int Layer){
	Group *wg=GetLayer(Layer);
	if(wg){
		wg->DestroyGroup();
	}
}

/*! \brief Shows or hides all the widgets in a given layer
 *  \param Layer Layer to show or hide
 *  \param Visible True to show widgets
 */
void EngineBase::ShowLayer(int Layer,bool Visible){
	Group *wg=GetLayer(Layer);
	Widget *wb=wg->GetWidget(0);
	while(wb){
		wb->SetVisible(Visible);
		wb=wb->NextPtr;
	}
}

/*! \brief Limits the framerate and relieves CPU by sleeping
 *  \param Framerate Framerate to lock into (1s/Framerate)
 */
void EngineBase::LimitFramerate(int Framerate){
	static Uint32 lastreport=SDL_GetTicks();
	static Uint32 sleeptime=0;
	static Uint32 sleeptick=0;
	static Uint32 lasttick=SDL_GetTicks();
	static Uint32 framecount=0;
	framecount++;
	Uint32 duration=(Uint32)((double)framecount*(1000.0/(double)Framerate));
	Uint32 nowtick=SDL_GetTicks();
	Uint32 endtick=lasttick+duration;
	if(Cfg::System::Verbose && nowtick>=lastreport+30000){
		// Report sleep/wake statistics
		Uint32 waketime=nowtick-lastreport;
		double wakepercent=(sleeptime/(double)waketime)*100;
		LogVerbose("Slept %d mS of %d mS (%f%%) in %d turns (%d mS per turn)",
				sleeptime,waketime,wakepercent,sleeptick,sleeptime/sleeptick);
		lastreport=nowtick;
		sleeptime=0;
		sleeptick=0;
	}
	if(nowtick<=endtick){
		// Relieve CPU
		SDL_Delay(endtick-nowtick);
		sleeptime+=endtick-nowtick;
		sleeptick++;
	}
	else{
		// Adjust timer without sleeping
		lasttick=nowtick;
		framecount=0;
	}
}

/*! \brief Relocates the mouse cursor to the given coordinates
 *  \param X Screen coordinate (Native resolution)
 *  \param Y Screen coordinate (Native resolution)
 *
 *  Just use SDL_WarpMouse directly if you want the cursor to physical
 *  coordinates rather than operating at the game resolution.
 */
void EngineBase::CursorWarp(int X,int Y){
	//SDL_Surface *screen=SDL_GetVideoSurface();
	//if(screen){
	//	int x=X*(screen->w/(double)NativeWidth());
	//	int y=Y*(screen->h/(double)NativeHeight());
	//	SDL_WarpMouse(x,y);
	//}
}

/*! \brief Calculates X position relative to native resolution
 *  \param Screen Screen surface (To get output size)
 *  \param X Screen coordinate
 *  \return X coordinate for native resolution
 */
int EngineBase::GetRelativeX(SDL_Surface *Screen,int X){
	int retval=X;
	if(Screen){
		double r=NativeWidth()/(double)Screen->w;
		retval=X*r;
	}
	return retval;
}

/*! \brief Calculates Y position relative to native resolution
 *  \param Screen Screen surface (To get output size)
 *  \param Y Screen coordinate
 *  \return Y coordinate for native resolution
 */
int EngineBase::GetRelativeY(SDL_Surface *Screen,int Y){
	int retval=Y;
	if(Screen){
		double r=NativeHeight()/(double)Screen->h;
		retval=Y*r;
	}
	return retval;
}

/*! \brief Passes ticks into the engine and regulates framerate
 *
 *  Ticks will not be passed into the client engine if there are animations
 *  currently running.
 *
 *  Must be called at each iteration loop
 */
void EngineBase::EventHostTick(){
	// Process engine ticks
	if(!animations->GetWidgetCount()){
		if(!EventGameTick()){
#if ENGINE_PROCESSTICKS
			Uint32 t=SDL_GetTicks();
			while(!EventGameProcess() && t+ENGINE_PROCESSTICKS>SDL_GetTicks());
			//LogDebug("Processed: %d",SDL_GetTicks()-t);
#else
			EventGameProcess();
#endif
		}
	}

	// Process widget ticks
	static Uint32 lasttick=SDL_GetTicks();
	Uint32 now=SDL_GetTicks();
	if(now<lasttick || now-lasttick>=100){
		lasttick=now;
		Group *gptr=widget_head->GetGroup(0);
		while(gptr){
			Widget *wptr=gptr->GetWidget(0);
			while(wptr){
				wptr->Tick();
				wptr=wptr->NextPtr;
			}
			gptr=gptr->NextPtr;
		}
	}
	if(Cfg::System::Framerate){
		LimitFramerate(Cfg::System::Framerate);
	}
	render();
}

/*! \brief Paints widgets to a surface
 *  \param Surface Surface to paint widgets onto
 *  \param Toplayer An optional toplayer (To skip dialogboxes etc)
 */
const int FLIP_VERTICAL = 1;
const int FLIP_HORIZONTAL = 2;

Uint32 get_pixel32( SDL_Surface *surface, int x, int y )
{
    //Convert the pixels to 32 bit
    Uint32 *pixels = (Uint32 *)surface->pixels;

    //Get the requested pixel
    return pixels[ ( y * surface->w ) + x ];
}
void put_pixel32( SDL_Surface *surface, int x, int y, Uint32 pixel )
{
    //Convert the pixels to 32 bit
    Uint32 *pixels = (Uint32 *)surface->pixels;

    //Set the pixel
    pixels[ ( y * surface->w ) + x ] = pixel;
}
SDL_Surface *flip_surface( SDL_Surface *surface, int flags )
{
    //Pointer to the soon to be flipped surface
    SDL_Surface *flipped = NULL;

    //If the image is color keyed
    if( surface->flags  )
    {
        flipped = SDL_CreateRGBSurface( SDL_SWSURFACE, surface->w, surface->h, surface->format->BitsPerPixel, surface->format->Rmask, surface->format->Gmask, surface->format->Bmask, 0 );
    }
    //Otherwise
    else
    {
        flipped = SDL_CreateRGBSurface( SDL_SWSURFACE, surface->w, surface->h, surface->format->BitsPerPixel, surface->format->Rmask, surface->format->Gmask, surface->format->Bmask, surface->format->Amask );
    }

    //If the surface must be locked
    if( SDL_MUSTLOCK( surface ) )
    {
        //Lock the surface
        SDL_LockSurface( surface );
    }

    //Go through columns
    for( int x = 0, rx = flipped->w - 1; x < flipped->w; x++, rx-- )
    {
        //Go through rows
        for( int y = 0, ry = flipped->h - 1; y < flipped->h; y++, ry-- )
        {
            //Get pixel
            Uint32 pixel = get_pixel32( surface, x, y );

            //Copy pixel
            if( ( flags & FLIP_VERTICAL ) && ( flags & FLIP_HORIZONTAL ) )
            {
                put_pixel32( flipped, rx, ry, pixel );
            }
            else if( flags & FLIP_HORIZONTAL )
            {
                put_pixel32( flipped, rx, y, pixel );
            }
            else if( flags & FLIP_VERTICAL )
            {
                put_pixel32( flipped, x, ry, pixel );
            }
        }
    }

    //Unlock surface
    if( SDL_MUSTLOCK( surface ) )
    {
        SDL_UnlockSurface( surface );
    }

    //Copy color key
    if( surface->flags )
    {
       // SDL_SetColorKey( flipped, SDL_RLEACCEL , surface->format-> );
    }

    //Return flipped surface
    return flipped;
}

void EngineBase::Paint(SDL_Surface *Surface,VN_LAYERS Toplayer){

    SDL_SetRenderDrawColor(EDLRenderer,0,0,0,255);
    SDL_RenderClear(EDLRenderer);
    EDLrendererTex->setAsRenderTarget();
    renderer->Render(Toplayer);

    SDL_Surface *tsurface=SDL_CreateRGBSurface(SDL_SWSURFACE,Surface->w,Surface->h,32,
                                                0xff000000,0x00ff0000,0x0000ff00,0x000000ff);
    SDL_RenderReadPixels(EDLRenderer,0,SDL_PIXELFORMAT_RGBA8888,tsurface->pixels,tsurface->pitch);
    // Render-target readback orientation varies between backends
    // (bottom-up storage on some GLES2 targets); probe it once instead
    // of assuming a flip
    SDL_Surface *fl=EDL_ReadbackFlipped()?flip_surface(tsurface,FLIP_VERTICAL):tsurface;
    EDL_BlendSurface(fl,0,Surface,0);

    if(fl!=tsurface){
        SDL_FreeSurface(fl);
    }
    SDL_FreeSurface(tsurface);

    SDL_SetRenderTarget( EDLRenderer, NULL );
    EDLrendererTex->render(0, 0);
}


void EngineBase::render(){
	if(Widget::PeekGlobalRefresh()){

		// Paint widgets
		if(transition_type!=VT_NONE){
			// Use a transition effect
			SDL_Surface *s=EDL_CreateSurface(NativeWidth(),NativeHeight());
			renderer->Render();
			SDL_Rect r=transition_rect;
			AddAnimation(new Fade(r,s,r,transition_time));
			SDL_FreeSurface(s);

			// Start animation with a recursive call
			SDL_Event event;
			while(SDL_PollEvent(&event));
			transition_type=VT_NONE;
			render();
		}
		else{
			// Copy surface to display
			//renderer->Update(Screen);
			SDL_SetRenderDrawColor(EDLRenderer,0,0,0,255);
			SDL_RenderClear(EDLRenderer);
			EDLrendererTex->setAsRenderTarget();
			renderer->Update();
			SDL_SetRenderTarget( EDLRenderer, NULL );
            EDLrendererTex->render(0, 0);

            SDL_RenderPresent(EDLRenderer);
		}
	}
	else
	{
		SDL_SetRenderDrawColor(EDLRenderer,0,0,0,255);
		SDL_RenderClear(EDLRenderer);
		EDLrendererTex->render(0, 0);
		SDL_RenderPresent(EDLRenderer);
	}
}

/*! \brief Get visible widget at screen coordinate
 *  \param X Coordinate
 *  \param Y Coordinate
 *  \return Widget at coordinate or NULL if none
 */
Widget *EngineBase::GetWidgetAt(int X,int Y){
	// Find "top" of visible widgets
	Widget *wptr=0;
	Group *gptr=0;
	int gtop=0;
	gptr=widget_head->GetGroup(gtop);
	while(!wptr && gptr){
		wptr=gptr->GetWidget(X,Y);
		gptr=gptr->NextPtr;
	}
	return wptr;
}

/*! \brief Passes mouse click events into the engine
 *	\param Screen Screen surface
 *	\param X Screen X coordinate
 *	\param Y Screen Y coordinate
 */
void EngineBase::EventHostMouseLeftDown(SDL_Surface *Screen,int X,int Y){
	X=GetRelativeX(Screen,X);
	Y=GetRelativeY(Screen,Y);
	mx = X;
	my = Y;
		if(animations->GetWidgetCount()){
		// Cancel running animation
		SkipAnimation();
	}
	else{
		// Pass event to widgets
		Widget *wptr=GetWidgetAt(X,Y);
		LOGCAT("ikuradroid input: left down (%d,%d) hit=%p",X,Y,(void*)wptr);
		if(!wptr || !wptr->MouseLeftDown(X,Y)){
			EventBackgroundMouseLeftDown(X,Y);
		}
	}
}

/*! \brief Passes mouse click events into the engine
 *	\param Screen Screen surface
 *	\param X Screen X coordinate
 *	\param Y Screen Y coordinate
 */
void EngineBase::EventHostMouseLeftUp(SDL_Surface *Screen,int X,int Y){
	X=GetRelativeX(Screen,X);
	Y=GetRelativeY(Screen,Y);
	mx = X;
	my = Y;
	if(!animations->GetWidgetCount()){
		// Pass event to widgets
		Widget *wptr=GetWidgetAt(X,Y);
		if(!wptr || !wptr->MouseLeftUp(X,Y)){
			EventBackgroundMouseLeftUp(X,Y);
		}
	}
}

/*! \brief Passes mouse click events into the engine
 *	\param Screen Screen surface
 *	\param X Screen X coordinate
 *	\param Y Screen Y coordinate
 */
void EngineBase::EventHostMouseRightUp(SDL_Surface *Screen,int X,int Y){
	X=GetRelativeX(Screen,X);
	Y=GetRelativeY(Screen,Y);
	mx = X;
	my = Y;
	if(!animations->GetWidgetCount()){
		// Pass event to widgets
		Widget *wptr=GetWidgetAt(X,Y);
		if(!wptr || !wptr->MouseRightUp(X,Y)){
			EventBackgroundMouseRightUp(X,Y);
		}
	}
}

/*! \brief Passes mouse click events into the engine
 *	\param Screen Screen surface
 *	\param X Screen X coordinate
 *	\param Y Screen Y coordinate
 */
void EngineBase::EventHostMouseRightDown(SDL_Surface *Screen,int X,int Y){
	X=GetRelativeX(Screen,X);
	Y=GetRelativeY(Screen,Y);
	mx = X;
	my = Y;
	if(animations->GetWidgetCount()){
		// Cancel running animation
		SkipAnimation();
	}
	else{
		// Pass event to widgets
		Widget *wptr=GetWidgetAt(X,Y);
		if(!wptr || !wptr->MouseRightDown(X,Y)){
			EventBackgroundMouseRightDown(X,Y);
		}
	}
}

/*! \brief Passes mouse click events into the engine
 *	\param Screen Screen surface
 *	\param X Screen X coordinate
 *	\param Y Screen Y coordinate
 */
void EngineBase::EventHostMouseMove(SDL_Surface *Screen,int X,int Y){

	X=GetRelativeX(Screen,X);
	Y=GetRelativeY(Screen,Y);
	mx = X;
	my = Y;
	if(!animations->GetWidgetCount()){
#if ENGINE_HIDEMOUSE
		// Force cursor
		if(hidemouse<ENGINE_HIDEMOUSE){
			hidemouse++;
		}
		else if(hidemouse==ENGINE_HIDEMOUSE){
			SDL_ShowCursor(SDL_ENABLE);
			hidemouse++;
		}
#endif

		// Track focus
		Widget *wptr=GetWidgetAt(X,Y);
		if(wptr!=widget_focus){
			if(widget_head->CheckWidget(widget_focus)){
				widget_focus->FocusLeave();
			}
			widget_focus=wptr;
			if(widget_focus){
				widget_focus->FocusEnter();
			}
		}

		// Pass event to widgets
		if(!wptr || !wptr->MouseMove(X,Y)){
			EventBackgroundMouseMove(X,Y);
		}
	}
}

/*! \brief Decodes and distributes keyboard events
 *  \param Key SDL Keyboard symbol struct
 *
 *  Please note that most engine processing goes down in the KeyDown chain.
 */
void EngineBase::EventHostKeyUp(SDL_Keycode Key){
	if(animations->GetWidgetCount()){
	}
	else if(widget_head->CheckWidget(widget_focus)){
		// Pass to focused widget
		if(!widget_focus->KeyUp(Key)){
			EventBackgroundKeyUp(Key);
		}
	}
	else{
		// Register as noise
		EventBackgroundKeyUp(Key);
	}
}

/*! \brief Decodes and distributes keyboard events
 *  \param Key SDL Keyboard symbol struct
 *
 *  The engine only does minimal checking for system keys (Full screen, etc)
 *  and global hotkeys. Most of the keyboard processing goes down in the
 *  DialogBase class wich manages an onscreen group of widgets.
 */
void EngineBase::EventHostKeyDown(SDL_Keycode Key){
	// Check system combinations
	SDL_Keymod mod=SDL_GetModState();
	if((mod&KMOD_LALT || mod&KMOD_RALT)){
		if(Key==SDLK_F4){
			EventGameDialog(VD_SHUTDOWN);
		}
		else if(KEY_RETURN(Key)){
			SetFullscreen();
		}
		else if(Key==SDLK_0){
			renderer->SetScaler(GS_DEFAULT);
		}
		else if(Key==SDLK_1){
			renderer->SetScaler(GS_FAST);
		}
		else if(Key==SDLK_2){
			renderer->SetScaler(GS_NEAREST);
		}
		else if(Key==SDLK_3){
			renderer->SetScaler(GS_HQ2X);
		}
		else if(Key==SDLK_4){
			renderer->SetScaler(GS_HQ3X);
		}
		else if(Key==SDLK_5){
			renderer->SetScaler(GS_HQ4X);
		}

		// Repaint with new scaler
		for(int i=0;widget_head->GetGroup(i);i++){
			for(int j=0;GetWidget(i,j);j++){
				Widget *wptr=GetWidget(i,j);
				wptr->Refresh();
			}
		}

	}
	else if(Key==SDLK_F5){
		EventGameDialog(VD_LOAD);
	}
	else if(Key==SDLK_F6){
		EventGameDialog(VD_SAVE);
	}
	else if(Key==SDLK_F7){
		EventGameDialog(VD_OPTIONS);
	}
	else if(Key==SDLK_F8){
		EventGameDialog(VD_TITLE);
	}
	else if(Key==SDLK_F9){
		EventGameDialog(VD_SHUTDOWN);
	}
	else if(Key==SDLK_F10){
		// Dump screenshot
		SDL_Surface *tmp=EDL_CreateSurface(width,height);
		Paint(tmp);
		EDL_SaveSurface(tmp,"IkuraDroid");
		SDL_FreeSurface(tmp);
	}
	else if(Key==SDLK_F11){
		Cfg::System::Verbose=!Cfg::System::Verbose;
	}
	else if(animations->GetWidgetCount()){
		// Cancel running animation
		SkipAnimation();
	}
	else{
#if ENGINE_HIDEMOUSE
		// Disable cursor when using keyboard
		if(hidemouse>0){
			hidemouse--;
		}
		else if(hidemouse==0){
			SDL_ShowCursor(SDL_DISABLE);
			hidemouse--;
		}
#endif

		// Pass event to focused widget/dialog/engine
		if(widget_head->CheckWidget(widget_focus) &&
				widget_focus->GetVisible()){
			if(KEY_REFOCUS(Key)){
				// Find next non-obscured widget
				SetFocusNext();
			}
			else if(!widget_focus->KeyDown(Key)){
				// Unset focus and pass event to engine
				LogDebug("Engine drops keyboard focus");
				widget_focus->FocusLeave();
				widget_focus=0;
				EventBackgroundKeyDown(Key);
			}
		}
		else if(KEY_DIRECTION(Key)){
			// Find "top" visible widget capable of input
			SetFocusFirst();
		}
		else{
			// Pass event to engine
			LogDebug("Keyboard signal passed to engine");
			EventBackgroundKeyDown(Key);
		}
	}
}

/*! \brief Shifts keyboard focus from current to next widget
 *  \return True if a focus change occured
 */
bool EngineBase::SetFocusNext(){
	// Find next non-obscured widget
	bool retval=false;
	int sw=NativeWidth();
	int sh=NativeHeight();
	Widget *oldfocus=widget_focus;
	if(!widget_head->CheckWidget(widget_focus)){
		// Reset to first widget
		retval=SetFocusFirst();
	}
	else if(widget_focus->TestMouse(0,0) && widget_focus->TestMouse(sw,sh)){
		// Reset to first widget
		retval=SetFocusFirst();
	}
	else{
		// Find next widget
		widget_focus=0;
		bool oldsearch=true;
		Group *gptr=widget_head->GetGroup(0);
		while(!widget_focus && gptr){
			Widget *wptr=gptr->GetWidget(0);
			while(!widget_focus && wptr){
				if(wptr==oldfocus){
					oldsearch=false;
				}
				else if(oldsearch){
					// Wait for focused widget
				}
				else if(wptr->TestMouse(0,0) && wptr->TestMouse(sw,sh)){
					if(wptr->GetHittable() && wptr->GetVisible()){
						// Accept bottom widget but stop scan
						oldfocus->FocusLeave();
						wptr->FocusEnter();
						wptr->KeyDown(SDLK_TAB);
						widget_focus=wptr;
					}
				}
				else{
					// Search for hittable widgets
					if(wptr->GetHittable() && wptr->GetVisible()){
						//
						// Mayclub krever tabbing mellom overlappende
						// dialoger siden textview er fullskjerm. Vurder
						// om vi må sette en regel mot dette.
						//
						//if(!wptr->TestMouse(fx1,fy1)){
						//	if(!wptr->TestMouse(fx2,fy2)){
						//		int tx1=wptr->GetX();
						//		int ty1=wptr->GetY();
						//		int tx2=tx1+wptr->GetWidth();
						//		int ty2=ty1+wptr->GetHeight();
						//		if(!oldfocus->TestMouse(tx1,ty1)){
						//			if(!oldfocus->TestMouse(tx2,ty2)){
										oldfocus->FocusLeave();
										wptr->FocusEnter();
										wptr->KeyDown(SDLK_TAB);
										widget_focus=wptr;
						//			}
						//		}
						//	}
						//}
					}
				}
				wptr=wptr->NextPtr;
			}
			gptr=gptr->NextPtr;
		}
	}
	return retval;
}

/*! \brief Forces keyboard focus to top widget
 *  \return True if a focus change occured
 */
bool EngineBase::SetFocusFirst(){
	Widget *oldfocus=widget_focus;
	widget_focus=0;
	Group *gptr=widget_head->GetGroup(0);
	while(!widget_focus && gptr){
		Widget *wptr=gptr->GetWidget(0);
		while(!widget_focus && wptr){
			if(wptr->GetHittable() && wptr->GetVisible()){
				wptr->FocusEnter();
				wptr->KeyDown(SDLK_TAB);
				widget_focus=wptr;
			}
			wptr=wptr->NextPtr;
		}
		gptr=gptr->NextPtr;
	}
	return widget_focus && widget_focus!=oldfocus;
}

/*! \brief Forces keyboard focus to top widget
 *  \return True if a focus change occured
 */
bool EngineBase::SetFocusWidget(Widget *WidgetPtr){
	if(WidgetPtr->GetHittable() && WidgetPtr->GetVisible()){
		WidgetPtr->FocusEnter();
		WidgetPtr->KeyDown(SDLK_TAB);
		widget_focus=WidgetPtr;
		return true;
	}
	else{
		return false;
	}
}


/*! \brief Event that is called when no widgets has accepted an event
 *  \param X Screen coordinate
 *  \param Y Screen coordinate
 *  \return True if event was handled
 */
bool EngineBase::EventBackgroundMouseMove(int X,int Y){
	return false;
}

/*! \brief Event that is called when no widgets has accepted an event
 *  \param X Screen coordinate
 *  \param Y Screen coordinate
 *  \return True if event was handled
 */
bool EngineBase::EventBackgroundMouseLeftDown(int X,int Y){
	return false;
}

/*! \brief Event that is called when no widgets has accepted an event
 *  \param X Screen coordinate
 *  \param Y Screen coordinate
 *  \return True if event was handled
 */
bool EngineBase::EventBackgroundMouseRightDown(int X,int Y){
	return false;
}

/*! \brief Event that is called when no widgets has accepted an event
 *  \param X Screen coordinate
 *  \param Y Screen coordinate
 *  \return True if event was handled
 */
bool EngineBase::EventBackgroundMouseLeftUp(int X,int Y){
	return false;
}

/*! \brief Event that is called when no widgets has accepted an event
 *  \param X Screen coordinate
 *  \param Y Screen coordinate
 *  \return True if event was handled
 */
bool EngineBase::EventBackgroundMouseRightUp(int X,int Y){
	return false;
}

/*! \brief Event that is called when no widgets has accepted an event
 *  \param Key Pressed key
 *  \return True if event was handled
 */
bool EngineBase::EventBackgroundKeyDown(SDL_Keycode Key){
	return false;
}

/*! \brief Event that is called when no widgets has accepted an event
 *  \param Key Pressed key
 *  \return True if event was handled
 */
bool EngineBase::EventBackgroundKeyUp(SDL_Keycode Key){
	return false;
}

/*! \brief Event which signals the need for opening a specific dialog
 *  \param Dialog Dialog type to open
 */
void EngineBase::EventGameDialog(VN_DIALOGS Dialog){
}

/*! \brief Generates an SDL event which shuts down the host application
 *  \return True if event was handled
 */
bool EngineBase::EventGameShutdown(){
	int c=0;
	SDL_Event event;
	while(SDL_PollEvent(&event)){
		c++;
	}
	if(c){
		LogVerbose(EDL_Format("Flushed %d system events",c));
	}
	event.type=SDL_QUIT;
	SDL_PushEvent(&event);
	shutdown=true;
	return true;
}

/*! \brief Passes a tick event into client engine
 *  \return True if event was handled
 *
 *  Tickevents are supposed to monitor stuff without regards to framerate
 *  or other low priority tasks which should be kept in the process event.
 *
 *  EventGameProcess will only be called if this returned false
 */
bool EngineBase::EventGameTick(){
	return true;
}

/*! \brief Passes a process event into client engine
 *  \return True if event was handled
 *
 *  Returning false in this function will cause the engine to repeat the
 *  call until the designated time runs out.
 */
bool EngineBase::EventGameProcess(){
	return true;
}

/*! \brief Defines a short string that identifies the game
 *  \return Game identification code
 *
 *  The game code should not contain any whitespace or special letters
 *  as it will be used to generate gamespecific filenames such as
 *  savegames.
 */
const uString EngineBase::NativeID(){
	return "IkuraDroid";
}

/*! \brief Defines a name for the game
 *  \return Game name
 *
 *  Game name can contain whitespace and simular characters.
 */
const uString EngineBase::NativeName(){
	return PACKAGE_STRING;
}

/*! \brief Returns the games native with
 *  \return Native width in pixels
 */
const int EngineBase::NativeWidth(){
	return width;
}

/*! \brief Returns the games native height
 *  \return Native height in pixels
 */
const int EngineBase::NativeHeight(){
	return height;
}

