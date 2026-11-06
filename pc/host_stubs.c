/* pc/host_stubs.c — desktop-only replacements for Android platform
 * symbols the ONScripter-Droid module links against. On Android these
 * come from liblog; here they just go to stderr so the console build
 * keeps the same diagnostics.
 *
 * This file is part of IkuraDroid.
 * Copyright (C) 2026 VienDesu! Porting Team
 * Licensed under the GNU GPL v3 or later; see the repository LICENSE file.
 */
#include <stdarg.h>
#include <stdio.h>

int __android_log_print(int prio, const char *tag, const char *fmt, ...)
{
    static const char *levels[] = { "?", "?", "V", "D", "I", "W", "E" };
    const char *lvl = (prio >= 2 && prio <= 6) ? levels[prio] : "?";
    va_list ap;
    fprintf(stderr, "%s/%s: ", lvl, tag ? tag : "nscr");
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fputc('\n', stderr);
    return 0;
}
