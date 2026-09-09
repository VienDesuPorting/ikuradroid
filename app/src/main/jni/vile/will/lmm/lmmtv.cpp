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

        // Unlike Critical Point (winbase0.wip + winbase0.msk), LMM splits
        // the dialog window into winbase0.wip (image) and winip0.msk (mask)
	SDL_Surface **mask=Engine->EngineVN::LoadAnimation("winip0","msk");
	SDL_Surface **winbase=Engine->EngineVN::LoadAnimation("winbase0","wip");

        // Merge the mask into the alpha channel of every frame
        if(mask && winbase){
                for(int i=0;mask[i] && winbase[i];i++){
                        int w=mask[i]->w<winbase[i]->w?mask[i]->w:winbase[i]->w;
                        int h=mask[i]->h<winbase[i]->h?mask[i]->h:winbase[i]->h;
                        if(SDL_MUSTLOCK(mask[i])){
                                if(SDL_LockSurface(mask[i])<0){
                                        break;
                                }
                        }
                        if(SDL_MUSTLOCK(winbase[i])){
                                if(SDL_LockSurface(winbase[i])<0){
                                        SDL_UnlockSurface(mask[i]);
                                        break;
                                }
                        }
                        for(int y=0;y<h;y++){
                                Uint8 *srcp=static_cast<Uint8*>(mask[i]->pixels)+
                                                (y*mask[i]->pitch);
                                Uint8 *dstp=static_cast<Uint8*>(winbase[i]->pixels)+
                                                (y*winbase[i]->pitch);
                                for(int x=0;x<w;x++){
                                        dstp[x*winbase[i]->format->BytesPerPixel+3]=
                                                        srcp[x*mask[i]->format->BytesPerPixel];
                                }
                        }
                        if(SDL_MUSTLOCK(mask[i])){
                                SDL_UnlockSurface(mask[i]);
                        }
                        if(SDL_MUSTLOCK(winbase[i])){
                                SDL_UnlockSurface(winbase[i]);
                        }
                }
        }
        if(mask){
                for(int i=0;mask[i];i++){
                        SDL_FreeSurface(mask[i]);
                }
                delete [] mask;
        }

        // Position the dialog window over the background frame
        if(winbase){
                int ypos=0;
                for(int i=0;winbase[i];i++){
                        if(Engine->GetImagePosition("winbase0",i,&rect)){
                                if(i==LMMTV_BACKGROUND){
                                        ypos=Engine->NativeHeight()-rect.h;
                                        SetTextPosition(50,60,540,100);
                                        MoveDialog(rect.x+40,ypos);
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

