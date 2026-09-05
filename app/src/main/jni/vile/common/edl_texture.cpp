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

#include "edl_texture.h"
#include "edl_gfx.h"

EDLTexture::EDLTexture()
{
        //Initialize
        mTexture = NULL;
        gFont = NULL;
        mWidth = 0;
        mHeight = 0;
        mPixels = NULL;
        mPitch = 0;
}

EDLTexture::~EDLTexture()
{
        //Deallocate
        free();
}

bool EDLTexture::loadFromFile( std::string path )
{
        //Get rid of preexisting texture
        free();

        //The final texture
        SDL_Texture* newTexture = NULL;

        //Load image at specified path
        SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
        if( loadedSurface == NULL )
        {
                printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
        }
        else
        {
                //Convert surface to display format
                SDL_Surface* formattedSurface = SDL_ConvertSurfaceFormat( loadedSurface, SDL_PIXELFORMAT_RGBA8888, NULL );
                if( formattedSurface == NULL )
                {
                        printf( "Unable to convert loaded surface to display format! %s\n", SDL_GetError() );
                }
                else
                {
                        //Create blank streamable texture
                        newTexture = SDL_CreateTexture( EDLRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, formattedSurface->w, formattedSurface->h );
                        if( newTexture == NULL )
                        {
                                printf( "Unable to create blank texture! SDL Error: %s\n", SDL_GetError() );
                        }
                        else
                        {
                                //Enable blending on texture
                                SDL_SetTextureBlendMode( newTexture, SDL_BLENDMODE_BLEND );

                                //Lock texture for manipulation
                                SDL_LockTexture( newTexture, &formattedSurface->clip_rect, &mPixels, &mPitch );

                                //Copy loaded/formatted surface pixels
                                memcpy( mPixels, formattedSurface->pixels, formattedSurface->pitch * formattedSurface->h );

                                //Get image dimensions
                                mWidth = formattedSurface->w;
                                mHeight = formattedSurface->h;

                                //Get pixel data in editable format
                                Uint32* pixels = (Uint32*)mPixels;
                                int pixelCount = ( mPitch / 4 ) * mHeight;

                                //Map colors
                                Uint32 colorKey = SDL_MapRGB( formattedSurface->format, 0, 0xFF, 0xFF );
                                Uint32 transparent = SDL_MapRGBA( formattedSurface->format, 0x00, 0xFF, 0xFF, 0x00 );

                                //Color key pixels
                                for( int i = 0; i < pixelCount; ++i )
                                {
                                        if( pixels[ i ] == colorKey )
                                        {
                                                pixels[ i ] = transparent;
                                        }
                                }

                                //Unlock texture to update
                                SDL_UnlockTexture( newTexture );
                                mPixels = NULL;
                        }

                        //Get rid of old formatted surface
                        SDL_FreeSurface( formattedSurface );
                }

                //Get rid of old loaded surface
                SDL_FreeSurface( loadedSurface );
        }

        //Return success
        mTexture = newTexture;
        return mTexture != NULL;
}


static void cp1251tUtf1(char *out, const char *in) {
    static const int table[128] = {
        0x82D0,0x83D0,0x9A80E2,0x93D1,0x9E80E2,0xA680E2,0xA080E2,0xA180E2,
        0xAC82E2,0xB080E2,0x89D0,0xB980E2,0x8AD0,0x8CD0,0x8BD0,0x8FD0,
        0x92D1,0x9880E2,0x9980E2,0x9C80E2,0x9D80E2,0xA280E2,0x9380E2,0x9480E2,
        0,0xA284E2,0x99D1,0xBA80E2,0x9AD1,0x9CD1,0x9BD1,0x9FD1,
        0xA0C2,0x8ED0,0x9ED1,0x88D0,0xA4C2,0x90D2,0xA6C2,0xA7C2,
        0x81D0,0xA9C2,0x84D0,0xABC2,0xACC2,0xADC2,0xAEC2,0x87D0,
        0xB0C2,0xB1C2,0x86D0,0x96D1,0x91D2,0xB5C2,0xB6C2,0xB7C2,
        0x91D1,0x9684E2,0x94D1,0xBBC2,0x98D1,0x85D0,0x95D1,0x97D1,
        0x90D0,0x91D0,0x92D0,0x93D0,0x94D0,0x95D0,0x96D0,0x97D0,
        0x98D0,0x99D0,0x9AD0,0x9BD0,0x9CD0,0x9DD0,0x9ED0,0x9FD0,
        0xA0D0,0xA1D0,0xA2D0,0xA3D0,0xA4D0,0xA5D0,0xA6D0,0xA7D0,
        0xA8D0,0xA9D0,0xAAD0,0xABD0,0xACD0,0xADD0,0xAED0,0xAFD0,
        0xB0D0,0xB1D0,0xB2D0,0xB3D0,0xB4D0,0xB5D0,0xB6D0,0xB7D0,
        0xB8D0,0xB9D0,0xBAD0,0xBBD0,0xBCD0,0xBDD0,0xBED0,0xBFD0,
        0x80D1,0x81D1,0x82D1,0x83D1,0x84D1,0x85D1,0x86D1,0x87D1,
        0x88D1,0x89D1,0x8AD1,0x8BD1,0x8CD1,0x8DD1,0x8ED1,0x8FD1
    };
    while (*in)
        if (*in & 0x80) {
            int v = table[(int)(0x7f & *in++)];
            if (!v)
                continue;
            *out++ = (char)v;
            *out++ = (char)(v >> 8);
            if (v >>= 16)
                *out++ = (char)v;
        }
        else
            *out++ = *in++;
    *out = 0;
}

std::string cp1251tUtf(std::string s) {
    int c,i;
    std::string ns;
    for(i=0; i<s.size(); i++) {
        c=s[i];
        char buf[4], in[2] = {0, 0};
        *in = c;
        cp1251tUtf1(buf, in);
        ns+=std::string(buf);
    }
   return ns;
}

bool EDLTexture::loadFromRenderedText( TTF_Font *Font, const uString &Text,SDL_Color fg)
{
        free();
        //Render text surface
         std::string a = cp1251tUtf(Text.c_str());

        SDL_Surface* textSurface = TTF_RenderUTF8_Blended( Font, a.c_str(), fg );
        if( textSurface == NULL )
        {
                printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
        }
        else
        {
                //Create texture from surface pixels
        mTexture = SDL_CreateTextureFromSurface( EDLRenderer, textSurface );
                if( mTexture == NULL )
                {
                        printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
                }
                else
                {
                        //Get image dimensions
                        mWidth = textSurface->w;
                        mHeight = textSurface->h;
                }

                //Get rid of old surface
                SDL_FreeSurface( textSurface );
        }

        //Return success
        return mTexture != NULL;
}

bool EDLTexture::loadFromSurface( SDL_Surface*  Surface)
{
    free();
        SDL_Texture* newTexture = NULL;

                SDL_Surface* formattedSurface = SDL_ConvertSurfaceFormat( Surface, SDL_PIXELFORMAT_RGBA8888, NULL );
                if( formattedSurface == NULL )
                {
                        printf( "Unable to convert loaded surface to display format! %s\n", SDL_GetError() );
                }
                else
                {
                        //Create blank streamable texture
                        newTexture = SDL_CreateTexture( EDLRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, formattedSurface->w, formattedSurface->h );
                        if( newTexture == NULL )
                        {
                                printf( "Unable to create blank texture! SDL Error: %s\n", SDL_GetError() );
                        }
                        else
                        {
                                //Enable blending on texture
                                SDL_SetTextureBlendMode( newTexture, SDL_BLENDMODE_BLEND );

                                //Lock texture for manipulation
                                SDL_LockTexture( newTexture, &formattedSurface->clip_rect, &mPixels, &mPitch );

                                //Copy loaded/formatted surface pixels
                                memcpy( mPixels, formattedSurface->pixels, formattedSurface->pitch * formattedSurface->h );

                                //Get image dimensions
                                mWidth = formattedSurface->w;
                                mHeight = formattedSurface->h;

                                //Get pixel data in editable format
                                Uint32* pixels = (Uint32*)mPixels;
                                int pixelCount = ( mPitch / 4 ) * mHeight;

                                //Map colors
                                Uint32 colorKey = SDL_MapRGB( formattedSurface->format, 0, 0xFF, 0xFF );
                                Uint32 transparent = SDL_MapRGBA( formattedSurface->format, 0x00, 0xFF, 0xFF, 0x00 );

                                //Color key pixels
                                for( int i = 0; i < pixelCount; ++i )
                                {
                                        if( pixels[ i ] == colorKey )
                                        {
                                                pixels[ i ] = transparent;
                                        }
                                }

                                //Unlock texture to update
                                SDL_UnlockTexture( newTexture );
                                mPixels = NULL;
                        }

                        //Get rid of old formatted surface
                        SDL_FreeSurface( formattedSurface );
                }

                //Get rid of old loaded surface
//              SDL_FreeSurface( loadedSurface );

    mTexture = newTexture;
    return mTexture != NULL;
}

bool EDLTexture::createBlank( int width, int height )
{//free();

        mTexture = SDL_CreateTexture( EDLRenderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, width, height );
        if( mTexture == NULL )
        {
                printf( "Unable to create blank texture! SDL Error: %s\n", SDL_GetError() );
        }
        else
        {
                mWidth = width;
                mHeight = height;
        }
    SDL_SetTextureBlendMode( mTexture, SDL_BLENDMODE_BLEND );
        return mTexture != NULL;
}

void EDLTexture::setAsRenderTarget()
{
    //Make self render target
    SDL_SetRenderTarget( EDLRenderer, mTexture );
}

bool EDLTexture::createTarget( int width, int height )
{
        //Create uninitialized texture
        mTexture = SDL_CreateTexture( EDLRenderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_TARGET, width, height );
        if( mTexture == NULL )
        {
                printf( "Unable to create blank texture! SDL Error: %s\n", SDL_GetError() );
        }
        else
        {
                mWidth = width;
                mHeight = height;
        }
    //SDL_SetTextureBlendMode( mTexture, SDL_BLENDMODE_BLEND );

        return mTexture != NULL;
}

bool EDLTexture::createVideoBlank( int width, int height )
{
        //Create uninitialized texture
        mTexture = SDL_CreateTexture( EDLRenderer, SDL_PIXELFORMAT_YV12,SDL_TEXTUREACCESS_STREAMING, width, height);
        if( mTexture == NULL )
        {
                printf( "Unable to create Video blank texture! SDL Error: %s\n", SDL_GetError() );
        }
        else
        {
                mWidth = width;
                mHeight = height;
        }

        return mTexture != NULL;
}

int EDLTexture::UpdateTexture(const SDL_Rect * rect,const void *pixels0, int pitch0,
                                                    const void *pixels1, int pitch1,
                                                    const void *pixels2, int pitch2)
{
 //return   SDL_UpdateTexture(mTexture, NULL, mPixels, mPitch);
 //return SDL_UpdateTexture( mTexture, rect, (const Uint8 *)pixels0,pitch0);
 return SDL_UpdateYUVTexture( mTexture, rect, (const Uint8 *)pixels0,pitch0, (const Uint8 *)pixels1,pitch1, (const Uint8 *)pixels2,pitch2);

}

void EDLTexture::free()
{
        //Free texture if it exists
        if( mTexture != NULL || mTexture != 0 )
        {

                SDL_DestroyTexture( mTexture );
                mTexture = NULL;
                mWidth = 0;
                mHeight = 0;
        mPixels = NULL;
                mPitch = 0;
        }
}

void EDLTexture::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
        //Modulate texture rgb
        SDL_SetTextureColorMod( mTexture, red, green, blue );
}

void EDLTexture::setBlendMode( SDL_BlendMode blending )
{
        //Set blending function
        SDL_SetTextureBlendMode( mTexture, blending );
}

void EDLTexture::setAlpha( Uint8 alpha )
{
        //Modulate texture alpha
        SDL_SetTextureAlphaMod( mTexture, alpha );
}

void EDLTexture::render( int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip )
{
        //Set rendering space and render to screen
        SDL_Rect renderQuad = { x, y, mWidth, mHeight };

        //Set clip rendering dimensions
        if( clip != NULL )
        {
                renderQuad.w = clip->w;
                renderQuad.h = clip->h;
        }

        //Render to screen
        SDL_RenderCopyEx( EDLRenderer, mTexture, clip, &renderQuad, angle, center, flip );

}
void EDLTexture::render( SDL_Rect* clipin, SDL_Rect* clipdest)
{
    SDL_RenderCopy( EDLRenderer, mTexture, clipin, clipdest);
}

int EDLTexture::getWidth()
{
        return mWidth;
}

int EDLTexture::getHeight()
{
        return mHeight;
}


bool EDLTexture::lockTexture()
{
        bool success = true;

        //Texture is already locked
        if( mPixels != NULL )
        {
                printf( "Texture is already locked!\n" );
                success = false;
        }
        //Lock texture
        else
        {
                if( SDL_LockTexture( mTexture, NULL, &mPixels, &mPitch ) != 0 )
                {
                        printf( "Unable to lock texture! %s\n", SDL_GetError() );
                        success = false;
                }
        }

        return success;
}

bool EDLTexture::unlockTexture()
{
        bool success = true;

        //Texture is not locked
        if( mPixels == NULL )
        {
                printf( "Texture is not locked!\n" );
                success = false;
        }
        //Unlock texture
        else
        {
                SDL_UnlockTexture( mTexture );
                mPixels = NULL;
                mPitch = 0;
        }

        return success;
}

void* EDLTexture::getPixels()
{
        return mPixels;
}

void EDLTexture::copyPixels( void* pixels )
{
        //Texture is locked
        if( mPixels != NULL )
        {
                //Copy to locked pixels
                memcpy( mPixels, pixels, mPitch * mHeight );
        }
}
void EDLTexture::copyPixelsVideo( void* pixels0,void* pixels1, void* pixels2, int siz )
{
    if( mPixels != NULL )
        {
                //Copy to locked pixels
                // clang: no arithmetic on void* (was a GNU extension in old GCC)
                char* dst = (char*)mPixels;
        memcpy(dst,             pixels0, siz   );
        memcpy(dst + siz,       pixels2, siz/4 );
        memcpy(dst + siz*5/4,   pixels1, siz/4 );
        }

}

void EDLTexture::readPixelsRenderer()
{
    SDL_Rect rect = {0,0,mWidth,mHeight};
    lockTexture();
    SDL_RenderReadPixels(EDLRenderer,&rect,
                         SDL_PIXELFORMAT_ABGR8888,
                         mPixels,
                         mPitch);
    unlockTexture();
}

int EDLTexture::getPitch()
{
        return mPitch;
}

Uint32 EDLTexture::getPixel32( unsigned int x, unsigned int y )
{
    //Convert the pixels to 32 bit
    Uint32 *pixels = (Uint32*)mPixels;

    //Get the pixel requested
    return pixels[ ( y * ( mPitch / 4 ) ) + x ];
}
