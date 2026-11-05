/* -*- C++ -*-
 *
 *  Encoding.cpp - Character encoding handler
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

#include "Encoding.h"

extern unsigned short convSJIS2UTF16(unsigned short in);
extern unsigned short convUTF8ToUTF16(const char** src);

Encoding::Encoding()
{
    code = CODE_CP932;
}

Encoding::~Encoding()
{
}

void Encoding::setEncoding(int code)
{
    this->code = code;
}

int Encoding::getBytes(unsigned char ch, int code)
{
    if (code == -1) code = this->code;

    if (code == CODE_CP932) {
        if ((ch & 0xe0) == 0xe0 || (ch & 0xe0) == 0x80) return 2;
    }
    else if (code == CODE_CP1251) {
        // CP1251 is a single-byte encoding; every byte = 1 character.
        return 1;
    }
    else {
        if (0 <= ch && ch < 0x80) return 1;
        if (0xc0 <= ch && ch < 0xe0) return 2;
        if (0xe0 <= ch && ch < 0xf0) return 3;
        if (0xf0 <= ch && ch < 0xf8) return 4;
    }

    return 1;
}

int Encoding::getNum(const unsigned char* buf)
{
    int n = 0;

    while (buf[0] != 0) {
        int n2 = getBytes(buf[0]);
        n++;
        if (n2 > 1) n++;
        buf += n2;
    }

    return n;
}

char Encoding::getTextMarker()
{
    if (code == CODE_UTF8)
        return '^';

    return '`';
}

/*
 * Windows-1251 (CP1251) → UTF-16 conversion table.
 * CP1251 maps 0x00-0x7F = ASCII, 0x80-0xFF = Cyrillic + symbols.
 * We map directly to Unicode code points.
 *
 * Source: https://en.wikipedia.org/wiki/Windows-1251
 */
static const unsigned short cp1251_to_utf16[128] = {
    // 0x80 - 0x8F
    0x0402, 0x0403, 0x201A, 0x0453, 0x201E, 0x2026, 0x2020, 0x2021,
    0x20AC, 0x2030, 0x0409, 0x2039, 0x040A, 0x040C, 0x040B, 0x040F,
    // 0x90 - 0x9F  (corrected — was duplicated from 0x80-0x8F)
    0x0452, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014,
    0x02DC, 0x2122, 0x0459, 0x203A, 0x045A, 0x045C, 0x045B, 0x045F,
    // 0xA0 - 0xAF
    0x00A0, 0x040E, 0x045E, 0x0408, 0x00A4, 0x0490, 0x00A6, 0x00A7,
    0x0401, 0x00A9, 0x0404, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x0407,
    // 0xB0 - 0xBF
    0x00B0, 0x00B1, 0x0406, 0x0456, 0x0491, 0x00B5, 0x00B6, 0x00B7,
    0x0451, 0x2116, 0x0454, 0x00BB, 0x0458, 0x0405, 0x0455, 0x0457,
    // 0xC0 - 0xCF  (А-П)
    0x0410, 0x0411, 0x0412, 0x0413, 0x0414, 0x0415, 0x0416, 0x0417,
    0x0418, 0x0419, 0x041A, 0x041B, 0x041C, 0x041D, 0x041E, 0x041F,
    // 0xD0 - 0xDF  (Р-Я)
    0x0420, 0x0421, 0x0422, 0x0423, 0x0424, 0x0425, 0x0426, 0x0427,
    0x0428, 0x0429, 0x042A, 0x042B, 0x042C, 0x042D, 0x042E, 0x042F,
    // 0xE0 - 0xEF  (а-п)
    0x0430, 0x0431, 0x0432, 0x0433, 0x0434, 0x0435, 0x0436, 0x0437,
    0x0438, 0x0439, 0x043A, 0x043B, 0x043C, 0x043D, 0x043E, 0x043F,
    // 0xF0 - 0xFF  (р-я)
    0x0440, 0x0441, 0x0442, 0x0443, 0x0444, 0x0445, 0x0446, 0x0447,
    0x0448, 0x0449, 0x044A, 0x044B, 0x044C, 0x044D, 0x044E, 0x044F
};

unsigned short Encoding::getUTF16(const char* text, int code)
{
    unsigned short unicode = 0;

    if (code == -1) code = this->code;

    if (code == CODE_CP932) {
        if ((text[0] & 0xe0) == 0xe0 || (text[0] & 0xe0) == 0x80) {
            unsigned index = ((unsigned char*)text)[0];
            index = index << 8 | ((unsigned char*)text)[1];
            unicode = convSJIS2UTF16(index);
        }
        else {
            if ((text[0] & 0xe0) == 0xa0 || (text[0] & 0xe0) == 0xc0)
                unicode = ((unsigned char*)text)[0] - 0xa0 + 0xff60;
            else
                unicode = text[0];
        }
    }
    else if (code == CODE_CP1251) {
        unsigned char ch = ((unsigned char*)text)[0];
        if (ch < 0x80) {
            // ASCII — pass through directly
            unicode = ch;
        } else {
            // 0x80-0xFF → Cyrillic / symbols via lookup table
            unicode = cp1251_to_utf16[ch - 0x80];
        }
    }
    else {
        unicode = convUTF8ToUTF16(&text);
    }

    return unicode;
}
