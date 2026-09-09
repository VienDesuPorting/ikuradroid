/*
 * IkuraDroid - Little My Maid engine module
 * Copyright (c) 2026, VienDesu! Porting Team
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

#include "littlemaid.h"
#include "lmmtv.h"

LMMTextview::LMMTextview(LittleMyMaid *Engine) : Textview(Engine) {
        SDL_Rect rect;
        header=0;

        // winbase0.wip frame 0 carries the whole dialog chrome (points bar,
        // translucent body, RECORD/OPEN/STATUS buttons) and winbase0.msk is
        // its 8bpp transparency mask (body=128, chrome=255). LoadMaskedAnimation
        // merges the two exactly like Critical Point does.
        //
        // NB: winip0.msk looks similar but is NOT an alpha mask - it is the
        // mouse clickmap of the dialog window (pixel values are button zone
        // ids 0/1/2/7). Merging it into the alpha channel used to make the
        // whole dialog ~99% transparent, i.e. invisible while the text printer
        // kept drawing straight onto the background.
        SDL_Surface **winbase=Engine->LoadMaskedAnimation("winbase0");

        // The WIPF header stores the on-screen position of every frame; for
        // the LMM build the background frame sits at (0,0) covering the top
        // 145 pixels of the screen - matching the PC original where the
        // dialog window is docked under the top screen edge
        if(winbase){
                for(int i=0;winbase[i];i++){
                        if(Engine->GetImagePosition("winbase0",i,&rect)){
                                if(i==LMMTV_BACKGROUND){
                                        SetTextPosition(18,28,505,100);
                                        MoveDialog(rect.x,rect.y);
                                        Resize(rect.w,rect.h);
                                        Set(winbase[i]);
                                }
                                if(i==LMMTV_HEADER){
                                        // Speaker name plate (guarded: LMM winbase0 may
                                        // carry fewer frames than the CP build)
                                        header=new Printer(rect.x,rect.y,winbase[i]->w,
                                                        winbase[i]->h);
                                        header->Blit(winbase[i]);
                                }
                        }
                        SDL_FreeSurface(winbase[i]);
                }
                delete [] winbase;
        }
}

LMMTextview::~LMMTextview(){
}

void LMMTextview::PrintText(uString Title,uString Text){
        if(header){
                header->Clear();
                header->Print(Title.c_str(),0);
                header->SetVisible(true);
        }
        Textview::PrintText(Text);
}

void LMMTextview::PrintText(uString Text){
        if(header){
                header->SetVisible(false);
        }
        Textview::PrintText(Text);
}

