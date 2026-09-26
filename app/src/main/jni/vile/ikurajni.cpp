/*
 * IkuraDroid - JNI entry points for the Java save/load UI.
 *
 * SaveLoadDialog (su.viende.ikuradroid) parses the savegame files
 * directly from disk - no JNI in the read path. What it cannot do on
 * its own is ask the engine which filename prefix the current game's
 * saves use (NativeID, e.g. "Crescendo", "Critical"), and perform the
 * actual save/load: EventSave/EventLoad must run on the engine thread.
 *
 * Both needs are served here: the prefix is read from the parked
 * engine pointer (javabridge.h), and the save/load trigger is pushed
 * as an SDL_USEREVENT that the ViLE::RunEngine pump consumes on the
 * engine thread - the same handoff pattern the engine already uses
 * for SDL_QUIT (nativeSendQuit) and that emixer.cpp uses for the
 * mixer pause/resume natives.
 */

#include <jni.h>
#include <SDL.h>
#include <stdint.h>
#include "javabridge.h"

/*! \brief Filename prefix of the running engine's savegames
 *  \return NativeID of the loaded engine, or an empty string when no
 *          engine is running (the caller falls back to the old
 *          F5/F6 hotkey path)
 */
extern "C" JNIEXPORT jstring JNICALL
Java_org_libsdl_app_SDLActivity_nativeGetSavePrefix(JNIEnv *env, jclass)
{
        if (g_running_engine) {
                return env->NewStringUTF(g_running_engine->NativeID().c_str());
        }
        return env->NewStringUTF("");
}

/*! \brief Asks the engine to save or load a slot (thread-safe push)
 *  \param Mode VILE_JAVA_EVENT_LOAD or VILE_JAVA_EVENT_SAVE
 *  \param Slot Savegame index (0-39, mirroring the 5x8 native pages)
 *
 *  The event sits in the SDL queue until the engine pump drains it,
 *  so a tap arriving during a shutdown window is harmless: RunEngine
 *  flushes stale events before the next game starts.
 */
extern "C" JNIEXPORT void JNICALL
Java_org_libsdl_app_SDLActivity_nativeSendSaveLoadEvent(JNIEnv *, jclass,
                                                       jint Mode, jint Slot)
{
        SDL_Event event;
        SDL_zero(event);
        event.type = SDL_USEREVENT;
        event.user.code = (Mode == VILE_JAVA_EVENT_SAVE)
                                  ? VILE_JAVA_EVENT_SAVE
                                  : VILE_JAVA_EVENT_LOAD;
        event.user.data1 = (void *)(intptr_t)Slot;
        SDL_PushEvent(&event);
}
