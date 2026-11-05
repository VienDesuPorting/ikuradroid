/* Minimal android/log.h stub for host-side syntax checking. */
#ifndef __ANDROID_LOG_H_STUB__
#define __ANDROID_LOG_H_STUB__

enum { ANDROID_LOG_VERBOSE = 2, ANDROID_LOG_DEBUG = 3,
       ANDROID_LOG_INFO = 4, ANDROID_LOG_WARN = 5, ANDROID_LOG_ERROR = 6 };

/* extern "C" like the real NDK header, so host-built test binaries
 * link against C-definitions of the stub. */
#ifdef __cplusplus
extern "C" {
#endif

int __android_log_print(int prio, const char* tag, const char* fmt, ...);

#ifdef __cplusplus
}
#endif

#endif
