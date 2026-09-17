/*
    IkuraDroid launch glue (SDL 2.30.x), based on SDL_android_main.c,
    placed in the public domain by Sam Lantinga 3/13/14.

    The generic parts of the old glue moved into SDL itself: modern
    SDLActivity starts SDL_main() via nativeRunMain(), which dlopens
    the engine library (libikuradroid.so, see getMainSharedObject()),
    looks up SDL_main there and feeds it the arguments returned by
    getArguments(). JNI bindings (SDL_Android_Init / SDL_SetMainReady)
    are wired up internally by checkJNIReady() - nothing to do here.

    What is left for this file is the engine contract of the 2015
    port: the game folder becomes the process working directory, so
    the engine keeps opening its archives with plain relative paths.
    The savegame folder is passed through argv (--save) and needs no
    C-side storage anymore.
*/
#include <jni.h>
#include <unistd.h>     /* modern NDK: chdir() is no longer implicitly declared */

/* Called from SDLActivity.onCreate() on the UI thread, before the
 * SDL thread starts. */
void Java_org_libsdl_app_SDLActivity_nativeInit(
        JNIEnv* env, jclass cls,
        jstring currentDirectoryPath_j, jstring currentSavePath_j)
{
    if (currentDirectoryPath_j) {
        const char *currentDirectoryPath =
                (*env)->GetStringUTFChars(env, currentDirectoryPath_j, 0);
        if (currentDirectoryPath) {
            chdir(currentDirectoryPath);
            (*env)->ReleaseStringUTFChars(env, currentDirectoryPath_j,
                                          currentDirectoryPath);
        }
    }
    /* currentSavePath_j travels to SDL_main via getArguments(); the
     * parameter stays in the signature so the Java declaration keeps
     * matching the one of the 1.x releases. */
    (void)cls;
    (void)currentSavePath_j;
}

/* vi: set ts=4 sw=4 expandtab: */
