/*
 * IkuraDroid - engine -> Java UI bridge.
 *
 * Java already drives the engine through SDL_USEREVENTs (ikurajni.cpp);
 * this is the return path: the engine asks the Java UI to open the
 * slot dialog. EngineVN::EventGameDialog routes every in-engine
 * save/load request here - the title screen Load/Continue buttons,
 * the popup menu entries, the F5/F6 hotkeys, the Ikura iop_opsl and
 * Will OP83 opcodes - so those surface the Java SaveLoadDialog
 * instead of the native StdSave/StdLoad widgets.
 *
 * The engine thread runs the SDL main loop on a Java thread, so its
 * JNIEnv is either already attached or attaches on demand through
 * SDL_AndroidGetJNIEnv(). The Java method itself hops to the UI
 * thread before touching any views, so nothing here blocks.
 */

#include "javabridge.h"

#ifdef __ANDROID__
#include <jni.h>
#include <SDL_system.h>
#endif

bool BridgeRequestSaveLoadDialog(bool Save){
#ifdef __ANDROID__
	bool retval=false;
	JNIEnv *env=(JNIEnv*)SDL_AndroidGetJNIEnv();
	if(env){
		jclass cls=env->FindClass("org/libsdl/app/SDLActivity");
		if(cls){
			jmethodID mid=env->GetStaticMethodID(cls,
							"openSaveLoadFromEngine","(Z)V");
			if(mid){
				env->CallStaticVoidMethod(cls,mid,
								Save?JNI_TRUE:JNI_FALSE);
				retval=true;
			}
			env->DeleteLocalRef(cls);
		}
		if(env->ExceptionCheck()){
			// A broken bridge must never wedge the engine:
			// log the pending exception and drop it
			env->ExceptionDescribe();
			env->ExceptionClear();
			retval=false;
		}
	}
	return retval;
#else
	// Host builds have no Java UI - the caller keeps its native dialog
	return false;
#endif
}
