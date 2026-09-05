/*
    SDL_android_main.c, placed in the public domain by Sam Lantinga  3/13/14
*/
#include "../sdl/src/SDL_internal.h"

#ifdef __ANDROID__

/* Include the SDL main definition header */
#include "SDL_main.h"

/*******************************************************************************
                 Functions called by JNI
*******************************************************************************/
#include <jni.h>
#include <unistd.h>     /* modern NDK: chdir() is no longer implicitly declared */

/* Called before SDL_main() to initialize JNI bindings in SDL library */
extern void SDL_Android_Init(JNIEnv* env, jclass cls);

/* Start up the SDL app */
void Java_org_libsdl_app_SDLActivity_nativeInit(JNIEnv* env, jclass cls, jstring currentDirectoryPath_j,jstring currentSavePath_j)
{
    /* This interface could expand with ABI negotiation, calbacks, etc. */
    SDL_Android_Init(env, cls);

    SDL_SetMainReady();

    /* Run the application code! */
    int status;
    char *argv[2] =  { SDL_strdup("SDL_app"), NULL, NULL};
    argv[1] = "--save";

        const char *currentDirectoryPath = (*env)->GetStringUTFChars(env, currentDirectoryPath_j, 0);
        chdir(currentDirectoryPath);

        (*env)->ReleaseStringUTFChars(env, currentDirectoryPath_j, currentDirectoryPath);

        const char *currentSavePath = (*env)->GetStringUTFChars(env, currentSavePath_j, 0);
        char *cSavePath = malloc(strlen(currentSavePath) + 1);
        strcpy( cSavePath,currentSavePath);
        argv[2] = cSavePath;
        (*env)->ReleaseStringUTFChars(env, currentSavePath_j, currentSavePath);

    status = SDL_main(3, argv);

    /* Do not issue an exit or the whole application will terminate instead of just the SDL thread */
    /* exit(status); */
}

#endif /* __ANDROID__ */

/* vi: set ts=4 sw=4 expandtab: */
