#include <iostream>
#include <string.h>
#include <stdlib.h>
#include "simutils.h"

static const int OUT_BUFFER_SIZE = 1024*1024;
static char outbuffer_[2*OUT_BUFFER_SIZE] = {0};
static int cnt_wr_ = 0;
static int cnt_rd_ = 0;
FILE *logfile_ = 0;

int printf_log(int verbosity, const char *fmt, ...) {
    int ret = 0;
    char tstr[1<<14];

    va_list arg;
    va_start(arg, fmt);

#if defined(_WIN32) || defined(__CYGWIN__)
    ret += vsprintf_s(&tstr[ret], sizeof(tstr), fmt, arg);
#else
    ret += vsprintf(&buf[ret], fmt, arg);
#endif
    va_end(arg);

    for (int i = 0; i < ret; i++) {
        outbuffer_[cnt_wr_] =
        outbuffer_[cnt_wr_ + OUT_BUFFER_SIZE] = tstr[i];
        if (++cnt_wr_ >= OUT_BUFFER_SIZE) {
            cnt_wr_ = 0;
        }
        outbuffer_[cnt_wr_] = 
        outbuffer_[cnt_wr_ + OUT_BUFFER_SIZE] = 0;
    }
    return ret;
}

int sprintf_log(char *s, size_t len, const char *fmt, ...) {
    int ret;
    va_list arg;
    va_start(arg, fmt);
#if defined(_WIN32) || defined(__CYGWIN__)
    ret = vsprintf_s(s, len, fmt, arg);
#else
    ret = vsprintf(s, fmt, arg);
#endif
    va_end(arg);
    return ret;
}


/**
    printf(), write() etc. are the thread safe functions and the hang-on if function
    SuspendThread was called during their execution that leads to system lock.
    So make output in a separate method.
 */
void utils_update() {
    int total = cnt_wr_ - cnt_rd_;
    if (total < 0) {
        total += OUT_BUFFER_SIZE;
    }
    if (logfile_ == 0) {
        fopen_s(&logfile_, "_sim.log", "wb");
    }
    fwrite(&outbuffer_[cnt_rd_], 1, total, logfile_);
    fflush(logfile_);
    
    //printf("%s", &outbuffer_[cnt_rd_]);
    fwrite(&outbuffer_[cnt_rd_], 1, total, stdout);
    cnt_rd_ += total;
    if (cnt_rd_ >= OUT_BUFFER_SIZE) {
        cnt_rd_ -= OUT_BUFFER_SIZE;
    }
}

void utils_sleep_ms(int ms) {
#if defined(_WIN32) || defined(__CYGWIN__)
    Sleep(ms);
#else
    usleep(1000*ms);
#endif
}

sharemem_def utils_memshare_create(const char *name, int sz) {
    sharemem_def ret = 0;
#if defined(_WIN32) || defined(__CYGWIN__)
    wchar_t w_name[1024];
    size_t bytes_converted;
    mbstowcs_s(&bytes_converted, w_name, name, 256);

    ret = CreateFileMappingW(
                 INVALID_HANDLE_VALUE,  // use paging file
                 NULL,                  // default security
                 PAGE_READWRITE,      // read/write access
                 0,                   // maximum object size (high-order DWORD)
                 sz,                  // maximum object size (low-order DWORD)
                 w_name);               // name of mapping object
#else
    ret = shm_open(name, O_CREAT | O_RDWR, 0777);
    if (ret > 0) {
        if ( ftruncate(ret, sz + 1) == -1 ) {
            printf_error("Couldn't set mem size %s", name);
            ret = 0;
        }
    } else {
        ret = 0;
    }
#endif
    if (ret == 0) {
        printf_error("Couldn't create map object %s", name);
    }
    return ret;
}

void* utils_memshare_map(sharemem_def h, int sz) {
    void *ret = 0;
#if defined(_WIN32) || defined(__CYGWIN__)
    ret = MapViewOfFile(h,   // handle to map object
                        FILE_MAP_ALL_ACCESS, // read/write permission
                        0,
                        0,
                        sz);
#else
    ret = mmap(NULL, sz + 1, PROT_READ|PROT_WRITE, MAP_SHARED, h, 0);
    if (ret < 0) {
        ret = 0;
    }
#endif
    if (ret == 0) {
        printf_error("Couldn't map view of file with size %d", sz);
    }
    return ret;
}

void utils_memshare_unmap(void *buf, int sz) {
#if defined(_WIN32) || defined(__CYGWIN__)
    UnmapViewOfFile(buf);
#else
    munmap(buf, sz + 1);
#endif
}

void utils_memshare_delete(sharemem_def h) {
#if defined(_WIN32) || defined(__CYGWIN__)
    CloseHandle(h);
#else
    close(h);
#endif
}

void utils_event_create(event_def *ev, const char *name) {
#if defined(_WIN32) || defined(__CYGWIN__)
    wchar_t wevent_name[256];
    size_t converted;
    mbstowcs_s(&converted, wevent_name, name, 256);
    ev->state = false;
    ev->cond = CreateEventW(
        NULL,               // default security attributes
        TRUE,               // manual-reset event
        FALSE,              // initial state is nonsignaled
        wevent_name         // object name
        );
#else
    pthread_mutex_init(&ev->mut, NULL);
    pthread_cond_init(&ev->cond, NULL);
    ev->state = false;
#endif
}

void utils_event_close(event_def *ev) {
#if defined(_WIN32) || defined(__CYGWIN__)
    CloseHandle(ev->cond);
#else
    pthread_mutex_destroy(&ev->mut);
    pthread_cond_destroy(&ev->cond);
#endif
}

void utils_event_set(event_def *ev) {
#if defined(_WIN32) || defined(__CYGWIN__)
    ev->state = true;
    SetEvent(ev->cond);
#else
    pthread_mutex_lock(&ev->mut);
    ev->state = true;
    pthread_mutex_unlock(&ev->mut);
    pthread_cond_signal(&ev->cond);
#endif
}

int utils_event_is_set(event_def *ev) {
    return ev->state ? 1: 0;
}

void utils_event_clear(event_def *ev) {
#if defined(_WIN32) || defined(__CYGWIN__)
    ev->state = false;
    ResetEvent(ev->cond);
#else
    ev->state = false;
#endif
}

void utils_event_wait(event_def *ev) {
#if defined(_WIN32) || defined(__CYGWIN__)
    WaitForSingleObject(ev->cond, INFINITE);
#else
    int result = 0;
    pthread_mutex_lock(&ev->mut);
    while (result == 0 && !ev->state) {
        result = pthread_cond_wait(&ev->cond, &ev->mut);
    } 
    pthread_mutex_unlock(&ev->mut);
#endif
}

int utils_event_wait_ms(event_def *ev, int ms) {
#if defined(_WIN32) || defined(__CYGWIN__)
    DWORD wait_ms = ms;
    if (ms == 0) {
        wait_ms = INFINITE;
    }
    if (WAIT_TIMEOUT == WaitForSingleObject(ev->cond, wait_ms)) {
        return 1;
    }
    return 0;
#else
    struct timeval tc;
    struct timespec ts;
    int next_us;
    int result = 0;
    gettimeofday(&tc, NULL);
    next_us = tc.tv_usec + 1000 * ms;
    ts.tv_sec = tc.tv_sec + (next_us / 1000000);
    next_us -= 1000000 * (next_us / 1000000);
    ts.tv_nsec = 1000 * next_us;

    pthread_mutex_lock(&ev->mut);
    while (result == 0 && !ev->state) {
        result = pthread_cond_timedwait(&ev->cond, &ev->mut, &ts);
    }
    pthread_mutex_unlock(&ev->mut);
    if (ETIMEDOUT == result) {
        return 1;
    }
    return 0;
#endif
}

int utils_mutex_init(mutex_def *mutex) {
#if defined(_WIN32) || defined(__CYGWIN__)
    InitializeCriticalSection(mutex);
#else
    pthread_mutexattr_t attr;

    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(mutex, &attr);
#endif
    return 0;
}

int utils_mutex_lock(mutex_def *mutex) {
#if defined(_WIN32) || defined(__CYGWIN__)
    EnterCriticalSection(mutex);
#else
    pthread_mutex_lock(mutex) ;
#endif
    return 0;
}

int utils_mutex_unlock(mutex_def * mutex) {
#if defined(_WIN32) || defined(__CYGWIN__)
    LeaveCriticalSection(mutex);
#else
    pthread_mutex_unlock(mutex);
#endif
    return 0;
}

int utils_mutex_destroy(mutex_def *mutex) {
#if defined(_WIN32) || defined(__CYGWIN__)
    DeleteCriticalSection(mutex);
#else
    pthread_mutex_destroy(mutex);
#endif
    return 0;
}


void utils_thread_join(thread_def th, int ms) {
#if defined(_WIN32) || defined(__CYGWIN__)
    WaitForSingleObject(th, ms);
#else
    pthread_join(th, 0);
#endif
}
