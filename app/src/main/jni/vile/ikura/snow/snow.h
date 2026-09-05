/*! \class Snow
 *  \brief Loader class for Snow Family 
 */
#ifndef _SNOW_H_
#define _SNOW_H_

#include "../ikuradecoder.h"

class Snow : public IkuraDecoder {
	private:
		void CreateTextview();
	public:
		Snow(uString Path);
};

#endif

