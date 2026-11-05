/* ONScripter-Droid library title probe (see ons_title.h).
 *
 * Extracts the game title of an NScripter directory without running the
 * engine: the "caption" command of the define block is the string the
 * engine would show in the window title, so it doubles as the library
 * tile name and as the VNDB cover search term. "versionstr" is the
 * fallback ("Title ver1.0","Author" - the version tail is cut). The
 * script file to read is chosen in ScriptHandler::readScript priority
 * (0.txt, 00.txt, nscr_sec.dat, nscript.dat) and decrypted the same way
 * readScriptSub does: plain text, the rolling 5-byte XOR of
 * nscr_sec.dat, or the XOR 0x84 of nscript.dat. nscript.___ (mode 3)
 * needs the key table of the original EXE and cannot be probed.
 *
 * The CP932 vs CP1251 reading of the caption bytes is decided per
 * string by a character-quality score: Cyrillic in the CP1251 reading
 * beats the kanji/halfwidth-kana garbage the same bytes produce in
 * CP932, and vice versa for Japanese originals. Localization notes are
 * trimmed before the result leaves the module - a trailing
 * ", Russian Patch" or "(rus)" makes VNDB searches miss, while a real
 * subtitle after a comma ("Sharin no Kuni, Himawari no Shoujo") is
 * kept, because only marker words are cut.
 */
#include "ons_title.h"
#include "Encoding.h"

#include <stdio.h>
#include <string.h>
#include <vector>

#include <android/log.h>

extern unsigned short convSJIS2UTF16(unsigned short in);
extern void initSJIS2UTF16();

/* Script files in ScriptHandler::readScript priority; the mode is the
 * readScriptSub decryption: 0 plain, 1 XOR 0x84, 2 rolling 5-byte XOR. */
static const char *const SCRIPT_FILES[] = {
    "0.txt", "00.txt", "nscr_sec.dat", "nscript.dat"
};
static const int SCRIPT_MODES[] = { 0, 0, 2, 1 };

/* The define block head carries the caption in practice; half a
 * megabyte covers even the most talkative ones with a wide margin. */
#define ONS_TITLE_BUF_CAP (512 * 1024)
#define ONS_TITLE_MAX_UNITS 128

/* ------------------------------------------------------------------
 * Decoding: raw caption bytes -> UTF-16 -> UTF-8
 * ------------------------------------------------------------------ */

/* True for characters a real title is made of: ASCII, Latin, Cyrillic,
 * kana, kanji and their fullwidth forms. Everything else (typographic
 * punctuation, halfwidth kana in a wrong reading, replacement chars)
 * counts against a reading in the score. */
static bool ons_title_char_good(unsigned short u)
{
    if (u >= 0x20 && u <= 0x7e) return true;          /* ASCII */
    if (u >= 0x00a0 && u <= 0x017f) return true;      /* Latin-1/-ext */
    if (u >= 0x0400 && u <= 0x04ff) return true;      /* Cyrillic */
    if (u == 0x3000 || u == 0x3001 || u == 0x3002) return true;
    if (u >= 0x3040 && u <= 0x30ff) return true;      /* kana */
    if (u >= 0x4e00 && u <= 0x9fff) return true;      /* CJK */
    if (u >= 0xff01 && u <= 0xff5e) return true;      /* fullwidth ASCII */
    return false;
}

/* Decodes caption bytes into UTF-16 units with one encoding; returns
 * the number of undecodable positions. CP932 pairs are validated by
 * hand before the table lookup: the upstream table spans
 * 0x8140-0xfcfc, and garbage pairs (Russian bytes read as CP932) can
 * index outside it. Unassigned pairs read 0 and count as invalid. */
static int ons_decode_units(const unsigned char *raw, size_t len, int code,
                            std::vector<unsigned short> *units)
{
    static Encoding enc;
    units->clear();
    int invalid = 0;
    size_t i = 0;
    while (i < len) {
        if (code == Encoding::CODE_CP932) {
            unsigned char ch = raw[i];
            if ((ch & 0xe0) == 0xe0 || (ch & 0xe0) == 0x80) {
                if (i + 1 >= len) {
                    invalid++;
                    break;
                }
                unsigned char trail = raw[i + 1];
                unsigned short index =
                    (unsigned short)(((unsigned short)ch << 8) | trail);
                bool trail_ok = (trail >= 0x40 && trail <= 0x7e) ||
                                (trail >= 0x80 && trail <= 0xfc);
                unsigned short u = 0;
                if (index >= 0x8140 && index <= 0xfcfc && trail_ok)
                    u = convSJIS2UTF16(index);
                if (u == 0) {
                    invalid++;
                    units->push_back(0xfffd);
                }
                else {
                    units->push_back(u);
                }
                i += 2;
            }
            else if ((ch & 0xe0) == 0xa0 || (ch & 0xe0) == 0xc0) {
                /* halfwidth katakana, mapped like Encoding::getUTF16 */
                units->push_back((unsigned short)(ch - 0xa0 + 0xff60));
                i++;
            }
            else {
                units->push_back(ch);
                i++;
            }
        }
        else {
            /* CODE_CP1251: full single-byte table, never invalid */
            units->push_back(enc.getUTF16((const char *)&raw[i],
                                          Encoding::CODE_CP1251));
            i++;
        }
    }
    return invalid;
}

/* Quality of one reading: good title characters minus junk. */
static int ons_score_units(const std::vector<unsigned short> &units)
{
    int score = 0;
    for (size_t i = 0; i < units.size(); i++) {
        unsigned short u = units[i];
        if (u >= 0xd800 && u <= 0xdfff) continue;   /* lone surrogate */
        if (ons_title_char_good(u)) score++;
        else score--;
    }
    return score;
}

/* Serializes UTF-16 units as UTF-8; lone surrogates are dropped. */
static void ons_utf16_to_utf8(const std::vector<unsigned short> &units,
                              char *out, size_t outlen)
{
    size_t o = 0;
    for (size_t i = 0; i < units.size() && o + 4 < outlen; i++) {
        unsigned int u = units[i];
        if (u >= 0xd800 && u <= 0xdfff) continue;
        if (u < 0x80) {
            out[o++] = (char)u;
        }
        else if (u < 0x800) {
            out[o++] = (char)(0xc0 | (u >> 6));
            out[o++] = (char)(0x80 | (u & 0x3f));
        }
        else {
            out[o++] = (char)(0xe0 | (u >> 12));
            out[o++] = (char)(0x80 | ((u >> 6) & 0x3f));
            out[o++] = (char)(0x80 | (u & 0x3f));
        }
    }
    out[o] = 0;
}

/* ------------------------------------------------------------------
 * Suffix trimming (works on the UTF-8 result)
 * ------------------------------------------------------------------ */

/* Trims edge whitespace and stray CRs. */
static void ons_trim(char *s)
{
    size_t n = strlen(s);
    size_t a = 0;
    while (a < n && (s[a] == ' ' || s[a] == '\t' ||
                     s[a] == '\r' || s[a] == '\n')) a++;
    size_t b = n;
    while (b > a && (s[b - 1] == ' ' || s[b - 1] == '\t' ||
                     s[b - 1] == '\r' || s[b - 1] == '\n')) b--;
    if (a > 0) memmove(s, s + a, b - a);
    s[b - a] = 0;
}

/* Codepoint of a UTF-8 sequence; -1 on malformed input. */
static int ons_utf8_cp(const char *s, size_t len, size_t *advance)
{
    unsigned char c0 = (unsigned char)s[0];
    if (c0 < 0x80) {
        *advance = 1;
        return c0;
    }
    if ((c0 & 0xe0) == 0xc0 && len >= 2) {
        *advance = 2;
        return ((c0 & 0x1f) << 6) | (s[1] & 0x3f);
    }
    if ((c0 & 0xf0) == 0xe0 && len >= 3) {
        *advance = 3;
        return ((c0 & 0x0f) << 12) | ((s[1] & 0x3f) << 6) | (s[2] & 0x3f);
    }
    if ((c0 & 0xf8) == 0xf0 && len >= 4) {
        *advance = 4;
        return ((c0 & 0x07) << 18) | ((s[1] & 0x3f) << 6) |
               ((s[2] & 0x3f) << 6) | (s[3] & 0x3f);
    }
    *advance = 1;
    return -1;
}

/* ASCII+Cyrillic case fold: A-Z -> a-z, А-Я -> а-я, Ё -> ё. */
static int ons_cp_fold(int cp)
{
    if (cp >= 'A' && cp <= 'Z') return cp + 0x20;
    if (cp >= 0x0410 && cp <= 0x042f) return cp + 0x20;
    if (cp == 0x0401) return 0x0451;
    return cp;
}

/* True when the word (punctuation-stripped, case-folded) is a known
 * localization marker in English or Russian. */
static bool ons_word_is_marker(const char *w, size_t len)
{
    while (len > 0 && (w[len - 1] == '.' || w[len - 1] == ',' ||
                       w[len - 1] == ';' || w[len - 1] == ':' ||
                       w[len - 1] == '!' || w[len - 1] == '?' ||
                       w[len - 1] == ')' || w[len - 1] == '"')) len--;
    if (len == 0 || len > 24) return false;

    static const char *const ASCII_MARKERS[] = {
        "russian", "english", "rus", "eng", "patch", "ver", "version",
        "ru", "translation", "translated", NULL
    };
    /* Russian markers, case-folded on comparison */
    static const char *const RU_MARKERS[] = {
        "перевод", "версия", "рус", "русский", "русская", "русское",
        "пер", NULL
    };

    bool ascii_only = true;
    for (size_t i = 0; i < len; i++) {
        if ((unsigned char)w[i] >= 0x80) {
            ascii_only = false;
            break;
        }
    }
    if (ascii_only) {
        char buf[25];
        for (size_t i = 0; i < len; i++) {
            char c = w[i];
            if (c >= 'A' && c <= 'Z') c += 'a' - 'A';
            buf[i] = c;
        }
        buf[len] = 0;
        for (int i = 0; ASCII_MARKERS[i]; i++) {
            if (!strcmp(buf, ASCII_MARKERS[i])) return true;
        }
        return false;
    }

    /* Cyrillic path: fold the word into codepoints */
    std::vector<int> word;
    size_t i = 0;
    while (i < len) {
        size_t adv = 0;
        int cp = ons_utf8_cp(w + i, len - i, &adv);
        if (cp < 0) return false;
        word.push_back(ons_cp_fold(cp));
        i += adv;
    }
    for (int m = 0; RU_MARKERS[m]; m++) {
        std::vector<int> marker;
        const char *r = RU_MARKERS[m];
        while (*r) {
            size_t adv = 0;
            int cp = ons_utf8_cp(r, strlen(r), &adv);
            if (cp < 0) break;
            marker.push_back(ons_cp_fold(cp));
            r += adv;
        }
        if (marker.size() == word.size() && marker == word) return true;
    }
    return false;
}

/* True when every space-separated word of tail is a marker word. */
static bool ons_tail_is_markers(const char *tail)
{
    const char *p = tail;
    bool any = false;
    while (*p) {
        while (*p == ' ' || *p == '\t') p++;
        if (!*p) break;
        const char *w = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (!ons_word_is_marker(w, (size_t)(p - w))) return false;
        any = true;
    }
    return any;
}

/* Drops trailing localization notes: "..., Russian Patch" after the
 * last comma (only when all its words are markers, so real subtitles
 * survive) and a "(rus)"-style trailing parenthesis. */
static void ons_cut_patch_suffixes(char *s)
{
    bool cut = true;
    while (cut && s[0]) {
        cut = false;
        char *last_comma = strrchr(s, ',');
        if (last_comma) {
            if (ons_tail_is_markers(last_comma + 1)) {
                *last_comma = 0;
                ons_trim(s);
                cut = true;
            }
            continue;
        }
        size_t n = strlen(s);
        if (n > 0 && s[n - 1] == ')') {
            char *lp = strrchr(s, '(');
            if (lp && (size_t)(lp - s) < n - 1 &&
                ons_tail_is_markers(lp + 1)) {
                *lp = 0;
                ons_trim(s);
                cut = true;
            }
        }
    }
}

/* Cuts a " ver1.004"-style tail off a versionstr value: a space, then
 * ver/Ver/VER directly followed by a digit and nothing but digits and
 * dots until the end. */
static void ons_cut_version_tail(char *s)
{
    size_t n = strlen(s);
    for (size_t i = 4; i < n; i++) {
        if (s[i - 4] != ' ' && s[i - 4] != '\t') continue;
        char v = s[i - 3] | 0x20;
        char e = s[i - 2] | 0x20;
        char r = s[i - 1] | 0x20;
        if (v != 'v' || e != 'e' || r != 'r') continue;
        if (s[i] < '0' || s[i] > '9') continue;
        size_t j = i;
        while (j < n && ((s[j] >= '0' && s[j] <= '9') || s[j] == '.')) j++;
        if (j == n) {
            s[i - 4] = 0;
            ons_trim(s);
        }
        break;
    }
}

/* ------------------------------------------------------------------
 * Caption scan
 * ------------------------------------------------------------------ */

/* Value of the first quoted string of a command line; NULL when the
 * line does not carry a non-empty one. */
static const unsigned char *ons_quoted_value(const unsigned char *line,
                                             size_t len, size_t *vlen)
{
    size_t i = 0;
    while (i < len && line[i] != '"') i++;
    if (i >= len) return NULL;
    i++;
    size_t start = i;
    while (i < len && line[i] != '"') i++;
    if (i >= len || i == start) return NULL;
    *vlen = i - start;
    return line + start;
}

/* CP932/CP1251 reading of one caption value; false when neither
 * reading looks like real text. */
static bool ons_decode_caption(const unsigned char *raw, size_t len,
                               char *out, size_t outlen)
{
    static bool table_ready = false;
    if (!table_ready) {
        initSJIS2UTF16();
        table_ready = true;
    }
    if (len == 0 || len > ONS_TITLE_MAX_UNITS * 3) return false;

    std::vector<unsigned short> jp;
    std::vector<unsigned short> ru;
    int jp_invalid = ons_decode_units(raw, len, Encoding::CODE_CP932, &jp);
    ons_decode_units(raw, len, Encoding::CODE_CP1251, &ru);
    int jp_score = ons_score_units(jp) - 2 * jp_invalid;
    int ru_score = ons_score_units(ru);
    const std::vector<unsigned short> &best =
        jp_score > ru_score ? jp : ru;
    if (best.size() < 2 || best.size() > ONS_TITLE_MAX_UNITS) return false;

    int good = 0;
    for (size_t i = 0; i < best.size(); i++) {
        if (ons_title_char_good(best[i])) good++;
    }
    if (good < 2) return false;

    ons_utf16_to_utf8(best, out, outlen);
    return true;
}

bool ONScripterProbeTitle(const char *dir, char *out, size_t outlen)
{
    out[0] = 0;
    if (!dir || !out || outlen < 32) return false;

    for (unsigned fi = 0;
         fi < sizeof(SCRIPT_FILES) / sizeof(SCRIPT_FILES[0]); fi++) {
        char path[1024];
        if (dir[0])
            snprintf(path, sizeof(path), "%s/%s", dir, SCRIPT_FILES[fi]);
        else
            snprintf(path, sizeof(path), "%s", SCRIPT_FILES[fi]);

        FILE *fp = fopen(path, "rb");
        if (!fp) continue;
        std::vector<unsigned char> buf(ONS_TITLE_BUF_CAP);
        size_t len = fread(buf.data(), 1, buf.size(), fp);
        fclose(fp);
        if (len < 8) continue;

        int mode = SCRIPT_MODES[fi];
        if (mode == 1) {
            for (size_t i = 0; i < len; i++) buf[i] ^= 0x84;
        }
        else if (mode == 2) {
            static const unsigned char magic[5] =
                {0x79, 0x57, 0x0d, 0x80, 0x04};
            for (size_t i = 0; i < len; i++) buf[i] ^= magic[i % 5];
        }

        bool define_ended = false;
        bool have_define_caption = false;
        bool have_any_caption = false;
        bool have_versionstr = false;
        char define_caption[256];
        char any_caption[256];
        char versionstr[256];

        const unsigned char *line = buf.data();
        size_t remaining = len;
        while (remaining > 0) {
            const unsigned char *nl =
                (const unsigned char *)memchr(line, '\n', remaining);
            size_t llen = nl ? (size_t)(nl - line) : remaining;
            const unsigned char *l = line;
            while (llen > 0 && (l[0] == ' ' || l[0] == '\t')) {
                l++;
                llen--;
            }
            if (llen > 0 && l[llen - 1] == '\r') llen--;

            if (llen == 4 && !memcmp(l, "game", 4)) {
                define_ended = true;
                if (have_define_caption) break;
            }
            else if (llen > 7 && !memcmp(l, "caption", 7) &&
                     (l[7] == ' ' || l[7] == '\t')) {
                size_t vlen;
                const unsigned char *v =
                    ons_quoted_value(l + 7, llen - 7, &vlen);
                if (v) {
                    if (!have_any_caption &&
                        ons_decode_caption(v, vlen, any_caption,
                                           sizeof(any_caption))) {
                        have_any_caption = true;
                    }
                    if (!define_ended && !have_define_caption &&
                        ons_decode_caption(v, vlen, define_caption,
                                           sizeof(define_caption))) {
                        have_define_caption = true;
                    }
                    if (have_define_caption || define_ended) {
                        if (have_define_caption || have_any_caption) break;
                    }
                }
            }
            else if (!have_versionstr && llen > 10 &&
                     !memcmp(l, "versionstr", 10) &&
                     (l[10] == ' ' || l[10] == '\t')) {
                size_t vlen;
                const unsigned char *v =
                    ons_quoted_value(l + 10, llen - 10, &vlen);
                if (v && vlen > 0 && vlen < sizeof(versionstr)) {
                    memcpy(versionstr, v, vlen);
                    versionstr[vlen] = 0;
                    have_versionstr = true;
                }
            }

            if (!nl) break;
            remaining -= llen + (size_t)(l - line) + 1;
            line = nl + 1;
        }

        const char *via = NULL;
        if (have_define_caption) {
            snprintf(out, outlen, "%s", define_caption);
            via = "define caption";
        }
        else if (have_any_caption) {
            snprintf(out, outlen, "%s", any_caption);
            via = "caption";
        }
        else if (have_versionstr) {
            snprintf(out, outlen, "%s", versionstr);
            ons_cut_version_tail(out);
            via = "versionstr";
        }
        if (via) {
            ons_cut_patch_suffixes(out);
            ons_trim(out);
            if (strlen(out) >= 2 && strlen(out) <= outlen - 1) {
                __android_log_print(ANDROID_LOG_INFO, "ONS",
                                    "title probe (%s): \"%s\"", via, out);
                return true;
            }
            out[0] = 0;
        }
    }
    return false;
}

/* ------------------------------------------------------------------
 * JNI bridge for GameLibrary.java
 * ------------------------------------------------------------------ */

#ifdef __ANDROID__
#include <jni.h>

extern "C" JNIEXPORT jstring JNICALL
Java_su_viende_ikuradroid_GameLibrary_probeNscrTitleNative(JNIEnv *env,
                                                           jclass,
                                                           jstring jdir)
{
    char out[256];
    out[0] = 0;
    if (!jdir) return env->NewStringUTF("");

    const char *dir = env->GetStringUTFChars(jdir, NULL);
    if (!dir) return env->NewStringUTF("");

    ONScripterProbeTitle(dir, out, sizeof(out));
    env->ReleaseStringUTFChars(jdir, dir);
    return env->NewStringUTF(out);
}
#endif
