#pragma once

#include <stdarg.h>

#if defined(_WIN32) || defined(__CYGWIN__)
    #define WIN32_LEAN_AND_MEAN
    #define _WINSOCK_DEPRECATED_NO_WARNINGS
    #include <windows.h>
    #include <process.h>

    #define RV_PRI64 "I64"
    #define __attribute__()

    typedef CRITICAL_SECTION mutex_def;
    typedef HANDLE thread_def;
    typedef void *sharemem_def;   // HANDLE = void*
    typedef struct event_def {
        void *cond;             // HANDLE = void*
        bool state;
    } event_def;
#else
    # if defined(__WORDSIZE) && (__WORDSIZE == 64)
    #  define RV_PRI64 "l"
    # else
    #  define RV_PRI64 "ll"
    # endif

    typedef pthread_mutex_t mutex_def;
    typedef pthread_t thread_def;
    typedef struct event_def {
        pthread_mutex_t mut;
        pthread_cond_t cond;
        bool state;
    } event_def;
#endif

#ifdef __cplusplus
extern "C" {
#endif

int printf_log(int verbosity, const char *fmt, ...);
int sprintf_log(char *s, size_t len, const char *fmt, ...);

#define printf_debug(fmt, ...) \
    printf_log(4, fmt, __VA_ARGS__)

#define printf_info(fmt, ...) \
    printf_log(3, fmt, __VA_ARGS__)

#define printf_important(fmt, ...) \
    printf_log(2, fmt, __VA_ARGS__)

#define printf_error(fmt, ...) \
    printf_log(1, "%s:%d " fmt, __FILE__, __LINE__, __VA_ARGS__)

void utils_update();
void utils_sleep_ms(int ms);
sharemem_def utils_memshare_create(const char *name, int sz);
void* utils_memshare_map(sharemem_def h, int sz);
void utils_memshare_unmap(void *buf, int sz);
void utils_memshare_delete(sharemem_def h);

void utils_event_create(event_def *ev, const char *name);
void utils_event_close(event_def *ev);
void utils_event_set(event_def *ev);
int utils_event_is_set(event_def *ev);
void utils_event_clear(event_def *ev);
void utils_event_wait(event_def *ev);
int utils_event_wait_ms(event_def *ev, int ms);

int utils_mutex_init(mutex_def *mutex);
int utils_mutex_lock(mutex_def *mutex);
int utils_mutex_unlock(mutex_def *mutex);
int utils_mutex_destroy(mutex_def *mutex);

void utils_thread_join(thread_def th, int ms);

#ifdef __cplusplus
}
#endif
