
#ifndef _EDL_TEXTURE_H_
#define _EDL_TEXTURE_H_

#include "edl_common.h"
#include <SDL_ttf.h>
class EDLTexture
{
	public:
		//Initializes variables
		EDLTexture();
		~EDLTexture();
		bool createBlank( int width, int height );
		int UpdateTexture(const SDL_Rect * rect,const void *pixels0, int pitch0,
                                                    const void *pixels1, int pitch1,
                                                    const void *pixels2, int pitch2);
		bool createTarget( int width, int height );
		bool createVideoBlank( int width, int height );
		bool loadFromFile( std::string path );
		bool loadFromSurface( SDL_Surface*  Surface);
		bool loadFromRenderedText( TTF_Font *Font, const uString &Text,SDL_Color fg );
		void free();
		void setColor( Uint8 red, Uint8 green, Uint8 blue );
		void setBlendMode( SDL_BlendMode blending );
		void setAlpha( Uint8 alpha );
        void setAsRenderTarget();

		void render( int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE );
		void render( SDL_Rect* clipin = NULL, SDL_Rect* clipdest = NULL);
		int getWidth();
		int getHeight();

		bool lockTexture();
		bool unlockTexture();
		void* getPixels();
		void copyPixels( void* pixels );
		void copyPixelsVideo( void* pixels0,void* pixels1, void* pixels2, int siz );
		void readPixelsRenderer();
		int getPitch();
		Uint32 getPixel32( unsigned int x, unsigned int y );
	private:
		SDL_Texture* mTexture;
		TTF_Font *gFont;
		void* mPixels;
		int mPitch;
		int mWidth;
		int mHeight;
};


#endif

