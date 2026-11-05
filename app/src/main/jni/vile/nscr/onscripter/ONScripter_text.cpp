/* -*- C++ -*-
 *
 *  ONScripter_text.cpp - Text parser of ONScripter
 *
 *  Copyright (c) 2001-2020 Ogapee. All rights reserved.
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

#include "ONScripter.h"
#ifdef ANDROID
#include <android/log.h>
#endif

extern unsigned short convSJIS2UTF16(unsigned short in);

#define IS_ROTATION_REQUIRED(x)                                          \
    (!IS_TWO_BYTE(*(x)) ||                                               \
     (*(x) == (char)0x81 && *((x) + 1) == (char)0x50) ||                 \
     (*(x) == (char)0x81 && *((x) + 1) == (char)0x51) ||                 \
     (*(x) == (char)0x81 && *((x) + 1) >= 0x5b && *((x) + 1) <= 0x5d) || \
     (*(x) == (char)0x81 && *((x) + 1) >= 0x60 && *((x) + 1) <= 0x64) || \
     (*(x) == (char)0x81 && *((x) + 1) >= 0x69 && *((x) + 1) <= 0x7a) || \
     (*(x) == (char)0x81 && *((x) + 1) == (char)0x80))

#define IS_TRANSLATION_REQUIRED(x) \
    (*(x) == (char)0x81 && *((x) + 1) >= 0x41 && *((x) + 1) <= 0x44)

void ONScripter::shiftHalfPixelX(SDL_Surface* surface)
{
    SDL_LockSurface(surface);
    unsigned char* buf = (unsigned char*)surface->pixels;
    for (int i = surface->h; i != 0; i--) {
        unsigned char c = buf[0];
        for (int j = 1; j < surface->w; j++) {
            buf[j - 1] = (buf[j] + c) >> 1;
            c = buf[j];
        }
        buf[surface->w - 1] = c >> 1;
        buf += surface->pitch;
    }
    SDL_UnlockSurface(surface);
}

void ONScripter::shiftHalfPixelY(SDL_Surface* surface)
{
    SDL_LockSurface(surface);
    for (int j = surface->w - 1; j >= 0; j--) {
        unsigned char* buf = (unsigned char*)surface->pixels + j;
        unsigned char c = buf[0];
        for (int i = 1; i < surface->h; i++) {
            buf += surface->pitch;
            *(buf - surface->pitch) = (*buf + c) >> 1;
            c = *buf;
        }
        *buf = c >> 1;
    }
    SDL_UnlockSurface(surface);
}

/* -------------------------------------------------------------------------
 * Unicode escape sequence support for CP1251 mode.
 *
 * Russian fan-translations of NScripter games (e.g. Higanbana by 07th
 * Expansion) encode Unicode codepoints as a 6-byte escape inside backtick
 * (1-byte char) blocks:
 *
 *   0xDA '0' '0' '1' '7' '1'   ->  U+00AB  «  (left guillemet)
 *   0xDA '0' '0' '1' '8' '7'   ->  U+00BB  »  (right guillemet)
 *   0xDA '1' '2' '3' '0' '4'   ->  U+3010  【  (CJK bracket)
 *
 * The 0xDA byte is 'Ъ' in CP1251 (Cyrillic hard sign), which is extremely
 * rare in Russian text and therefore safe to use as an escape marker.
 *
 * The original ONScripter-RU handles this natively; the stock ogapee
 * ONScripter does not, so we add it here.
 * ------------------------------------------------------------------------- */
static inline bool isUnicodeEscapeCP1251(const char* text)
{
    return (unsigned char)text[0] == 0xDA &&
           text[1] >= '0' && text[1] <= '9' &&
           text[2] >= '0' && text[2] <= '9' &&
           text[3] >= '0' && text[3] <= '9' &&
           text[4] >= '0' && text[4] <= '9' &&
           text[5] >= '0' && text[5] <= '9';
}

static inline unsigned short parseUnicodeEscape(const char* text)
{
    unsigned short code = 0;
    for (int i = 1; i <= 5; i++)
        code = (unsigned short)(code * 10 + (text[i] - '0'));
    return code;
}

int ONScripter::drawGlyph(SDL_Surface* dst_surface, FontInfo* info, SDL_Color& color, char* text, int xy[2], AnimationInfo* cache_info, SDL_Rect* clip, SDL_Rect& dst_rect)
{
    unsigned short unicode;

    /* Detect CP1251 Unicode escape: 0xDA + 5 decimal digits */
    if (script_h.enc.getEncoding() == Encoding::CODE_CP1251 &&
        isUnicodeEscapeCP1251(text)) {
        unicode = parseUnicodeEscape(text);
    }
    else {
        unicode = script_h.enc.getUTF16(text);
    }

    int minx, maxx, miny, maxy, advanced;
#if 0
    if (TTF_GetFontStyle( (TTF_Font*)info->ttf_font[0] ) !=
        (info->is_bold?TTF_STYLE_BOLD:TTF_STYLE_NORMAL) )
        TTF_SetFontStyle( (TTF_Font*)info->ttf_font[0], (info->is_bold?TTF_STYLE_BOLD:TTF_STYLE_NORMAL));
#endif
    TTF_GlyphMetrics((TTF_Font*)info->ttf_font[0], unicode,
                     &minx, &maxx, &miny, &maxy, &advanced);
    // printf("min %d %d %d %d %d %d\n", minx, maxx, miny, maxy, advanced,TTF_FontAscent((TTF_Font*)info->ttf_font[0])  );

    static SDL_Color fcol = {0xff, 0xff, 0xff}, bcol = {0, 0, 0};
    SDL_Surface* tmp_surface = TTF_RenderGlyph_Shaded((TTF_Font*)info->ttf_font[0], unicode, fcol, bcol);

    SDL_Color scolor = {0, 0, 0};
    SDL_Surface* tmp_surface_s = tmp_surface;
    if (info->is_shadow && render_font_outline) {
        unsigned char max_color = color.r;
        if (max_color < color.g) max_color = color.g;
        if (max_color < color.b) max_color = color.b;
        if (max_color < 0x80)
            scolor.r = 0xff;
        else
            scolor.r = 0;
        scolor.g = scolor.b = scolor.r;

        tmp_surface_s = TTF_RenderGlyph_Shaded((TTF_Font*)info->ttf_font[1], unicode, fcol, bcol);
        if (tmp_surface && tmp_surface_s) {
            if ((tmp_surface_s->w - tmp_surface->w) & 1) shiftHalfPixelX(tmp_surface_s);
            if ((tmp_surface_s->h - tmp_surface->h) & 1) shiftHalfPixelY(tmp_surface_s);
        }
    }

    bool rotate_flag = false;
    if (info->getTateyokoMode() == FontInfo::TATE_MODE && IS_ROTATION_REQUIRED(text)) rotate_flag = true;

    dst_rect.x = xy[0] + minx;
    // Position the glyph vertically.
    // For CP932 (Japanese), glyphs are square — use TTF_FontAscent with centering.
    // For CP1251/UTF-8 (1-byte), center each glyph vertically in the cell.
    // This keeps all glyphs on the same vertical center line, preventing
    // letters like "б" or "." from jumping relative to "а" or "о".
    if (script_h.enc.getEncoding() == Encoding::CODE_CP932) {
        dst_rect.y = xy[1] + TTF_FontAscent((TTF_Font*)info->ttf_font[0]) - maxy;
        dst_rect.y -= (TTF_FontHeight((TTF_Font*)info->ttf_font[0]) - info->font_size_xy[1] * screen_ratio1 / screen_ratio2) / 2;
    }
    else {
        // For 1-byte encodings: center each glyph in the font cell.
        // maxy from TTF_GlyphMetrics is in pixels (already scaled to font_size).
        int font_size_px = info->font_size_xy[1] * screen_ratio1 / screen_ratio2;
        int maxy_px = maxy * font_size_px / 1000;
        dst_rect.y = xy[1] + (font_size_px - maxy_px) / 2;
        // Raise the entire text line by a fixed amount to match ONScripter-RU
        // positioning (text sits higher in the cell). This is a constant offset
        // applied to ALL glyphs equally, so letters don't jump relative to
        // each other. /3 gives ~3px more lift than /5 (matching the ~3px gap
        // observed between our build and ONScripter-RU).
        dst_rect.y -= font_size_px / 3;
    }

    if (rotate_flag) dst_rect.x += miny - minx;

    if (info->getTateyokoMode() == FontInfo::TATE_MODE && IS_TRANSLATION_REQUIRED(text)) {
        dst_rect.x += info->font_size_xy[0] / 2;
        dst_rect.y -= info->font_size_xy[0] / 2;
    }

    if (info->is_shadow && tmp_surface_s) {
        SDL_Rect dst_rect_s = dst_rect;
        if (render_font_outline) {
            dst_rect_s.x -= (tmp_surface_s->w - tmp_surface->w) / 2;
            dst_rect_s.y -= (tmp_surface_s->h - tmp_surface->h) / 2;
        }
        else {
            dst_rect_s.x += shade_distance[0];
            dst_rect_s.y += shade_distance[1];
        }

        if (rotate_flag) {
            dst_rect_s.w = tmp_surface_s->h;
            dst_rect_s.h = tmp_surface_s->w;
        }
        else {
            dst_rect_s.w = tmp_surface_s->w;
            dst_rect_s.h = tmp_surface_s->h;
        }

        if (cache_info)
            cache_info->blendText(tmp_surface_s, dst_rect_s.x, dst_rect_s.y, scolor, clip, rotate_flag);

        if (dst_surface)
            alphaBlendText(dst_surface, dst_rect_s, tmp_surface_s, scolor, clip, rotate_flag);
    }

    if (tmp_surface) {
        if (rotate_flag) {
            dst_rect.w = tmp_surface->h;
            dst_rect.h = tmp_surface->w;
        }
        else {
            dst_rect.w = tmp_surface->w;
            dst_rect.h = tmp_surface->h;
        }

        if (cache_info)
            cache_info->blendText(tmp_surface, dst_rect.x, dst_rect.y, color, clip, rotate_flag);

        if (dst_surface)
            alphaBlendText(dst_surface, dst_rect, tmp_surface, color, clip, rotate_flag);
    }

    if (tmp_surface_s && tmp_surface_s != tmp_surface)
        SDL_FreeSurface(tmp_surface_s);
    if (tmp_surface)
        SDL_FreeSurface(tmp_surface);

    return advanced;
}

void ONScripter::openFont(FontInfo* fi)
{
    if (fi->ttf_font[0] == NULL) {
        if (fi->openFont(font_file, screen_ratio1, screen_ratio2) == NULL) {
            __android_log_print(ANDROID_LOG_ERROR, "ONS",
                                "can't open font file: %s", font_file);
            quit();
            exit(-1);
        }
    }
#if defined(PSP)
    else
        fi->openFont(font_file, screen_ratio1, screen_ratio2);
#endif
}

void ONScripter::drawChar(char* text, FontInfo* info, bool flush_flag, bool lookback_flag, SDL_Surface* surface, AnimationInfo* cache_info, SDL_Rect* clip)
{
    // printf("draw %x-%x[%s] %d, %d\n", text[0], text[1], text, info->xy[0], info->xy[1] );
    openFont(info);

    if (info->isEndOfLine()) {
        info->newLine();
        for (int i = 0; i < indent_offset; i++) {
            if (lookback_flag) {
                if (script_h.enc.getEncoding() == Encoding::CODE_CP932) {
                    current_page->add(0x81);
                    current_page->add(0x40);
                }
                else {
                    current_page->add(0xe3);
                    current_page->add(0x80);
                    current_page->add(0x80);
                }
            }
            info->advanceCharInHankaku(2);
        }
    }

    info->old_xy[0] = info->x(false);
    info->old_xy[1] = info->y(false);

    /* text2 must hold up to 6 bytes for CP1251 Unicode escapes (0xDA + 5 digits) */
    char text2[8] = {};
    int n = script_h.enc.getBytes(text[0]);

    /* Check for CP1251 Unicode escape sequence (6 bytes representing 1 glyph) */
    bool is_unicode_escape = false;
    if (script_h.enc.getEncoding() == Encoding::CODE_CP1251 &&
        (unsigned char)text[0] == 0xDA &&
        text[1] >= '0' && text[1] <= '9' &&
        text[2] >= '0' && text[2] <= '9' &&
        text[3] >= '0' && text[3] <= '9' &&
        text[4] >= '0' && text[4] <= '9' &&
        text[5] >= '0' && text[5] <= '9') {
        is_unicode_escape = true;
        n = 6;
    }

    for (int i = 0; i < n; i++)
        text2[i] = text[i];

    for (int i = 0; i < 2; i++) {
        int xy[2];
        xy[0] = info->x() * screen_ratio1 / screen_ratio2;
        xy[1] = info->y() * screen_ratio1 / screen_ratio2;

        SDL_Color color = {info->color[0], info->color[1], info->color[2]};
        SDL_Rect dst_rect;
        float adv = drawGlyph(surface, info, color, text2, xy, cache_info, clip, dst_rect);
        // Note: ogapee's original code subtracts 0.5 here to compensate for
        // FT_CEIL rounding up the advance width. However, this makes letters
        // too tight for CP1251/UTF-8 (1-byte) text. ONScripter-RU doesn't
        // apply this correction, so we skip it for non-CP932 encodings to
        // match their letter spacing.
        if (n == 1 && script_h.enc.getEncoding() == Encoding::CODE_CP932) adv -= 0.5;

        if (surface == accumulation_surface &&
            !flush_flag &&
            (!clip || AnimationInfo::doClipping(&dst_rect, clip) == 0)) {
            dirty_rect.add(dst_rect);
        }
        else if (flush_flag) {
            if (info->is_shadow) {
                if (render_font_outline)
                    info->addShadeArea(dst_rect, -1, -1, 2, 2);
                else
                    info->addShadeArea(dst_rect, 0, 0, shade_distance[0], shade_distance[1]);
            }
            flushDirect(dst_rect, REFRESH_NONE_MODE);
        }

        if (n >= 2) {
            if (is_unicode_escape) {
                /* Unicode escape renders as a single proportional glyph */
                adv += info->pitch_xy[0] - info->font_size_xy[0];
                info->advanceCharInHankaku(adv * 2.0 / info->pitch_xy[0]);
            }
            else if (script_h.enc.getEncoding() == Encoding::CODE_UTF8) {
                // handle a proportional font
                adv += info->pitch_xy[0] - info->font_size_xy[0];
                info->advanceCharInHankaku(adv * 2.0 / info->pitch_xy[0]);
            }
            else
                info->advanceCharInHankaku(2);
            break;
        }
        // For single-byte encodings (CP1251, ASCII), advance by the actual
        // glyph width so characters are spaced proportionally, not fixed-width.
        if (script_h.enc.getEncoding() == Encoding::CODE_UTF8 ||
            script_h.enc.getEncoding() == Encoding::CODE_CP1251) {
            // handle a proportional font
            adv += info->pitch_xy[0] - info->font_size_xy[0];
            info->advanceCharInHankaku(adv * 2.0 / info->pitch_xy[0]);
        }
        else
            info->advanceCharInHankaku(1);
        // For 1-byte encodings, we only draw ONE character per drawChar call.
        // The loop was designed for SJIS (2-byte chars) where it draws both
        // bytes. For CP1251/UTF-8, break after the first iteration.
        break;
    }

    if (lookback_flag) {
        if (is_unicode_escape) {
            /* Store the raw escape bytes in lookback; the lookback renderer
             * uses the same drawChar() which will detect and parse the escape. */
            for (int i = 0; i < 6; i++)
                current_page->add(text[i]);
        }
        else {
            for (int i = 0; i < n; i++)
                current_page->add(text[i]);
        }
    }
}

void ONScripter::drawString(const char* str, uchar3 color, FontInfo* info, bool flush_flag, SDL_Surface* surface, SDL_Rect* rect, AnimationInfo* cache_info, bool pack_hankaku)
{
    int i;

    int start_xy[2];
    start_xy[0] = info->xy[0];
    start_xy[1] = info->xy[1];

    /* ---------------------------------------- */
    /* Draw selected characters */
    uchar3 org_color;
    for (i = 0; i < 3; i++) org_color[i] = info->color[i];
    for (i = 0; i < 3; i++) info->color[i] = color[i];

    bool skip_whitespace_flag = true;
    if (script_h.enc.getEncoding() == Encoding::CODE_UTF8 ||
        script_h.enc.getEncoding() == Encoding::CODE_CP1251)
        skip_whitespace_flag = false;
    /* NOTE: for CP1251 this flag MUST be off. The game's backlog renders
     * strings via logsp -> drawString, and a lookback page never contains
     * the '`' text marker (processText toggles 1-byte mode without storing
     * it), so the flag would otherwise stay true and swallow EVERY space of
     * the Russian backlog text ("Полнаялунатайтвсебе..."). */
    char text[8] = {};
    while (*str) {
        while (*str == ' ' && skip_whitespace_flag) str++;

#ifdef ENABLE_1BYTE_CHAR
        if (*str == '`') {
            str++;
            skip_whitespace_flag = false;
            continue;
        }
#endif
        if (script_h.enc.getEncoding() == Encoding::CODE_UTF8 &&
            *str == script_h.enc.getTextMarker()) {
            str++;
            continue;
        }

#ifndef FORCE_1BYTE_CHAR
        if (cache_info && !cache_info->is_tight_region) {
            if (*str == '(') {
                startRuby(str + 1, *info);
                str++;
                continue;
            }
            else if (*str == '/' && ruby_struct.stage == RubyStruct::BODY) {
                info->addLineOffset(ruby_struct.margin);
                str = ruby_struct.ruby_end;
                if (*ruby_struct.ruby_end == ')') {
                    endRuby(false, false, NULL, cache_info);
                    str++;
                }
                continue;
            }
            else if (*str == ')' && ruby_struct.stage == RubyStruct::BODY) {
                ruby_struct.stage = RubyStruct::NONE;
                str++;
                continue;
            }
            else if (*str == '<') {
                str++;
                int no = 0;
                while (*str >= '0' && *str <= '9')
                    no = no * 10 + (*str++) - '0';
                in_textbtn_flag = true;
                continue;
            }
            else if (*str == '>' && in_textbtn_flag) {
                str++;
                in_textbtn_flag = false;
                continue;
            }
        }
#endif
        int n = script_h.enc.getBytes(*str);
        if (n >= 2) {
            if (checkLineBreak(str, info)) {
                info->newLine();
                for (int i = 0; i < indent_offset; i++)
                    info->advanceCharInHankaku(2);
            }

            for (int i = 0; i < n; i++)
                text[i] = *str++;
            drawChar(text, info, false, false, surface, cache_info);
        }
        else if (*str == 0x0a || (*str == '\\' && info->is_newline_accepted)) {
            info->newLine();
            str++;
        }
        else if (script_h.enc.getEncoding() == Encoding::CODE_UTF8 &&
                 *str == '~') {
            while (1) {
                str++;
                if (*str == 0x0a || *str == 0) break;
                if (*str == '~') {
                    str++;
                    break;
                }
                if (*str == 'i') {
                    openFont(info);
                    info->toggleStyle(TTF_STYLE_ITALIC);
                }
                else if (*str == 'b') {
                    openFont(info);
                    info->toggleStyle(TTF_STYLE_BOLD);
                }
            }
            continue;
        }
        else if (*str) {
            /* Check for CP1251 Unicode escape (0xDA + 5 digits) */
            if (script_h.enc.getEncoding() == Encoding::CODE_CP1251 &&
                (unsigned char)*str == 0xDA &&
                str[1] >= '0' && str[1] <= '9' &&
                str[2] >= '0' && str[2] <= '9' &&
                str[3] >= '0' && str[3] <= '9' &&
                str[4] >= '0' && str[4] <= '9' &&
                str[5] >= '0' && str[5] <= '9') {
                if (checkLigatureLineBreak(str, info))
                    info->newLine();
                for (int i = 0; i < 6; i++)
                    text[i] = *str++;
                text[6] = 0;
                drawChar(text, info, false, false, surface, cache_info);
            }
            else {
                if (checkLigatureLineBreak(str, info))
                    info->newLine();
                text[0] = *str++;
                if (*str && *str != 0x0a && pack_hankaku &&
                    script_h.enc.getEncoding() == Encoding::CODE_CP932)
                    text[1] = *str++;
                else
                    text[1] = 0;
                drawChar(text, info, false, false, surface, cache_info);
            }
        }
    }
    for (i = 0; i < 3; i++) info->color[i] = org_color[i];

    /* ---------------------------------------- */
    /* Calculate the area of selection */
    SDL_Rect clipped_rect = info->calcUpdatedArea(start_xy, screen_ratio1, screen_ratio2);

    SDL_Rect scaled_clipped_rect;
    scaled_clipped_rect.x = clipped_rect.x * screen_ratio1 / screen_ratio2;
    scaled_clipped_rect.y = clipped_rect.y * screen_ratio1 / screen_ratio2;
    scaled_clipped_rect.w = clipped_rect.w * screen_ratio1 / screen_ratio2;
    scaled_clipped_rect.h = clipped_rect.h * screen_ratio1 / screen_ratio2;

    if (info->is_shadow) {
        if (render_font_outline)
            info->addShadeArea(scaled_clipped_rect, -1, -1, 2, 2);
        else
            info->addShadeArea(scaled_clipped_rect, 0, 0, shade_distance[0], shade_distance[1]);
    }

    if (flush_flag)
        flush(refresh_shadow_text_mode, &scaled_clipped_rect);

    if (rect) *rect = clipped_rect;
}

void ONScripter::restoreTextBuffer(SDL_Surface* surface)
{
    text_info.fill(0, 0, 0, 0);

    char out_text[8] = {};
    FontInfo f_info = sentence_font;
    f_info.clear();
    for (int i = 0; i < current_page->text_count; i++) {
        if (current_page->text[i] == 0x0a) {
            f_info.newLine();
        }
        else {
            out_text[0] = current_page->text[i];
#ifndef FORCE_1BYTE_CHAR
            if (out_text[0] == '(') {
                startRuby(current_page->text + i + 1, f_info);
                continue;
            }
            else if (out_text[0] == '/' && ruby_struct.stage == RubyStruct::BODY) {
                f_info.addLineOffset(ruby_struct.margin);
                i = ruby_struct.ruby_end - current_page->text - 1;
                if (*ruby_struct.ruby_end == ')') {
                    endRuby(false, false, surface, &text_info);
                    i++;
                }
                continue;
            }
            else if (out_text[0] == ')' && ruby_struct.stage == RubyStruct::BODY) {
                ruby_struct.stage = RubyStruct::NONE;
                continue;
            }
            else if (out_text[0] == '<') {
                int no = 0;
                while (current_page->text[i + 1] >= '0' && current_page->text[i + 1] <= '9')
                    no = no * 10 + current_page->text[(i++) + 1] - '0';
                in_textbtn_flag = true;
                continue;
            }
            else if (out_text[0] == '>' && in_textbtn_flag) {
                in_textbtn_flag = false;
                continue;
            }
#endif

            int n = script_h.enc.getBytes(out_text[0]);

            /* Check for CP1251 Unicode escape in lookback buffer */
            bool is_unicode_escape = false;
            if (script_h.enc.getEncoding() == Encoding::CODE_CP1251 &&
                (unsigned char)out_text[0] == 0xDA &&
                i + 5 < current_page->text_count &&
                current_page->text[i + 1] >= '0' && current_page->text[i + 1] <= '9' &&
                current_page->text[i + 2] >= '0' && current_page->text[i + 2] <= '9' &&
                current_page->text[i + 3] >= '0' && current_page->text[i + 3] <= '9' &&
                current_page->text[i + 4] >= '0' && current_page->text[i + 4] <= '9' &&
                current_page->text[i + 5] >= '0' && current_page->text[i + 5] <= '9') {
                is_unicode_escape = true;
                n = 6;
                for (int j = 1; j < 6; j++)
                    out_text[j] = current_page->text[i + j];
                out_text[6] = 0;
                i += 5;
            }
            else if (n >= 2) {
                for (int j = 1; j < n; j++)
                    out_text[j] = current_page->text[i + j];

                if (checkLineBreak(current_page->text + i, &f_info))
                    f_info.newLine();
                i += n - 1;
            }
            else if (script_h.enc.getEncoding() == Encoding::CODE_UTF8 &&
                     out_text[0] == '~') {
                while (1) {
                    char ch = current_page->text[++i];
                    if (ch == 0x0a || ch == 0) break;
                    if (ch == '~') {
                        i++;
                        break;
                    }
                    if (ch == 'i') {
                        openFont(&f_info);
                        f_info.toggleStyle(TTF_STYLE_ITALIC);
                    }
                    else if (ch == 'b') {
                        openFont(&f_info);
                        f_info.toggleStyle(TTF_STYLE_BOLD);
                    }
                }
                continue;
            }
            else {
                if (checkLigatureLineBreak(current_page->text + i, &f_info))
                    f_info.newLine();

                out_text[1] = 0;

                if (!is_unicode_escape &&
                    i + 1 != current_page->text_count &&
                    current_page->text[i + 1] != 0x0a &&
                    script_h.enc.getEncoding() == Encoding::CODE_CP932) {
                    out_text[1] = current_page->text[i + 1];
                    i++;
                }
            }
            drawChar(out_text, &f_info, false, false, surface, &text_info);
        }
    }
}

void ONScripter::enterTextDisplayMode(bool text_flag)
{
    if (line_enter_status <= 1 && (!pretextgosub_label || saveon_flag) && internal_saveon_flag && text_flag) {
        storeSaveFile();
        internal_saveon_flag = false;
    }

    if (!(display_mode & DISPLAY_MODE_TEXT)) {
        dirty_rect.clear();
        dirty_rect.add(sentence_font_info.pos);
        display_mode = DISPLAY_MODE_TEXT;

        if (setEffect(&window_effect)) return;
        while (doEffect(&window_effect, false))
            ;

        text_on_flag = true;
    }
}

void ONScripter::leaveTextDisplayMode(bool force_leave_flag)
{
    if (display_mode & DISPLAY_MODE_TEXT &&
        (force_leave_flag || erase_text_window_mode != 0)) {
        dirty_rect.add(sentence_font_info.pos);
        display_mode = DISPLAY_MODE_NORMAL;

        if (setEffect(&window_effect)) return;
        while (doEffect(&window_effect, false))
            ;
    }
}

bool ONScripter::doClickEnd()
{
    bool ret = false;

    refresh_shadow_text_mode |= REFRESH_CURSOR_MODE;

    if (automode_flag) {
        event_mode = WAIT_TEXT_MODE | WAIT_INPUT_MODE | WAIT_VOICE_MODE | WAIT_TIMER_MODE;
        if (automode_time < 0)
            ret = waitEvent(-automode_time * num_chars_in_sentence);
        else
            ret = waitEvent(automode_time);
    }
    else if (autoclick_time > 0) {
        event_mode = WAIT_TIMER_MODE;
        ret = waitEvent(autoclick_time);
    }
    else {
        event_mode = WAIT_TEXT_MODE | WAIT_INPUT_MODE | WAIT_TIMER_MODE;
        ret = waitEvent(-1);
    }

    num_chars_in_sentence = 0;

    refresh_shadow_text_mode &= ~REFRESH_CURSOR_MODE;
    stopAnimation(clickstr_state);

    return ret;
}

bool ONScripter::clickWait(char* out_text)
{
    flush(REFRESH_NONE_MODE);
    skip_mode &= ~SKIP_TO_EOL;

    string_buffer_offset += script_h.checkClickstr(script_h.getStringBuffer() + string_buffer_offset);

    if ((skip_mode & (SKIP_NORMAL | SKIP_TO_EOP) || ctrl_pressed_status) && !textgosub_label) {
        clickstr_state = CLICK_NONE;
        if (out_text) {
            drawChar(out_text, &sentence_font, false, true, accumulation_surface, &text_info);
        }
        else { // called on '@'
            flush(refreshMode());
        }
        num_chars_in_sentence = 0;

        event_mode = IDLE_EVENT_MODE;
        if (waitEvent(0)) return false;
    }
    else {
        if (out_text) {
            drawChar(out_text, &sentence_font, true, true, accumulation_surface, &text_info);
            num_chars_in_sentence++;
        }

        while ((!(script_h.getEndStatus() & ScriptHandler::END_1BYTE_CHAR) &&
                script_h.getStringBuffer()[string_buffer_offset] == ' ') ||
               script_h.getStringBuffer()[string_buffer_offset] == '\t') string_buffer_offset++;

        if (textgosub_label) {
            saveon_flag = false;

            textgosub_clickstr_state = CLICK_WAIT;
            if (script_h.getStringBuffer()[string_buffer_offset] == 0x0)
                textgosub_clickstr_state |= CLICK_EOL;
            gosubReal(textgosub_label, script_h.getWait(), true);

            event_mode = IDLE_EVENT_MODE;
            waitEvent(0);

            return false;
        }

        // if this is the end of the line, pretext becomes enabled
        if (script_h.getStringBuffer()[string_buffer_offset] == 0x0)
            line_enter_status = 0;

        clickstr_state = CLICK_WAIT;
        if (doClickEnd()) return false;

        clickstr_state = CLICK_NONE;

        if (pagetag_flag) processEOT();
        page_enter_status = 0;
    }

    return true;
}

bool ONScripter::clickNewPage(char* out_text)
{
    if (out_text) {
        drawChar(out_text, &sentence_font, true, true, accumulation_surface, &text_info);
        num_chars_in_sentence++;
    }

    flush(REFRESH_NONE_MODE);
    skip_mode &= ~SKIP_TO_EOL;

    string_buffer_offset += script_h.checkClickstr(script_h.getStringBuffer() + string_buffer_offset);

    if ((skip_mode & SKIP_NORMAL || ctrl_pressed_status) && !textgosub_label) {
        num_chars_in_sentence = 0;
        clickstr_state = CLICK_NEWPAGE;

        event_mode = IDLE_EVENT_MODE;
        if (waitEvent(0)) return false;
    }
    else {
        while ((!(script_h.getEndStatus() & ScriptHandler::END_1BYTE_CHAR) &&
                script_h.getStringBuffer()[string_buffer_offset] == ' ') ||
               script_h.getStringBuffer()[string_buffer_offset] == '\t') string_buffer_offset++;

        if (textgosub_label) {
            saveon_flag = false;

            textgosub_clickstr_state = CLICK_NEWPAGE;
            gosubReal(textgosub_label, script_h.getWait(), true);

            event_mode = IDLE_EVENT_MODE;
            waitEvent(0);

            return false;
        }

        // if this is the end of the line, pretext becomes enabled
        if (script_h.getStringBuffer()[string_buffer_offset] == 0x0)
            line_enter_status = 0;

        clickstr_state = CLICK_NEWPAGE;
        if (doClickEnd()) return false;
    }

    newPage();
    clickstr_state = CLICK_NONE;
    pending_newline = false;  // newPage() clears the screen, no pending newline needed

    return true;
}

void ONScripter::startRuby(const char* buf, FontInfo& info)
{
    ruby_struct.stage = RubyStruct::BODY;
    ruby_font = info;
    ruby_font.ttf_font[0] = NULL;
    ruby_font.ttf_font[1] = NULL;
    if (ruby_struct.font_size_xy[0] != -1)
        ruby_font.font_size_xy[0] = ruby_struct.font_size_xy[0];
    else
        ruby_font.font_size_xy[0] = info.font_size_xy[0] / 2;
    if (ruby_struct.font_size_xy[1] != -1)
        ruby_font.font_size_xy[1] = ruby_struct.font_size_xy[1];
    else
        ruby_font.font_size_xy[1] = info.font_size_xy[1] / 2;

    ruby_struct.body_count = 0;
    ruby_struct.ruby_count = 0;

    while (1) {
        if (*buf == '/') {
            ruby_struct.stage = RubyStruct::RUBY;
            ruby_struct.ruby_start = buf + 1;
            buf++;
        }
        else if (*buf == ')' || *buf == '\0') {
            break;
        }
        else {
            int n = script_h.enc.getBytes(*buf);
            if (ruby_struct.stage == RubyStruct::BODY)
                ruby_struct.body_count += (n == 1) ? 1 : 2;
            else if (ruby_struct.stage == RubyStruct::RUBY)
                ruby_struct.ruby_count += (n == 1) ? 1 : 2;
            buf += n;
        }
    }
    ruby_struct.ruby_end = buf;
    ruby_struct.stage = RubyStruct::BODY;
    ruby_struct.margin = ruby_font.initRuby(info, ruby_struct.body_count / 2, ruby_struct.ruby_count / 2);
}

void ONScripter::endRuby(bool flush_flag, bool lookback_flag, SDL_Surface* surface, AnimationInfo* cache_info)
{
    char out_text[8] = {};
    if (sentence_font.rubyon_flag) {
        ruby_font.clear();
        const char* buf = ruby_struct.ruby_start;
        while (buf < ruby_struct.ruby_end) {
            int n = 2;
            if (script_h.enc.getEncoding() == Encoding::CODE_UTF8)
                n = script_h.enc.getBytes(buf[0]);
            for (int i = 0; i < n; i++)
                out_text[i] = buf[i];
            drawChar(out_text, &ruby_font, flush_flag, lookback_flag, surface, cache_info);
            buf += n;
        }
    }
    ruby_struct.stage = RubyStruct::NONE;
}

int ONScripter::textCommand()
{
    if (line_enter_status <= 1 && (!pretextgosub_label || saveon_flag) && internal_saveon_flag) {
        storeSaveFile();
        internal_saveon_flag = false;
    }

    char* buf = script_h.getStringBuffer();

    bool tag_flag = true;
    unsigned short unicode = script_h.enc.getUTF16("y", Encoding::CODE_CP932);
    int n = script_h.enc.getBytes(buf[string_buffer_offset]);
    if (buf[string_buffer_offset] == '[')
        string_buffer_offset++;
    else if (zenkakko_flag &&
             script_h.enc.getUTF16(buf + string_buffer_offset) == unicode)
        string_buffer_offset += n;
    else
        tag_flag = false;

    int start_offset = string_buffer_offset;
    int end_offset = start_offset;
    while (tag_flag && buf[string_buffer_offset]) {
        unsigned short unicode = script_h.enc.getUTF16("z", Encoding::CODE_CP932);
        int n = script_h.enc.getBytes(buf[string_buffer_offset]);
        if (zenkakko_flag &&
            script_h.enc.getUTF16(buf + string_buffer_offset) == unicode) {
            end_offset = string_buffer_offset;
            string_buffer_offset += n;
            break;
        }
        else if (buf[string_buffer_offset] == ']') {
            end_offset = string_buffer_offset;
            string_buffer_offset++;
            break;
        }
        else
            string_buffer_offset += n;
    }

    if (pretextgosub_label &&
        !last_nest_info->pretextgosub_flag &&
        (!pagetag_flag || page_enter_status == 0) &&
        line_enter_status == 0) {
        if (current_page->tag) delete[] current_page->tag;
        if (end_offset > start_offset) {
            int len = end_offset - start_offset;
            current_page->tag = new char[len + 1];
            memcpy(current_page->tag, buf + start_offset, len);
            current_page->tag[len] = 0;
        }
        else {
            current_page->tag = NULL;
        }

        saveon_flag = false;
        pretext_buf = script_h.getCurrent();
        gosubReal(pretextgosub_label, script_h.getCurrent(), false, true);
        line_enter_status = 1;

        return RET_CONTINUE;
    }

    enterTextDisplayMode();

#ifdef USE_LUA
    if (lua_handler.isCallbackEnabled(LUAHandler::LUA_TEXT)) {
        if (lua_handler.callFunction(true, "text"))
            errorAndExit(lua_handler.error_str);
        processEOT();
    }
    else
#endif
        while (processText())
            ;

    return RET_CONTINUE;
}

bool ONScripter::checkLineBreak(const char* buf, FontInfo* fi)
{
    if (!is_kinsoku) return false;

    // check start kinsoku
    int n = script_h.enc.getBytes(buf[0]);
    if (isStartKinsoku(buf + n) ||
        (buf[n] == '_' && isStartKinsoku(buf + n + 1))) {
        const char* buf2 = buf + n;
        if (buf2[0] == '_') buf2++;
        int i = 2;
        while (!fi->isEndOfLine(i)) {
            n = script_h.enc.getBytes(buf2[0]);
            if (buf2[n] == 0x0a || buf2[n] == 0)
                break;
            else if (script_h.enc.getBytes(buf2[n]) == 1)
                buf2++;
            else if (isStartKinsoku(buf2 + n)) {
                i += 2;
                buf2 += n;
            }
            else
                break;
        }

        if (fi->isEndOfLine(i)) return true;
    }

    // check end kinsoku
    if (isEndKinsoku(buf)) {
        const char* buf2 = buf + n;
        int i = 2;
        while (!fi->isEndOfLine(i)) {
            n = script_h.enc.getBytes(buf2[0]);
            if (buf2[n] == 0x0a || buf2[n] == 0)
                break;
            else if (script_h.enc.getBytes(buf2[n]) == 1)
                buf2++;
            else if (isEndKinsoku(buf2 + n)) {
                i += 2;
                buf2 += n;
            }
            else
                break;
        }

        if (fi->isEndOfLine(i)) return true;
    }

    return false;
}

bool ONScripter::checkLigatureLineBreak(const char* buf, FontInfo* fi)
{
    if (script_h.current_language != 0) return false;

    openFont(fi);

    int w = 0;
    while (buf[0] != ' ' && buf[0] != 0x0a && buf[0] != 0 &&
           buf[0] != script_h.enc.getTextMarker()) {
        int n = script_h.enc.getBytes(buf[0]);
        unsigned short unicode = script_h.enc.getUTF16(buf);

        int minx, maxx, miny, maxy, advanced;
        TTF_GlyphMetrics((TTF_Font*)fi->ttf_font[0], unicode,
                         &minx, &maxx, &miny, &maxy, &advanced);

        w += advanced + fi->pitch_xy[0] - fi->font_size_xy[0];
        buf += n;
    }
    w -= fi->pitch_xy[0] - fi->font_size_xy[0];

    return fi->isEndOfLine(w * 2.0 / fi->pitch_xy[0]);
}

void ONScripter::processEOT()
{
    if (skip_mode & SKIP_TO_EOL) {
        flush(refreshMode());
        skip_mode &= ~SKIP_TO_EOL;
    }

    if (!sentence_font.isLineEmpty() && !new_line_skip_flag) {
        // if sentence_font.isLineEmpty() is true, newPage() might be already issued
        if (!sentence_font.isEndOfLine()) current_page->add(0x0a);

        /* In 1-byte char mode (backtick/CP1251/UTF-8), the @ or \ that follows
         * the text may be a SEPARATE token. If we call newLine() now, it resets
         * xy[0] to 0, and the cursor (drawn by clickWait/clickNewPage) would
         * appear at the START of the next line instead of the END of the text.
         * Defer newLine() to the next processText() call if the next token
         * turns out to be text, not a clickstr. */
        if (script_h.getEndStatus() & ScriptHandler::END_1BYTE_CHAR) {
            pending_newline = true;
        }
        else {
            sentence_font.newLine();
        }
    }

    if (!new_line_skip_flag && !pagetag_flag && line_enter_status == 2) line_enter_status = 0;
}

bool ONScripter::processText()
{
    // printf("textCommand %c %d %d %d\n", script_h.getStringBuffer()[ string_buffer_offset ], string_buffer_offset, event_mode, line_enter_status);
    char out_text[8] = {};

    // printf("*** textCommand %d (%d,%d)\n", string_buffer_offset, sentence_font.xy[0], sentence_font.xy[1]);

    // Skip leading spaces/tabs — but ONLY for CP932 (Japanese), where spaces between
    // words are not significant. For CP1251 (Russian) and UTF-8 with 1-byte mode,
    // spaces are meaningful and must be rendered.
    while (script_h.enc.getEncoding() == Encoding::CODE_CP932 &&
           !(script_h.getEndStatus() & ScriptHandler::END_1BYTE_CHAR) &&
           (script_h.getStringBuffer()[string_buffer_offset] == ' ' ||
            script_h.getStringBuffer()[string_buffer_offset] == '\t')) {
        string_buffer_offset++;
    }
    // Also skip tabs for all encodings (tabs are never rendered)
    while (script_h.getStringBuffer()[string_buffer_offset] == '\t') {
        string_buffer_offset++;
    }

    if (script_h.getStringBuffer()[string_buffer_offset] == 0x00) {
        processEOT();
        return false;
    }

    /* If processEOT() deferred newLine() (1-byte char mode), execute it now
     * — but ONLY if the current character is NOT a clickstr (@ or \).
     * If it IS a clickstr, the cursor must stay at the END of the text. */
    if (pending_newline) {
        char ch = script_h.getStringBuffer()[string_buffer_offset];
        if (ch != '@' && ch != '\\') {
            sentence_font.newLine();
        }
        pending_newline = false;
    }

    line_enter_status = 2;
    if (pagetag_flag) page_enter_status = 1;

    new_line_skip_flag = false;

    char ch = script_h.getStringBuffer()[string_buffer_offset];
    int n = script_h.enc.getBytes(ch);
    if (n >= 2) {
        /* ---------------------------------------- */
        /* Kinsoku process */
        if (checkLineBreak(script_h.getStringBuffer() + string_buffer_offset, &sentence_font)) {
            sentence_font.newLine();
            for (int i = 0; i < indent_offset; i++) {
                if (script_h.enc.getEncoding() == Encoding::CODE_CP932) {
                    current_page->add(0x81);
                    current_page->add(0x40);
                }
                else {
                    current_page->add(0xe3);
                    current_page->add(0x80);
                    current_page->add(0x80);
                }
                sentence_font.advanceCharInHankaku(2);
            }
        }

        for (int i = 0; i < n; i++)
            out_text[i] = script_h.getStringBuffer()[string_buffer_offset + i];

        if (script_h.checkClickstr(&script_h.getStringBuffer()[string_buffer_offset]) > 0) {
            if (sentence_font.getRemainingLine() <= clickstr_line)
                return clickNewPage(out_text);
            else
                return clickWait(out_text);
        }
        else {
            clickstr_state = CLICK_NONE;
        }

        if (skip_mode || ctrl_pressed_status) {
            drawChar(out_text, &sentence_font, false, true, accumulation_surface, &text_info);
        }
        else {
            drawChar(out_text, &sentence_font, true, true, accumulation_surface, &text_info);

            event_mode = WAIT_TIMER_MODE | WAIT_INPUT_MODE;
            if (sentence_font.wait_time == -1)
                waitEvent(default_text_speed[text_speed_no]);
            else
                waitEvent(sentence_font.wait_time);
        }

        num_chars_in_sentence++;
        string_buffer_offset += n;

        return true;
    }
    else if (ch == '@') { // wait for click
        return clickWait(NULL);
    }
    else if (ch == '\\') { // new page
        return clickNewPage(NULL);
    }
    else if (ch == '_') { // Ignore an immediate click wait
        string_buffer_offset++;

        int matched_len = script_h.checkClickstr(script_h.getStringBuffer() + string_buffer_offset, true);
        if (matched_len > 0) {
            out_text[0] = script_h.getStringBuffer()[string_buffer_offset];
            if (out_text[0] != '@' && out_text[0] != '\\') {
                for (int i = 1; i < matched_len; i++)
                    out_text[i] = script_h.getStringBuffer()[string_buffer_offset + i];
                bool flush_flag = true;
                if (skip_mode || ctrl_pressed_status) flush_flag = false;
                drawChar(out_text, &sentence_font, flush_flag, true, accumulation_surface, &text_info);
            }
            string_buffer_offset += matched_len;
        }

        return true;
    }
    else if (ch == '!' && !(script_h.getEndStatus() & ScriptHandler::END_1BYTE_CHAR)) {
        string_buffer_offset++;
        if (script_h.getStringBuffer()[string_buffer_offset] == 's') {
            string_buffer_offset++;
            if (script_h.getStringBuffer()[string_buffer_offset] == 'd') {
                sentence_font.wait_time = -1;
                string_buffer_offset++;
            }
            else {
                int t = 0;
                while (script_h.getStringBuffer()[string_buffer_offset] >= '0' &&
                       script_h.getStringBuffer()[string_buffer_offset] <= '9') {
                    t = t * 10 + script_h.getStringBuffer()[string_buffer_offset] - '0';
                    string_buffer_offset++;
                }
                sentence_font.wait_time = t;
                while (script_h.getStringBuffer()[string_buffer_offset] == ' ' ||
                       script_h.getStringBuffer()[string_buffer_offset] == '\t') string_buffer_offset++;
            }
        }
        else if (script_h.getStringBuffer()[string_buffer_offset] == 'w' ||
                 script_h.getStringBuffer()[string_buffer_offset] == 'd') {
            bool flag = false;
            if (script_h.getStringBuffer()[string_buffer_offset] == 'd') flag = true;
            string_buffer_offset++;
            int t = 0;
            while (script_h.getStringBuffer()[string_buffer_offset] >= '0' &&
                   script_h.getStringBuffer()[string_buffer_offset] <= '9') {
                t = t * 10 + script_h.getStringBuffer()[string_buffer_offset] - '0';
                string_buffer_offset++;
            }
            while (script_h.getStringBuffer()[string_buffer_offset] == ' ' ||
                   script_h.getStringBuffer()[string_buffer_offset] == '\t') string_buffer_offset++;
            if (!skip_mode && !ctrl_pressed_status) {
                event_mode = WAIT_TIMER_MODE;
                if (flag) event_mode |= WAIT_INPUT_MODE;
                waitEvent(t);
            }
        }
        return true;
    }
    else if (ch == '#') {
        readColor(&sentence_font.color, script_h.getStringBuffer() + string_buffer_offset);
        readColor(&ruby_font.color, script_h.getStringBuffer() + string_buffer_offset);
        string_buffer_offset += 7;
        return true;
    }
    else if (ch == '(' &&
             (!english_mode ||
              !(script_h.getEndStatus() & ScriptHandler::END_1BYTE_CHAR))) {
        current_page->add('(');
        startRuby(script_h.getStringBuffer() + string_buffer_offset + 1, sentence_font);

        string_buffer_offset++;
        return true;
    }
    else if (ch == '/' && !(script_h.getEndStatus() & ScriptHandler::END_1BYTE_CHAR)) {
        if (ruby_struct.stage == RubyStruct::BODY) {
            current_page->add('/');
            sentence_font.addLineOffset(ruby_struct.margin);
            string_buffer_offset = ruby_struct.ruby_end - script_h.getStringBuffer();
            if (*ruby_struct.ruby_end == ')') {
                if (skip_mode || ctrl_pressed_status)
                    endRuby(false, true, accumulation_surface, &text_info);
                else
                    endRuby(true, true, accumulation_surface, &text_info);
                current_page->add(')');
                string_buffer_offset++;
            }

            return true;
        }
        else { // skip new line
            new_line_skip_flag = true;
            string_buffer_offset++;
            if (script_h.getStringBuffer()[string_buffer_offset] != 0x00)
                errorAndExit("'new line' must follow '/'.");
            return true; // skip the following eol
        }
    }
    else if (ch == ')' && !(script_h.getEndStatus() & ScriptHandler::END_1BYTE_CHAR) &&
             ruby_struct.stage == RubyStruct::BODY) {
        current_page->add(')');
        string_buffer_offset++;
        ruby_struct.stage = RubyStruct::NONE;
        return true;
    }
    else if (ch == '<' &&
             (!english_mode ||
              !(script_h.getEndStatus() & ScriptHandler::END_1BYTE_CHAR))) {
        current_page->add('<');
        string_buffer_offset++;
        int no = 0;
        while (script_h.getStringBuffer()[string_buffer_offset] >= '0' &&
               script_h.getStringBuffer()[string_buffer_offset] <= '9') {
            current_page->add(script_h.getStringBuffer()[string_buffer_offset]);
            no = no * 10 + script_h.getStringBuffer()[string_buffer_offset++] - '0';
        }
        in_textbtn_flag = true;
        return true;
    }
    else if (ch == '>' && in_textbtn_flag &&
             (!english_mode ||
              !(script_h.getEndStatus() & ScriptHandler::END_1BYTE_CHAR))) {
        current_page->add('>');
        string_buffer_offset++;
        in_textbtn_flag = false;
        return true;
    }
    else if ((script_h.enc.getEncoding() == Encoding::CODE_UTF8 ||
              script_h.enc.getEncoding() == Encoding::CODE_CP1251) &&
             ch == script_h.enc.getTextMarker()) {
        int status = script_h.getEndStatus();
        if (status & ScriptHandler::END_1BYTE_CHAR)
            script_h.setEndStatus(status & ~ScriptHandler::END_1BYTE_CHAR, true);
        else
            script_h.setEndStatus(ScriptHandler::END_1BYTE_CHAR);
        string_buffer_offset++;

        return true;
    }
    else if (script_h.enc.getEncoding() == Encoding::CODE_UTF8 &&
             ch == '~') {
        /* NOTE: CP1251 must NOT enter this branch. This is the UTF-8 inline
         * italic/bold markup (~i...~, ~b...~). In CP1251 scripts '~' is a
         * literal character (e.g. "Ага-а~" in HiganbanaRus): the branch
         * swallowed the tilde (never drawn, never typed out), and in builds
         * without ENABLE_1BYTE_CHAR — where backticks reach this loop as
         * plain text — it would also eat the rest of the line up to the
         * next '~' or EOL, including the '@' click-wait. */
        current_page->add('~');
        while (1) {
            ch = script_h.getStringBuffer()[++string_buffer_offset];
            if (ch == 0x0a || ch == 0) break;
            current_page->add(ch);
            if (ch == '~') {
                string_buffer_offset++;
                break;
            }
            if (ch == 'i') {
                openFont(&sentence_font);
                sentence_font.toggleStyle(TTF_STYLE_ITALIC);
            }
            else if (ch == 'b') {
                openFont(&sentence_font);
                sentence_font.toggleStyle(TTF_STYLE_BOLD);
            }
        }

        return true;
    }
    else if (script_h.enc.getEncoding() == Encoding::CODE_CP1251 &&
             (unsigned char)ch == 0xDA &&
             script_h.getStringBuffer()[string_buffer_offset + 1] >= '0' &&
             script_h.getStringBuffer()[string_buffer_offset + 1] <= '9' &&
             script_h.getStringBuffer()[string_buffer_offset + 2] >= '0' &&
             script_h.getStringBuffer()[string_buffer_offset + 2] <= '9' &&
             script_h.getStringBuffer()[string_buffer_offset + 3] >= '0' &&
             script_h.getStringBuffer()[string_buffer_offset + 3] <= '9' &&
             script_h.getStringBuffer()[string_buffer_offset + 4] >= '0' &&
             script_h.getStringBuffer()[string_buffer_offset + 4] <= '9' &&
             script_h.getStringBuffer()[string_buffer_offset + 5] >= '0' &&
             script_h.getStringBuffer()[string_buffer_offset + 5] <= '9') {
        /* CP1251 Unicode escape sequence: 0xDA + 5 decimal digits.
         * Copies all 6 bytes into out_text so drawChar() can detect and render it. */
        for (int i = 0; i < 6; i++)
            out_text[i] = script_h.getStringBuffer()[string_buffer_offset + i];

        bool flush_flag = true;
        if (skip_mode || ctrl_pressed_status)
            flush_flag = false;

        if (checkLigatureLineBreak(script_h.getStringBuffer() + string_buffer_offset, &sentence_font))
            sentence_font.newLine();

        drawChar(out_text, &sentence_font, flush_flag, true, accumulation_surface, &text_info);
        num_chars_in_sentence++;

        if (!skip_mode && !ctrl_pressed_status) {
            event_mode = WAIT_TIMER_MODE | WAIT_INPUT_MODE;
            if (sentence_font.wait_time == -1)
                waitEvent(default_text_speed[text_speed_no]);
            else
                waitEvent(sentence_font.wait_time);
        }

        string_buffer_offset += 6;
        return true;
    }
    else {
        out_text[0] = ch;

        int matched_len = script_h.checkClickstr(script_h.getStringBuffer() + string_buffer_offset);

        if (matched_len > 0) {
            if (matched_len == 2) out_text[1] = script_h.getStringBuffer()[string_buffer_offset + 1];
            if (sentence_font.getRemainingLine() <= clickstr_line)
                return clickNewPage(out_text);
            else
                return clickWait(out_text);
        }
        else if (script_h.getStringBuffer()[string_buffer_offset + 1] &&
                 script_h.checkClickstr(&script_h.getStringBuffer()[string_buffer_offset + 1]) == 1 &&
                 script_h.getEndStatus() & ScriptHandler::END_1BYTE_CHAR) {
            if (script_h.getStringBuffer()[string_buffer_offset + 2] &&
                script_h.checkClickstr(&script_h.getStringBuffer()[string_buffer_offset + 2]) > 0) {
                clickstr_state = CLICK_NONE;
            }
            else if (script_h.getStringBuffer()[string_buffer_offset + 1] == '@') {
                return clickWait(out_text);
            }
            else if (script_h.getStringBuffer()[string_buffer_offset + 1] == '\\') {
                return clickNewPage(out_text);
            }
            else {
                out_text[1] = script_h.getStringBuffer()[string_buffer_offset + 1];
                if (sentence_font.getRemainingLine() <= clickstr_line)
                    return clickNewPage(out_text);
                else
                    return clickWait(out_text);
            }
        }
        else {
            clickstr_state = CLICK_NONE;
        }

        bool flush_flag = true;
        if (skip_mode || ctrl_pressed_status)
            flush_flag = false;
        if (script_h.getStringBuffer()[string_buffer_offset + 1] &&
            !(script_h.getEndStatus() & ScriptHandler::END_1BYTE_CHAR) &&
            script_h.enc.getEncoding() == Encoding::CODE_CP932) {
            out_text[1] = script_h.getStringBuffer()[string_buffer_offset + 1];
            drawChar(out_text, &sentence_font, flush_flag, true, accumulation_surface, &text_info);
            num_chars_in_sentence++;
        }
        else if (script_h.getEndStatus() & ScriptHandler::END_1BYTE_CHAR ||
                 script_h.enc.getEncoding() == Encoding::CODE_UTF8 ||
                 script_h.enc.getEncoding() == Encoding::CODE_CP1251) {
            // UTF-8, CP1251, or 1-byte character mode — single-byte chars.
            if (checkLigatureLineBreak(script_h.getStringBuffer() + string_buffer_offset, &sentence_font))
                sentence_font.newLine();
            drawChar(out_text, &sentence_font, flush_flag, true, accumulation_surface, &text_info);
            num_chars_in_sentence++;
        }

        if (!skip_mode && !ctrl_pressed_status) {
            event_mode = WAIT_TIMER_MODE | WAIT_INPUT_MODE;
            if (sentence_font.wait_time == -1)
                waitEvent(default_text_speed[text_speed_no]);
            else
                waitEvent(sentence_font.wait_time);
        }

        if (script_h.getStringBuffer()[string_buffer_offset + 1] &&
            !(script_h.getEndStatus() & ScriptHandler::END_1BYTE_CHAR) &&
            script_h.enc.getEncoding() == Encoding::CODE_CP932)
            string_buffer_offset++;
        string_buffer_offset++;

        return true;
    }

    return false;
}
