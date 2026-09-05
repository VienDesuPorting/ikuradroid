/*! \class uString
 *  \brief Custom string class
 *  \todo Interface uString with libiconv and/or icu for encoding support
 *
 *  uString is an encoding and locale agnostic container class for text
 *  objects. It is UNICODE compliant and mimics the usual std::string and
 *  std::wstring classes in the design. It accepts ANSI or UTF8 text directly,
 *  and converts any other encodings to UTF8 before storing it as such. The
 *  base class uses the lightweight utf8-cpp headers as a backend, so it has
 *  no heavy dependencies on its own, but provides the EXTERNTOUTF8() method to
 *  transparantly interface heavier libraries such as libiconv or icu to
 *  handle and convert different text encodings. Such conversions are
 *  controlled by the Cfg settings.
 *
 *  uString is restrictive by design in order to enforce unicode compliant
 *  usage, but a variety of convenience functions are provided. The UNICODE
 *  awareness cannot be disabled per say, but the API will be reduced to
 *  force compiler warnings/errors and system related functions will call
 *  ANSI methods rather than their wide string counterparts.
 *
 *  uString should *only* be used for encoded text strings. Please use the
 *  aString (Which is actually std::string) or a regular buffer for handling
 *  script data and similar mixed/binary sources.
 */
#ifndef _USTRING_H_
#define _USTRING_H_

#include <SDL.h>
#include <stdarg.h>
#include <strings.h>
#include <string>

#define VILE_UNICODE_SAFE	0
#include "config.h"

// Native handlers
#define aString				std::string
#define wString				std::wstring

class uString {
	protected:
		// Internal UTF8 handling
		aString buffer;
		wString wbuffer;
		aString CHARTOUTF8(const Uint8 *Src,Uint32 &Bytes,Uint32 &Points);
		aString WIDETOUTF8(const wchar_t *Src,Uint32 &Bytes,Uint32 &Points);

		// Optimalizations
		unsigned int cbytes;
		unsigned int cpoints;
	public:
		// ANSI compliant Constructors and operators
		uString();
		uString(const aString *Src);
		uString(const char *Src);
		uString(const aString &Src);
		uString(const uString &Src);
		uString(const char &Src);
		uString & operator=(const uString &Src);
		uString & operator=(const aString &Src);
		uString & operator=(const char *Src);
		uString & operator=(const char &Src);
		uString & operator+=(const uString &Src);
		uString & operator+=(const aString &Src);
		uString & operator+=(const char *Src);
		uString & operator+=(const char &Src);
		uString operator+(const uString &Src);
		uString operator+(const aString &Src);
		uString operator+(const char *Src);
		uString operator+(const char &Src);
		bool operator==(const uString &Src);
		bool operator==(const aString &Src);
		bool operator==(const char *Src);
		bool operator==(const char &Src);
		bool operator!=(const uString &Src);
		bool operator!=(const aString &Src);
		bool operator!=(const char *Src);
		bool operator!=(const char &Src);

#ifdef VILE_BUILD_UNICODE
		// Unicode compliant set of constructors and operators
		uString(const wString *Src);
		uString(const wchar_t *Src);
		uString(const wString &Src);
		uString(const wchar_t &Src);
		uString & operator=(const wString &Src);
		uString & operator=(const wchar_t *Src);
		uString & operator=(const wchar_t &Src);
		uString & operator+=(const wchar_t *Src);
		uString & operator+=(const wString &Src);
		uString & operator+=(const wchar_t &Src);
		uString operator+(const wString &Src);
		uString operator+(const wchar_t *Src);
		uString operator+(const wchar_t &Src);
		bool operator==(const wString &Src);
		bool operator==(const wchar_t *Src);
		bool operator==(const wchar_t &Src);
		bool operator!=(const wString &Src);
		bool operator!=(const wchar_t *Src);
		bool operator!=(const wchar_t &Src);
#endif

		// std::string compability
		uString operator[](const unsigned int Index) const;
		uString substr(int Start,int Length=-1) const;
		int compare(const uString &str) const;
		const char *c_str() const;
		const wchar_t *c_wstr();
		unsigned int length() const;
		unsigned int bytes() const;
		unsigned int points() const;
		unsigned int points(const aString) const;

		// String utilities
		int to_int(int Base=10) const;
		double to_double() const;
		uString to_lower() const;
		uString to_upper() const;
		Uint32 to_code() const;
		int find_first(const uString &Text,int Start=0,int Length=-1) const;
		int find_last(const uString &Text,int Start=0,int Length=-1) const;

		// Encoding/conversion API
		aString EXTERNTOUTF8(const Uint8 *Src,const unsigned int Length);
		aString UTF8TOUTF8(const Uint8 *Src,const unsigned int Length);
		aString UTF16TOUTF8(const Uint16 *Src,const unsigned int Length);
		aString UTF32TOUTF8(const Uint32 *Src,const unsigned int Length);

};

#endif

