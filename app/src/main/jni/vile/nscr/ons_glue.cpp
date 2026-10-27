/* ONScripter-Droid dispatch glue: ViLE entry point -> ONScripter runtime.
 *
 * Replaces onscripter_main.cpp's SDL_main (which cannot be linked into
 * libikuradroid.so next to ViLE's own SDL_main) with a plain function
 * interface. The startup sequence mirrors the ANDROID branch of
 * SDL_main: enableButtonShortCut -> openScript -> init -> executeLabel.
 */
#include "ons_glue.h"
#include "ONScripter.h"
#include "Encoding.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <android/log.h>
#include <jni.h>

/* Java save/load bridge: the M3 menu needs to know that the ONScripter
 * runtime owns the screen. ViLE engines report through the parked engine
 * pointer (javabridge.h), but ONScripter never creates one, so the glue
 * keeps its own flag around the executeLabel() run. */
bool g_ons_runtime_active = false;

static bool ons_readable(const char *dir, const char *name)
{
    char path[1024];
    if (dir && dir[0])
        snprintf(path, sizeof(path), "%s/%s", dir, name);
    else
        snprintf(path, sizeof(path), "%s", name);
    FILE *fp = fopen(path, "rb");
    if (fp) {
        fclose(fp);
        return true;
    }
    return false;
}

bool ONScripterProbe(const char *dir)
{
    return ons_readable(dir, "nscript.dat") ||
           ons_readable(dir, "nscr_sec.dat") ||
           ons_readable(dir, "nscript.___") ||
           ons_readable(dir, "0.txt") ||
           ons_readable(dir, "00.txt");
}

static bool ons_contains(const unsigned char *hay, size_t len, const char *needle)
{
    size_t m = strlen(needle);
    if (m == 0 || len < m) return false;
    for (size_t i = 0; i + m <= len; i++)
        if (!memcmp(hay + i, needle, m)) return true;
    return false;
}

/* Sniff the script encoding: Russian fan translations mark the define
 * block with "language english" and carry CP1251 text; vanilla Japanese
 * scripts stay CP932. nscript.dat/pscript.dat are stored XOR 0x84
 * (readScript encrypt_mode 1), plain-text scripts are 0.txt/00.txt.
 * The marker normally sits near the head of the define block, but
 * translations vary, so scan a generous window. When no marker turns
 * up, fall back to the heuristic inherited from the ONScripterRU lineage:
 * Russian translations ship a default.ttf next to the script, vanilla Japanese
 * games do not. Every decision is logged - the logcat line "encoding
 * probe" is the first thing to check when text renders as mojibake. */
static int ons_detect_encoding(const char *dir)
{
    static const char *const names[] = {
        "nscript.dat", "pscript.dat", "0.txt", "00.txt"
    };
    static const int name_mode[] = { 1, 1, 0, 0 };  /* XOR 0x84? */
    static unsigned char buf[262144];
    for (unsigned i = 0; i < sizeof(names)/sizeof(names[0]); i++) {
        char path[1024];
        if (dir && dir[0])
            snprintf(path, sizeof(path), "%s/%s", dir, names[i]);
        else
            snprintf(path, sizeof(path), "%s", names[i]);
        FILE *fp = fopen(path, "rb");
        if (!fp) continue;
        size_t len = fread(buf, 1, sizeof(buf), fp);
        fclose(fp);
        if (name_mode[i]) {
            for (size_t j = 0; j < len; j++) buf[j] ^= 0x84;
        }
        int code;
        const char *how;
        if (ons_contains(buf, len, "language english")) {
            code = Encoding::CODE_CP1251;
            how = "marker 'language english'";
        }
        else if (ons_readable(dir, "default.ttf")) {
            code = Encoding::CODE_CP1251;
            how = "default.ttf heuristic";
        }
        else {
            code = Encoding::CODE_CP932;
            how = "fallback";
        }
        __android_log_print(ANDROID_LOG_INFO, "ONS",
            "encoding probe via %s (%zu bytes): %s <- %s", names[i], len,
            code == Encoding::CODE_CP1251 ? "cp1251" : "cp932", how);
        return code;
    }
    __android_log_print(ANDROID_LOG_WARN, "ONS",
        "encoding probe: no script file found, defaulting to cp932");
    return Encoding::CODE_CP932;
}

int ONScripterRun(const char *dir)
{
    /* ViLE turns SDL2's touch-to-mouse synthesis off process-wide
     * (SDL_HINT_TOUCH_MOUSE_EVENTS "0" in vile.cpp): ViLE feeds its own
     * widgets from FINGER events directly and the extra mouse stream
     * double-processed its input. ONScripter only reads mouse events on
     * Android - its SDL_FINGER* branch is iOS-only - so with the
     * synthesis disabled no tap ever reaches mousePressEvent and the
     * game is unclickable. Turn the synthesis back on while this
     * runtime owns the event loop; mousePressEvent rescales the
     * synthesized window coordinates into game space via
     * deviceToGameX/Y. Restored on the way out so a follow-up ViLE run
     * in the same process keeps its assumption. */
    SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS, "1");

    /* Fresh instance per run: main() can be re-entered in the same
     * process after a game quits, and ONScripter keeps engine-wide
     * state in its members. */
    ONScripter *ons = new ONScripter();

    ons->enableButtonShortCut();
    if (dir && dir[0]) {
        ons->setArchivePath(dir);
        ons->setSaveDir(dir);
    }
    int enc_code = ons_detect_encoding(dir);
    ons->setEncoding(enc_code);
    __android_log_print(ANDROID_LOG_INFO, "ONS",
        "setEncoding -> %d (%s)", enc_code,
        enc_code == Encoding::CODE_CP1251 ? "cp1251" :
        enc_code == Encoding::CODE_UTF8 ? "utf-8" : "cp932");

    if (ons->openScript()) {
        __android_log_print(ANDROID_LOG_ERROR, "ONS", "openScript failed");
        return -1;
    }
    if (ons->init()) {
        __android_log_print(ANDROID_LOG_ERROR, "ONS", "init failed");
        return -2;
    }
    g_ons_runtime_active = true;
    ons->executeLabel();
    g_ons_runtime_active = false;
    SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS, "0");
    __android_log_print(ANDROID_LOG_INFO, "ONS", "executeLabel returned");
    return 0;
}

/* Android video hook: ONScripter_sound.cpp's playAVI() calls this under
 * ANDROID. The original definition sits in onscripter_main.cpp, which is
 * not linked into libikuradroid.so (SDL_main clash), so the stub from the
 * ONScripterRU Android port is replicated here: log and return. playAVI() returns
 * immediately afterwards, hence the script skips the movie without
 * waiting; wiring the real pipeline (pl_mpeg / EDLTexture) is a task for
 * the video phase. */
extern "C" void playVideoAndroid(const char* filename)
{
    __android_log_print(ANDROID_LOG_WARN, "ONS",
        "playVideoAndroid: video playback not supported yet (filename='%s')",
        filename ? filename : "(null)");
}

/* True while an ONScripter game is on screen. SDLActivity's M3 menu
 * consults it before the ViLE prefix path: for these titles Save/Load
 * cannot open the native slot dialog, because ONScripter owns its own
 * save/load screens and its state machine must drive them. */
extern "C" JNIEXPORT jboolean JNICALL
Java_org_libsdl_app_SDLActivity_nativeIsOnscripterActive(JNIEnv *, jclass)
{
    return g_ons_runtime_active ? JNI_TRUE : JNI_FALSE;
}
