/* Minimal JNI stub for host-side syntax checking of the nscr module
 * (no NDK required). Provides the identifiers ons_glue.cpp references. */
#ifndef __JNI_H_STUB__
#define __JNI_H_STUB__

#include <stdint.h>

#ifdef __cplusplus
#define JNIEXPORT extern "C"
#define JNICALL
#else
#define JNIEXPORT
#define JNICALL
#endif

typedef uint8_t  jboolean;
typedef int8_t   jbyte;
typedef uint16_t jchar;
typedef int16_t  jshort;
typedef int32_t  jint;
typedef int64_t  jlong;
typedef float    jfloat;
typedef double   jdouble;
typedef jint     jsize;

typedef void* jobject;
typedef jobject jclass;
typedef jobject jstring;
typedef jobject jarray;
typedef jobject jobjectArray;
typedef jarray  jbooleanArray;
typedef jarray  jbyteArray;
typedef jarray  jcharArray;
typedef jarray  jshortArray;
typedef jarray  jintArray;
typedef jarray  jlongArray;
typedef jarray  jfloatArray;
typedef jarray  jdoubleArray;
typedef jobject jthrowable;

#define JNI_FALSE 0
#define JNI_TRUE  1
#define JNI_OK    0
#define JNI_ERR   (-1)
#define JNI_EDETACHED (-2)
#define JNI_EVERSION  (-3)
#define JNI_COMMIT    1
#define JNI_ABORT     2

struct JNIEnv_;
typedef JNIEnv_ JNIEnv;
struct JavaVM_;
typedef JavaVM_ JavaVM;

struct JNIEnv_ {
    const void* functions;
};

struct JavaVM_ {
    const void* functions;
};

#endif /* __JNI_H_STUB__ */
