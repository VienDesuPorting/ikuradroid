/* -*- C++ -*-
 *
 *  Encoding.h - Character encoding handler
 *
 *  Copyright (c) 2019-2020 Ogapee. All rights reserved.
 *
 *  ogapee@aqua.dti2.ne.jp
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
/*
 *  IkuraDroid modification notice - 2026
 *
 *  This file is modified for ONScripter-Droid, the NScripter/ONScripter
 *  engine module of IkuraDroid (su.viende.ikuradroid) - the ViLE-based
 *  Android visual novel player.
 *
 *  Base: ONScripter 20230825 by Ogapee. This fork was made with
 *  reference to the outdated ONScripterRU Android build and carries
 *  CP1251 changes for Russian fan translations.
 *  Modified for IkuraDroid by Christopher_VN <github.com/christopher-vn>
 *  as part of the VienDesuPorting/ikuradroid project
 *  <github.com/VienDesuPorting/ikuradroid>.
 *
 *  Distributed under the same GNU GPL (v2 or later) as the original
 *  ONScripter; this notice supplements the original copyright above.
 */

#ifndef __ENCODING_H__
#define __ENCODING_H__

class Encoding {
public:
    enum { CODE_CP932 = 0,
           CODE_UTF8 = 1,
           CODE_CP1251 = 2   // Windows-1251 (Cyrillic) — for Russian fan translations
    };

    Encoding();
    ~Encoding();

    void setEncoding(int code);
    int getEncoding() { return code; };

    char getTextMarker();

    int getBytes(unsigned char ch, int code = -1);
    int getNum(const unsigned char* buf);

    // Returns true if `ch` is the first byte of a multi-byte character.
    // For CP932: checks SJIS lead-byte ranges (0x81-0x9F, 0xE0-0xFC).
    // For UTF-8: checks UTF-8 lead bytes (>= 0xC0).
    // For CP1251: always false (single-byte encoding).
    bool isTwoByte(unsigned char ch) {
        return getBytes(ch) > 1;
    }

    unsigned short getUTF16(const char* text, int code = -1);

private:
    int code;
};

#endif // __ENCODING_H__
