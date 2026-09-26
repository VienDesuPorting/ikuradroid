/*
 * IkuraDroid - Java UI <-> engine bridge.
 *
 * The Java-side save/load menu (SaveLoadDialog) reads savegame files
 * directly from disk, so the only things it ever asks the engine for
 * are: the running engine's savegame filename prefix (NativeID) and
 * one-shot "perform a save/load of slot N" triggers. Both live here.
 *
 * The trigger is an SDL_USEREVENT pushed from the JNI function in
 * ikurajni.cpp and consumed by the ViLE::RunEngine event pump, because
 * EventSave/EventLoad must run on the engine thread - the same single
 * thread that owns every widget and parser state.
 */
#ifndef _JAVABRIDGE_H_
#define _JAVABRIDGE_H_

#include "engine/evn.h"

// event.user.code values pushed by nativeSendSaveLoadEvent().
// Mirrored as constants in SaveLoadDialog.java - keep in sync.
#define VILE_JAVA_EVENT_LOAD            1
#define VILE_JAVA_EVENT_SAVE            2

// The engine instance currently driven by ViLE::RunEngine(), parked in
// a global so the JNI bridge can reach it from the Java UI thread.
// Written only on the engine thread (set at RunEngine entry, cleared
// at exit) and read from JNI after a null check: the pointer swap is
// atomic on every supported ABI and NativeID() itself is stateless.
extern EngineVN *g_running_engine;

#endif
