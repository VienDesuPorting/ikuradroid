/* -*- C++ -*-
 *
 *  ONScripter_rmenu.cpp - Right click menu handler of ONScripter
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

#include "ONScripter.h"

#include <dirent.h>
#ifndef WIN32
#include <unistd.h>
#endif

/* IkuraDroid: M3 bridge diagnostics - which branch armed the F5/F6
 * injection (the game's own script screens vs the built-in slot lists)
 * and why a fallback fired. Filter logcat with ONS_M3. */
#if defined(ANDROID)
#include <android/log.h>
#define M3_LOG(...) __android_log_print(ANDROID_LOG_INFO, "ONS_M3", __VA_ARGS__)
#else
#define M3_LOG(...) do {} while (0)
#endif

#define DIALOG_W 241
#define DIALOG_H 167
#define DIALOG_HEADER 32
#define DIALOG_FOOTER 64
#define DIALOG_BUTTON_W 96
#define DIALOG_BUTTON_H 42

/* IkuraDroid: this fork is a 1-byte build serving CP1251 scripts only,
 * so the built-in system screens speak Russian. Cyrillic literals are
 * hex-escaped to stay independent of the source file encoding; the
 * leading backtick is the half-width marker the upstream strings carry -
 * drawString strips it under ENABLE_1BYTE_CHAR. */
#if defined(ENABLE_1BYTE_CHAR)
#define MESSAGE_SAVE_EXIST "`%s%s    \xC4\xE0\xF2\xE0 %s/%s    \xC2\xF0\xE5\xEC\xFF %s:%s"
#define MESSAGE_SAVE_EMPTY "`%s%s    ------------------------"
#define MESSAGE_SAVE_CONFIRM "`\xC7\xE0\xEF\xE8\xF1\xE0\xF2\xFC \xE2 %s%s?"
#define MESSAGE_LOAD_CONFIRM "`\xC7\xE0\xE3\xF0\xF3\xE7\xE8\xF2\xFC \xE8\xE7 %s%s?"
#define MESSAGE_RESET_CONFIRM "`\xC2\xE5\xF0\xED\xF3\xF2\xFC\xF1\xFF \xED\xE0 \xF2\xE8\xF2\xF3\xEB\xFC\xED\xFB\xE9 \xFD\xEA\xF0\xE0\xED?"
#define MESSAGE_END_CONFIRM "`\xC2\xFB\xE9\xF2\xE8 \xE8\xE7 \xE8\xE3\xF0\xFB?"
#define MESSAGE_YES "\xC4\xE0"
#define MESSAGE_NO "\xCD\xE5\xF2"
#define MESSAGE_OK "OK"
#define MESSAGE_CANCEL "\xCE\xF2\xEC\xE5\xED\xE0"
#else
#define MESSAGE_SAVE_EXIST "%s%s@%s%sú%s%sª"
#define MESSAGE_SAVE_EMPTY "%s%s@||||||||||||"
#define MESSAGE_SAVE_CONFIRM "%s%sÉZ[uµÜ·Bæëµ¢Å·©H"
#define MESSAGE_LOAD_CONFIRM "%s%sð[hµÜ·Bæëµ¢Å·©H"
#define MESSAGE_RESET_CONFIRM "ZbgµÜ·Bæëµ¢Å·©H"
#define MESSAGE_END_CONFIRM "I¹µÜ·Bæëµ¢Å·©H"
#define MESSAGE_YES "Í¢"
#define MESSAGE_NO "¢¢¦"
#define MESSAGE_OK "nj"
#define MESSAGE_CANCEL "LZ"
#endif

#ifdef ANDROID
#include <stdarg.h>
static int osprintf(char* str, const char* format, ...)
{
    str[0] = 0;
    va_list list;
    va_start(list, format);
    while (*format) {
        if (format[0] == '%' && format[1] == 's') {
            strcat(str, va_arg(list, char*));
            format += 2;
        }
        else {
            strncat(str, format++, 1);
        }
    }
    va_end(list);
    return strlen(str);
}
#define sprintf osprintf
#endif

void ONScripter::enterSystemCall()
{
    shelter_button_link = root_button_link.next;
    root_button_link.next = NULL;
    shelter_select_link = root_select_link.next;
    root_select_link.next = NULL;
    shelter_event_mode = event_mode;
    shelter_mouse_state.x = last_mouse_state.x;
    shelter_mouse_state.y = last_mouse_state.y;
    event_mode = IDLE_EVENT_MODE;
    shelter_display_mode = display_mode;
    display_mode = DISPLAY_MODE_TEXT;

    shelter_refresh_shadow_text_mode = refresh_shadow_text_mode;
    refresh_shadow_text_mode &= ~REFRESH_CURSOR_MODE;
    stopAnimation(clickstr_state);
}

void ONScripter::leaveSystemCall(bool restore_flag)
{
    current_font = &sentence_font;
    display_mode = shelter_display_mode;

    if (restore_flag) {
        current_page = cached_page;
        SDL_BlitSurface(backup_surface, NULL, text_info.image_surface, NULL);
        root_button_link.next = shelter_button_link;
        root_select_link.next = shelter_select_link;

        event_mode = shelter_event_mode;
        refresh_shadow_text_mode = shelter_refresh_shadow_text_mode;
        if (event_mode & WAIT_BUTTON_MODE) {
            int x = shelter_mouse_state.x * screen_device_width / screen_width;
            int y = shelter_mouse_state.y * screen_device_width / screen_width;
            SDL_WarpMouse(x, y);
        }
    }
    dirty_rect.fill(screen_width, screen_height);
    flush(refreshMode());

    // printf("leaveSystemCall %d %d\n",event_mode, clickstr_state);

    refreshMouseOverButton();

    system_menu_mode = SYSTEM_NULL;
}

int ONScripter::executeSystemCall()
{
    /* IkuraDroid: scripts with a textgosub header (07th-style) draw
     * their own save/load screens (*savemode/*loadmode, reached from
     * the right-click pause menu with `savegame`/`loadgame`). When the
     * M3 bridge routed F5/F6 into SYSTEM_SCRIPT_SAVE/LOAD, gosub
     * straight into the game's own screen instead of showing the
     * built-in slot list: ret 1 aborts the parked wait and the parser
     * continues inside the script routine; its final `return` lands on
     * the top of the textgosub label, the same place the script's own
     * post-menu path jumps to. */
    if (system_menu_mode == SYSTEM_SCRIPT_SAVE ||
        system_menu_mode == SYSTEM_SCRIPT_LOAD) {
        /* IkuraDroid: consume the trigger before dispatching - one
         * arming must gosub into the script routine exactly once.
         * Clearing the mode only after the gosub left a window where a
         * duplicated dispatch stacked a second return frame and
         * recaptured the screen after the text had been hidden. */
        bool save = (system_menu_mode == SYSTEM_SCRIPT_SAVE);
        system_menu_mode = SYSTEM_NULL;
        executeSystemScriptMenu(save);
        return 1;
    }

    SDL_BlitSurface(text_info.image_surface, NULL, backup_surface, NULL);

    enterSystemCall();

    while (system_menu_mode != SYSTEM_NULL) {
        dirty_rect.fill(screen_width, screen_height);
        switch (system_menu_mode) {
        case SYSTEM_SKIP:
            executeSystemSkip();
            return 2; // continue parsing text
            break;
        case SYSTEM_RESET:
            if (executeSystemReset()) return 1; // stop parsing text
            break;
        case SYSTEM_SAVE:
            M3_LOG("builtin SAVE screen open");
            executeSystemSave();
            break;
        case SYSTEM_LOAD:
            M3_LOG("builtin LOAD screen open");
            if (executeSystemLoad()) return 1; // stop parsing text
            break;
        case SYSTEM_LOOKBACK:
            executeSystemLookback();
            break;
        case SYSTEM_WINDOWERASE:
            executeWindowErase();
            break;
        case SYSTEM_MENU:
            executeSystemMenu();
            break;
        case SYSTEM_AUTOMODE:
            executeSystemAutomode();
            return 2; // continue parsing text
            break;
        case SYSTEM_END:
            executeSystemEnd();
            break;
        default:
            leaveSystemCall();
        }
    }

    return 0;
}

/* IkuraDroid: builds of the bridge before the dispatch capture let the
 * 07th save routine write its slot thumbnails (screenshot\ss<N>.bmp)
 * while screenshot_w/h still held the init-time full-screen values, so
 * those files are full-screen-sized canvases that the slot list lsp's
 * over the whole page. The script itself only ever writes thumbnails
 * (the rclk path and the bridge both capture 80x60), so a thumbnail
 * matching the full screen size can only be such a stale artifact.
 * Drop the files before the screen lsp's them: the slot falls back to
 * its dummy image until the next save into it writes a proper shot. */
static void purgeOversizedSlotScreenshots(const char* base_path, int screen_w, int screen_h)
{
    char dir[512];
    snprintf(dir, sizeof(dir), "%sscreenshot", base_path);
    DIR* dp = opendir(dir);
    if (!dp) return;

    struct dirent* entp;
    while ((entp = readdir(dp)) != NULL) {
        const char* name = entp->d_name;
        if (strncmp(name, "ss", 2) != 0) continue;
        const char* dot = strrchr(name, '.');
        if (!dot || strcmp(dot, ".bmp") != 0) continue;
        const char* p = name + 2;
        if (p == dot) continue;
        while (p < dot && *p >= '0' && *p <= '9') p++;
        if (p != dot) continue;

        char path[512];
        snprintf(path, sizeof(path), "%s/%s", dir, name);
        FILE* fp = fopen(path, "rb");
        if (!fp) continue;
        unsigned char header[26];
        size_t got = fread(header, 1, sizeof(header), fp);
        fclose(fp);
        if (got < sizeof(header)) continue;
        int w = header[18] | (header[19] << 8) | (header[20] << 16) | ((int)header[21] << 24);
        int h = header[22] | (header[23] << 8) | (header[24] << 16) | ((int)header[25] << 24);
        if (w <= 0 || h <= 0 || w != screen_w || h != screen_h) continue;

        if (unlink(path) == 0)
            M3_LOG("M3 purge: stale full-size slot preview removed: %s (%dx%d)",
                   name, w, h);
    }
    closedir(dp);
}

bool ONScripter::setupScriptSystemMenu(bool save)
{
    /* Script-drawn screens exist only in textgosub games (07th-style
     * header: *savemode / *loadmode subroutines behind the right-click
     * menu, writing saves with the `savegame` command). They are
     * entered from the engine-initiated textgosub wait itself - the
     * top nesting frame must be the textgosub one, which is exactly
     * the state the right-click menu hook runs in on PC. Inside the
     * game's own screens (menu, save, load, lookback - all entered by
     * script gosubs) the top frame is a plain one and the M3 keys
     * stay inert. Everything that fails here falls back to the
     * built-in screens. */
    if (!textgosub_label) {
        M3_LOG("M3 %s fallback: no textgosub_label", save ? "SAVE" : "LOAD");
        return false;
    }

    const char* target = save ? "savemode" : "loadmode";
    if (!script_h.hasLabel(target)) {
        M3_LOG("M3 %s fallback: no *%s label", save ? "SAVE" : "LOAD", target);
        return false;
    }
    if (!script_h.hasLabel(textgosub_label)) {
        M3_LOG("M3 %s fallback: no *%s label", save ? "SAVE" : "LOAD", textgosub_label);
        return false;
    }

    if (!last_nest_info || !last_nest_info->textgosub_flag) {
        M3_LOG("M3 %s fallback: park outside the textgosub frame (nest=%p tgosub=%d)",
               save ? "SAVE" : "LOAD",
               last_nest_info ? (void*)last_nest_info : NULL,
               last_nest_info ? (int)last_nest_info->textgosub_flag : -1);
        return false;
    }

    system_menu_mode = save ? SYSTEM_SCRIPT_SAVE : SYSTEM_SCRIPT_LOAD;
    M3_LOG("M3 %s armed: script screen *%s", save ? "SAVE" : "LOAD", target);
    return true;
}

void ONScripter::executeSystemScriptMenu(bool save)
{
    const char* target = save ? "savemode" : "loadmode";
    if (!script_h.hasLabel(target) || !script_h.hasLabel(textgosub_label)) {
        /* Cannot happen (setupScriptSystemMenu verified the labels
         * before arming the mode, and executeSystemCall consumed the
         * mode before calling here); nothing to unwind. */
        return;
    }

    ScriptHandler::LabelInfo text_li = script_h.lookupLabel(textgosub_label);

    /* The game's own right-click path captures the game screen before
     * hiding the text (*Text_cw_rclk: "getscreenshot 80,60"); the save
     * routine later squeezes that capture into the slot thumbnail file
     * (savescreenshot2 -> screenshot\ss<N>.bmp), which the slot list
     * lsp's at its native size. The bridge bypasses *Text_cw_rclk, so
     * run the capture here with the same 80x60 size; otherwise
     * screenshot_w/h keep their init-time full-screen values and
     * savescreenshot2 writes a full-screen-sized, never-filled BMP
     * that the slot preview then draws over the whole page. */
    captureScreenshot(80, 60);
    purgeOversizedSlotScreenshots(archive_path, screen_width, screen_height);
    M3_LOG("M3 %s dispatch: capture 80x60 done, gosub *%s",
           save ? "SAVE" : "LOAD", target);

    /* Hide the text layer the way the game's own right-click handler
     * does (texthide + textoff in *Text_cw_rclk / *rmenu_csutom): the
     * text is drawn above every sprite, so the screen's own black
     * backdrop would not cover it. Both script screens leave through
     * *save_exit, which runs texton, and the return path re-enters the
     * top of the textgosub label, where the window is re-rendered. */
    if (windowchip_sprite_no >= 0)
        sprite_info[windowchip_sprite_no].visible = false;
    leaveTextDisplayMode(true);
    text_on_flag = false;
    dirty_rect.fill(screen_width, screen_height);
    flush(refreshMode());

    /* gosub into the game's screen routine; the NestInfo frame carries
     * the first instruction of the textgosub label as the return
     * address - the exact target of the script's own "btndef clear :
     * goto *Text_cw" after the pause menu closes. A successful
     * `loadgame` inside the load screen never returns: the interpreter
     * state is replaced by the save, exactly like the built-in load. */
    gosubReal(target, text_li.start_address, false);

    /* The trigger was already consumed by executeSystemCall before it
     * called here; leaving the mode cleared is its job now. */
}

void ONScripter::executeSystemMenu()
{
    current_font = &menu_font;

    if (menuselectvoice_file_name[MENUSELECTVOICE_OPEN])
        playSound(menuselectvoice_file_name[MENUSELECTVOICE_OPEN],
                  SOUND_CHUNK, false, MIX_WAVE_CHANNEL);

    text_info.fill(0, 0, 0, 0);
    flush(refreshMode());

    menu_font.num_xy[0] = rmenu_link_width;
    menu_font.num_xy[1] = rmenu_link_num;
    menu_font.top_xy[0] = (screen_width * screen_ratio2 / screen_ratio1 - menu_font.num_xy[0] * menu_font.pitch_xy[0]) / 2;
    menu_font.top_xy[1] = (screen_height * screen_ratio2 / screen_ratio1 - menu_font.num_xy[1] * menu_font.pitch_xy[1]) / 2;
    menu_font.setXY(0, 0);

    RMenuLink* link = root_rmenu_link.next;
    int counter = 1;
    while (link) {
        ButtonLink* button = getSelectableSentence(link->label, &menu_font, false);
        root_button_link.insert(button);
        button->no = counter++;

        link = link->next;
        flush(refreshMode());
    }

    flushEvent();
    refreshMouseOverButton();

    event_mode = WAIT_BUTTON_MODE;
    do waitEventSub(-1);
    while (current_button_state.button == 0);

    deleteButtonLink();

    if (current_button_state.button == -1) {
        if (menuselectvoice_file_name[MENUSELECTVOICE_CANCEL])
            playSound(menuselectvoice_file_name[MENUSELECTVOICE_CANCEL],
                      SOUND_CHUNK, false, MIX_WAVE_CHANNEL);
        leaveSystemCall();
        return;
    }

    if (menuselectvoice_file_name[MENUSELECTVOICE_CLICK])
        playSound(menuselectvoice_file_name[MENUSELECTVOICE_CLICK],
                  SOUND_CHUNK, false, MIX_WAVE_CHANNEL);

    link = root_rmenu_link.next;
    counter = 1;
    while (link) {
        if (current_button_state.button == counter++) {
            system_menu_mode = link->system_call_no;
            break;
        }
        link = link->next;
    }
}

void ONScripter::executeSystemSkip()
{
    skip_mode |= SKIP_NORMAL;
    leaveSystemCall();
}

void ONScripter::executeSystemAutomode()
{
    automode_flag = true;
    skip_mode &= ~SKIP_NORMAL;
    printf("systemcall_automode: change to automode\n");
    leaveSystemCall();
}

bool ONScripter::executeSystemReset()
{
    if (executeSystemYesNo(SYSTEM_RESET)) {
        resetCommand();
        leaveSystemCall(false);

        return true;
    }

    leaveSystemCall();

    return false;
}

void ONScripter::executeSystemEnd()
{
    if (executeSystemYesNo(SYSTEM_END))
        endCommand();
    else
        leaveSystemCall();
}

void ONScripter::executeWindowErase()
{
    if (windowchip_sprite_no >= 0)
        sprite_info[windowchip_sprite_no].visible = false;

    display_mode = DISPLAY_MODE_NORMAL;
    flush(mode_saya_flag ? REFRESH_SAYA_MODE : REFRESH_NORMAL_MODE);

    event_mode = WAIT_TIMER_MODE | WAIT_BUTTON_MODE;
    waitEventSub(-1);

    if (windowchip_sprite_no >= 0)
        sprite_info[windowchip_sprite_no].visible = true;

    leaveSystemCall();
}

bool ONScripter::executeSystemLoad()
{
    current_font = &menu_font;

    text_info.fill(0, 0, 0, 0);

    int n = script_h.enc.getNum((unsigned char*)save_item_name);
    menu_font.num_xy[0] = (n + 1) / 2 + 2 + 13;
    menu_font.num_xy[1] = num_save_file + 2;
    menu_font.top_xy[0] = (screen_width * screen_ratio2 / screen_ratio1 - menu_font.num_xy[0] * menu_font.pitch_xy[0]) / 2;
    menu_font.top_xy[1] = (screen_height * screen_ratio2 / screen_ratio1 - menu_font.num_xy[1] * menu_font.pitch_xy[1]) / 2;
    int n2 = script_h.enc.getNum((unsigned char*)load_menu_name);
    menu_font.setXY((menu_font.num_xy[0] * 2 - n2) / 4, 0);
    uchar3 color = {0xff, 0xff, 0xff};
    drawString(load_menu_name, color, &menu_font, true, accumulation_surface, NULL, &text_info);
    menu_font.newLine();
    menu_font.newLine();

    flush(refreshMode());

    bool nofile_flag;
    char* buffer = new char[menu_font.num_xy[0] * 4 + 1];

    SaveFileInfo save_file_info;
    for (unsigned int i = 1; i <= num_save_file; i++) {
        searchSaveFile(save_file_info, i);
        menu_font.setXY((menu_font.num_xy[0] * 2 - (n + 15 * 2)) / 4);

        char* format_str = NULL;
        if (save_file_info.valid) {
            setStr(&format_str, MESSAGE_SAVE_EXIST, -1, true);
            sprintf(buffer, format_str,
                    save_item_name,
                    save_file_info.sjis_no,
                    save_file_info.sjis_month,
                    save_file_info.sjis_day,
                    save_file_info.sjis_hour,
                    save_file_info.sjis_minute);
            nofile_flag = false;
        }
        else {
            setStr(&format_str, MESSAGE_SAVE_EMPTY, -1, true);
            sprintf(buffer, format_str,
                    save_item_name,
                    save_file_info.sjis_no);
            nofile_flag = true;
        }
        if (format_str) delete[] format_str;
        ButtonLink* button = getSelectableSentence(buffer, &menu_font, false, nofile_flag);
        root_button_link.insert(button);
        button->no = i;
        flush(refreshMode());
    }
    delete[] buffer;

    refreshMouseOverButton();

    event_mode = WAIT_BUTTON_MODE;
    int file_no = 0;
    while (1) {
        waitEventSub(-1);

        if (current_button_state.button > 0) {
            file_no = current_button_state.button;
            searchSaveFile(save_file_info, file_no);
            if (!save_file_info.valid) continue;
        }

        if (current_button_state.button != 0) break;
    }

    if (current_button_state.button > 0) {
        deleteButtonLink();

        if (executeSystemYesNo(SYSTEM_LOAD, file_no)) {
            current_font = &sentence_font;
            system_menu_mode = 0; // for fadeout in mp3stopCommand()
            if (loadSaveFile(file_no))
                return false;

            leaveSystemCall(false);
            saveon_flag = true;
            internal_saveon_flag = true;
            text_on_flag = false;
            indent_offset = 0;
            line_enter_status = 0;
            page_enter_status = 0;
            string_buffer_offset = 0;
            break_flag = false;

            flushEvent();

#ifdef USE_LUA
            if (lua_handler.isCallbackEnabled(LUAHandler::LUA_LOAD)) {
                if (lua_handler.callFunction(true, "load", &file_no))
                    errorAndExit(lua_handler.error_str);
            }
#endif

            if (loadgosub_label)
                gosubReal(loadgosub_label, script_h.getCurrent());

            return true;
        }

        return false;
    }

    deleteButtonLink();
    leaveSystemCall();

    return false;
}

void ONScripter::executeSystemSave()
{
    current_font = &menu_font;

    text_info.fill(0, 0, 0, 0);

    int n = script_h.enc.getNum((unsigned char*)save_item_name);
    menu_font.num_xy[0] = (n + 1) / 2 + 2 + 13;
    menu_font.num_xy[1] = num_save_file + 2;
    menu_font.top_xy[0] = (screen_width * screen_ratio2 / screen_ratio1 - menu_font.num_xy[0] * menu_font.pitch_xy[0]) / 2;
    menu_font.top_xy[1] = (screen_height * screen_ratio2 / screen_ratio1 - menu_font.num_xy[1] * menu_font.pitch_xy[1]) / 2;
    int n2 = script_h.enc.getNum((unsigned char*)save_menu_name);
    menu_font.setXY((menu_font.num_xy[0] * 2 - n2) / 4, 0);
    uchar3 color = {0xff, 0xff, 0xff};
    drawString(save_menu_name, color, &menu_font, true, accumulation_surface, NULL, &text_info);
    menu_font.newLine();
    menu_font.newLine();

    flush(refreshMode());

    bool nofile_flag;
    char* buffer = new char[menu_font.num_xy[0] * 4 + 1];

    for (unsigned int i = 1; i <= num_save_file; i++) {
        SaveFileInfo save_file_info;
        searchSaveFile(save_file_info, i);
        menu_font.setXY((menu_font.num_xy[0] * 2 - (n + 15 * 2)) / 4);

        char* format_str = NULL;
        if (save_file_info.valid) {
            setStr(&format_str, MESSAGE_SAVE_EXIST, -1, true);
            sprintf(buffer, format_str,
                    save_item_name,
                    save_file_info.sjis_no,
                    save_file_info.sjis_month,
                    save_file_info.sjis_day,
                    save_file_info.sjis_hour,
                    save_file_info.sjis_minute);
            nofile_flag = false;
        }
        else {
            setStr(&format_str, MESSAGE_SAVE_EMPTY, -1, true);
            sprintf(buffer, format_str,
                    save_item_name,
                    save_file_info.sjis_no);
            nofile_flag = true;
        }
        if (format_str) delete[] format_str;
        ButtonLink* button = getSelectableSentence(buffer, &menu_font, false, nofile_flag);
        root_button_link.insert(button);
        button->no = i;
        flush(refreshMode());
    }
    delete[] buffer;

    refreshMouseOverButton();

    event_mode = WAIT_BUTTON_MODE;
    do waitEventSub(-1);
    while (current_button_state.button == 0);

    deleteButtonLink();

    if (current_button_state.button > 0) {
        int file_no = current_button_state.button;
        if (executeSystemYesNo(SYSTEM_SAVE, file_no)) {
            if (saveon_flag && internal_saveon_flag) storeSaveFile();
            writeSaveFile(file_no);
            leaveSystemCall();
        }
        return;
    }

    leaveSystemCall();
}

bool ONScripter::executeSystemYesNo(int caller, int file_no)
{
    current_font = &menu_font;

    text_info.fill(0, 0, 0, 0);
    dirty_rect.fill(screen_width, screen_height);

    char name[128] = {'\0'};
    char* format_str = NULL;
    if (caller == SYSTEM_SAVE) {
        SaveFileInfo save_file_info;
        searchSaveFile(save_file_info, file_no);
        setStr(&format_str, MESSAGE_SAVE_CONFIRM, -1, true);
        sprintf(name, format_str,
                save_item_name,
                save_file_info.sjis_no);
    }
    else if (caller == SYSTEM_LOAD) {
        SaveFileInfo save_file_info;
        searchSaveFile(save_file_info, file_no);
        setStr(&format_str, MESSAGE_LOAD_CONFIRM, -1, true);
        sprintf(name, format_str,
                save_item_name,
                save_file_info.sjis_no);
    }
    else if (caller == SYSTEM_RESET) {
        setStr(&format_str, MESSAGE_RESET_CONFIRM, -1, true);
        strcpy(name, format_str);
    }
    else if (caller == SYSTEM_END) {
        setStr(&format_str, MESSAGE_END_CONFIRM, -1, true);
        strcpy(name, format_str);
    }

    int n = script_h.enc.getNum((unsigned char*)name);
    menu_font.num_xy[0] = (n + 1) / 2;
    menu_font.num_xy[1] = 3;
    menu_font.top_xy[0] = (screen_width * screen_ratio2 / screen_ratio1 - menu_font.num_xy[0] * menu_font.pitch_xy[0]) / 2;
    menu_font.top_xy[1] = (screen_height * screen_ratio2 / screen_ratio1 - menu_font.num_xy[1] * menu_font.pitch_xy[1]) / 2;
    menu_font.setXY(0, 0);
    uchar3 color = {0xff, 0xff, 0xff};
    drawString(name, color, &menu_font, true, accumulation_surface, NULL, &text_info);

    flush(refreshMode());

    int offset1 = n / 5;
    int offset2 = n / 2 - offset1;
    setStr(&format_str, MESSAGE_YES, -1, true);
    strcpy(name, format_str);
    menu_font.setXY(offset1 - 2, 2);
    ButtonLink* button = getSelectableSentence(name, &menu_font, false);
    root_button_link.insert(button);
    button->no = 1;

    setStr(&format_str, MESSAGE_NO, -1, true);
    strcpy(name, format_str);
    menu_font.setXY(offset2, 2);
    button = getSelectableSentence(name, &menu_font, false);
    root_button_link.insert(button);
    button->no = 2;

    if (format_str) delete[] format_str;

    flush(refreshMode());

    refreshMouseOverButton();

    event_mode = WAIT_BUTTON_MODE;
    do waitEventSub(-1);
    while (current_button_state.button == 0);

    deleteButtonLink();

    if (current_button_state.button == 1) { // yes is selected
        if (menuselectvoice_file_name[MENUSELECTVOICE_YES])
            playSound(menuselectvoice_file_name[MENUSELECTVOICE_YES],
                      SOUND_CHUNK, false, MIX_WAVE_CHANNEL);
        return true;
    }
    else {
        if (menuselectvoice_file_name[MENUSELECTVOICE_NO])
            playSound(menuselectvoice_file_name[MENUSELECTVOICE_NO],
                      SOUND_CHUNK, false, MIX_WAVE_CHANNEL);
        return false;
    }
}

void ONScripter::setupLookbackButton()
{
    deleteButtonLink();

    /* ---------------------------------------- */
    /* Previous button check */
    if ((current_page->previous->text_count > 0) &&
        current_page != start_page) {
        ButtonLink* button = new ButtonLink();
        root_button_link.insert(button);

        button->no = 1;

        if (lookback_sp[0] >= 0) {
            button->button_type = ButtonLink::SPRITE_BUTTON;
            button->sprite_no = lookback_sp[0];
            AnimationInfo& si = sprite_info[button->sprite_no];
            si.visible = true;
            button->select_rect = si.pos;
            button->image_rect = si.pos;
        }
        else {
            button->button_type = ButtonLink::LOOKBACK_BUTTON;
            button->select_rect = sentence_font_info.pos;
            button->select_rect.h /= 3;
            button->show_flag = 2;
            button->anim[0] = &lookback_info[0];
            button->anim[1] = &lookback_info[1];
            button->image_rect.x = sentence_font_info.pos.x + sentence_font_info.pos.w - button->anim[0]->pos.w;
            button->image_rect.y = sentence_font_info.pos.y;
            button->image_rect.w = button->anim[0]->pos.w;
            button->image_rect.h = button->anim[0]->pos.h;
            button->anim[0]->pos.x = button->anim[1]->pos.x = button->image_rect.x;
            button->anim[0]->pos.y = button->anim[1]->pos.y = button->image_rect.y;
        }
    }
    else if (lookback_sp[0] >= 0) {
        sprite_info[lookback_sp[0]].visible = false;
    }

    /* ---------------------------------------- */
    /* Next button check */
    if (current_page->next != cached_page) {
        ButtonLink* button = new ButtonLink();
        root_button_link.insert(button);

        button->no = 2;

        if (lookback_sp[1] >= 0) {
            button->button_type = ButtonLink::SPRITE_BUTTON;
            button->sprite_no = lookback_sp[1];
            AnimationInfo& si = sprite_info[button->sprite_no];
            si.visible = true;
            button->select_rect = si.pos;
            button->image_rect = si.pos;
        }
        else {
            button->button_type = ButtonLink::LOOKBACK_BUTTON;
            button->select_rect = sentence_font_info.pos;
            button->select_rect.y += sentence_font_info.pos.h * 2 / 3;
            button->select_rect.h /= 3;
            button->show_flag = 2;
            button->anim[0] = &lookback_info[2];
            button->anim[1] = &lookback_info[3];
            button->image_rect.x = sentence_font_info.pos.x + sentence_font_info.pos.w - button->anim[0]->pos.w;
            button->image_rect.y = sentence_font_info.pos.y + sentence_font_info.pos.h - button->anim[0]->pos.h;
            button->image_rect.w = button->anim[0]->pos.w;
            button->image_rect.h = button->anim[0]->pos.h;
            button->anim[0]->pos.x = button->anim[1]->pos.x = button->image_rect.x;
            button->anim[0]->pos.y = button->anim[1]->pos.y = button->image_rect.y;
        }
    }
    else if (lookback_sp[1] >= 0) {
        sprite_info[lookback_sp[1]].visible = false;
    }
}

void ONScripter::executeSystemLookback()
{
    int i;
    uchar3 color;

    current_font = &sentence_font;

    current_page = current_page->previous;
    if (current_page->text_count == 0) {
        if (lookback_sp[0] >= 0)
            sprite_info[lookback_sp[0]].visible = false;
        if (lookback_sp[1] >= 0)
            sprite_info[lookback_sp[1]].visible = false;
        leaveSystemCall();
        return;
    }

    while (1) {
        setupLookbackButton();
        refreshMouseOverButton();

        dirty_rect.fill(screen_width, screen_height);
        flush(refreshMode() & ~REFRESH_TEXT_MODE);

        for (i = 0; i < 3; i++) {
            color[i] = sentence_font.color[i];
            sentence_font.color[i] = lookback_color[i];
        }
        restoreTextBuffer(accumulation_surface);
        for (i = 0; i < 3; i++) sentence_font.color[i] = color[i];
        flush(REFRESH_NONE_MODE);

        event_mode = WAIT_BUTTON_MODE;
        waitEventSub(-1);

        if (current_button_state.button == 0 ||
            (current_page == start_page &&
             current_button_state.button == -2)) {
            continue;
        }
        if (current_button_state.button == -1 ||
            (current_button_state.button == -3 &&
             current_page->next == cached_page) ||
            current_button_state.button <= -4) {
            event_mode = IDLE_EVENT_MODE;
            deleteButtonLink();
            if (lookback_sp[0] >= 0)
                sprite_info[lookback_sp[0]].visible = false;
            if (lookback_sp[1] >= 0)
                sprite_info[lookback_sp[1]].visible = false;
            leaveSystemCall();
            return;
        }

        if (current_button_state.button == 1 ||
            current_button_state.button == -2) {
            current_page = current_page->previous;
        }
        else
            current_page = current_page->next;
    }
}

void ONScripter::buildDialog(bool yesno_flag, const char* mes1, const char* mes2)
{
    SDL_PixelFormat* fmt = image_surface->format;
    SDL_Surface* s = SDL_CreateRGBSurface(SDL_SWSURFACE, DIALOG_W, DIALOG_H,
                                          fmt->BitsPerPixel, fmt->Rmask, fmt->Gmask, fmt->Bmask, fmt->Amask);

    SDL_Rect rect;
    unsigned char col = 255;
    SDL_FillRect(s, NULL, SDL_MapRGBA(s->format, col, col, col, 0xff));

    rect.x = 2;
    rect.y = DIALOG_HEADER;
    rect.w = DIALOG_W - 4;
    rect.h = DIALOG_H - rect.y - 2;
    col = 105;
    SDL_FillRect(s, &rect, SDL_MapRGBA(s->format, col, col, col, 0xff));

    rect.x++;
    rect.y++;
    rect.w -= 2;
    rect.h -= 2;
    col = 255;
    SDL_FillRect(s, &rect, SDL_MapRGBA(s->format, col, col, col, 0xff));

    rect.h = DIALOG_FOOTER;
    rect.y = DIALOG_H - 3 - rect.h;
    col = 240;
    SDL_FillRect(s, &rect, SDL_MapRGBA(s->format, col, col, col, 0xff));

    SDL_Surface* s2 = s;
    if (screen_ratio2 != screen_ratio1) {
        s2 = SDL_CreateRGBSurface(SDL_SWSURFACE, DIALOG_W * screen_ratio1 / screen_ratio2, DIALOG_H * screen_ratio1 / screen_ratio2,
                                  fmt->BitsPerPixel, fmt->Rmask, fmt->Gmask, fmt->Bmask, fmt->Amask);
        resizeSurface(s, s2);
        SDL_FreeSurface(s);
    }

    uchar3 col3 = {0, 0, 0};
    dialog_font.top_xy[0] = 7;
    dialog_font.top_xy[1] = DIALOG_HEADER + 5;
    dialog_font.num_xy[0] = (DIALOG_W - 7 * 2) / dialog_font.pitch_xy[0];
    dialog_font.num_xy[1] = 3;
    dialog_font.clear();
    drawString(mes1, col3, &dialog_font, false, s2, NULL, NULL);

    dialog_font.top_xy[0] = 5;
    dialog_font.top_xy[1] = (DIALOG_HEADER - dialog_font.font_size_xy[1]) / 2;
    openFont(&dialog_font);
    dialog_font.setLineArea(mes2);
    dialog_font.clear();
    drawString(mes2, col3, &dialog_font, false, s2, NULL, NULL);

    dialog_info.deleteSurface();
    dialog_info.num_of_cells = 1;
    dialog_info.setImage(s2, texture_format);

    dialog_info.pos.x = (screen_width - dialog_info.pos.w) / 2;
    dialog_info.pos.y = (screen_height - dialog_info.pos.h) / 2;

    // buttons
    char* mes[2] = {NULL};
    if (yesno_flag) {
        setStr(&mes[0], MESSAGE_YES, -1, true);
        setStr(&mes[1], MESSAGE_NO, -1, true);
    }
    else {
        setStr(&mes[0], MESSAGE_OK, -1, true);
        setStr(&mes[1], MESSAGE_CANCEL, -1, true);
    }

    for (int i = 0; i < 2; i++) {
        SDL_Surface* bs = SDL_CreateRGBSurface(SDL_SWSURFACE, DIALOG_BUTTON_W * 2, DIALOG_BUTTON_H,
                                               fmt->BitsPerPixel, fmt->Rmask, fmt->Gmask, fmt->Bmask, fmt->Amask);

        for (int j = 0; j < 2; j++) {
            rect.x = DIALOG_BUTTON_W * j;
            rect.y = 0;
            rect.w = DIALOG_BUTTON_W;
            rect.h = DIALOG_BUTTON_H;

            col = 105;
            SDL_FillRect(bs, &rect, SDL_MapRGBA(bs->format, col, col, col, 0xff));

            rect.w--;
            rect.h--;
            col = 255;
            SDL_FillRect(bs, &rect, SDL_MapRGBA(bs->format, col, col, col, 0xff));

            rect.x++;
            rect.y++;
            rect.w--;
            rect.h--;
            col = 227;
            SDL_FillRect(bs, &rect, SDL_MapRGBA(bs->format, col, col, col, 0xff));

            rect.x++;
            rect.y++;
            rect.w -= 2;
            rect.h -= 2;
            col = 240;
            if (j == 1) col = 214;
            SDL_FillRect(bs, &rect, SDL_MapRGBA(bs->format, col, col, col, 0xff));
        }

        SDL_Surface* bs2 = bs;
        if (screen_ratio2 != screen_ratio1) {
            bs2 = SDL_CreateRGBSurface(SDL_SWSURFACE, DIALOG_BUTTON_W * 2 * screen_ratio1 / screen_ratio2, DIALOG_BUTTON_H * screen_ratio1 / screen_ratio2,
                                       fmt->BitsPerPixel, fmt->Rmask, fmt->Gmask, fmt->Bmask, fmt->Amask);
            resizeSurface(bs, bs2);
            SDL_FreeSurface(bs);
        }

        for (int j = 0; j < 2; j++) {
            rect.x = DIALOG_BUTTON_W * j + 2;
            rect.y = 2;
            rect.w = DIALOG_BUTTON_W - 4;
            rect.h = DIALOG_BUTTON_H - 4;

            dialog_font.top_xy[0] = rect.x + (rect.w - dialog_font.pitch_xy[0] * strlen(mes[i]) / 2) / 2;
            dialog_font.top_xy[1] = rect.y + (rect.h - dialog_font.font_size_xy[1]) / 2;
            openFont(&dialog_font);
            dialog_font.setLineArea(mes[i]);
            dialog_font.clear();
            drawString(mes[i], col3, &dialog_font, false, bs2, NULL, NULL);
        }

        ButtonLink* btn = new ButtonLink();
        btn->no = i + 1;
        btn->button_type = ButtonLink::TMP_SPRITE_BUTTON;
        btn->anim[0] = new AnimationInfo();
        btn->anim[0]->num_of_cells = 2;
        btn->anim[0]->setImage(bs2, texture_format);
        btn->show_flag = 1;

        btn->anim[0]->pos.x = dialog_info.pos.x + (DIALOG_W - 3 - (DIALOG_BUTTON_W + 8) * (2 - i)) * screen_ratio1 / screen_ratio2;
        btn->anim[0]->pos.y = dialog_info.pos.y + (DIALOG_H - 3 - (DIALOG_FOOTER + DIALOG_BUTTON_H) / 2) * screen_ratio1 / screen_ratio2;

        btn->anim[0]->visible = true;
        btn->select_rect = btn->image_rect = btn->anim[0]->pos;

        root_button_link.insert(btn);
    }

    delete[] mes[0];
    delete[] mes[1];
}
