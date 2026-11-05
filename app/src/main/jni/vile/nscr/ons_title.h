/* ONScripter-Droid library title probe.
 *
 * The Android game library (GameLibrary.java) shows every recognized
 * NScripter folder as a tile; the tile name doubles as the VNDB cover
 * search term. Ikura GDL titles carry their name in *.suf INI files,
 * NScripter scripts carry it inside the script itself - the "caption"
 * command of the define block, the same string the engine would put in
 * the window title. This module digs it out without booting the engine:
 * pick the script file the runtime would pick (0.txt first, then
 * 00.txt, nscr_sec.dat, nscript.dat - exactly ScriptHandler::readScript's
 * priority), decode it (plain text, the 5-byte rolling XOR of
 * nscr_sec.dat, or the XOR 0x84 of nscript.dat) and scan the define
 * block.
 *
 * Output is always UTF-8, ready for JNI NewStringUTF; the CP932 vs
 * CP1251 reading of the caption bytes is decided per string by a
 * character-quality score, so Japanese originals and Russian fan
 * translations both resolve correctly without touching the engine's
 * own encoding probe.
 */
#ifndef ONS_TITLE_H
#define ONS_TITLE_H

#include <stddef.h>

/* Extracts the game title of an NScripter game directory into out
 * (UTF-8, NUL-terminated; outlen includes the terminator). Returns
 * false when nothing title-like was found - the caller keeps the
 * plain folder name then. */
bool ONScripterProbeTitle(const char *dir, char *out, size_t outlen);

#endif
