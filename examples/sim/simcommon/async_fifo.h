#pragma once

#include <inttypes.h>
#include "simutils.h"

template <class T> class TAsyncFifo {
 public:
    TAsyncFifo(const char *name, int size, bool blocking=true) {
        sprintf_log(name_, sizeof(name_), "%s", name);
        blocking_ = blocking;
        size_ = size;
        wr_cnt_ = 1;
        rd_cnt_ = 0;
        buf_ = new T[size];
        utils_mutex_init(&rxmutex_);
        char tstr[256];
        sprintf_log(tstr, sizeof(tstr), "%s_rxevent", name);
        utils_event_create(&rxevent_, tstr);
        utils_event_clear(&rxevent_);
    }
    virtual ~TAsyncFifo() {
        utils_mutex_destroy(&rxmutex_);
        utils_event_close(&rxevent_);
        delete [] buf_;
    }

    bool is_empty();
    bool is_full();
    void put(T *msg);
    bool get(T *msg);
    bool get(T *msg, int timeout_ms);
    void set_blocking(bool v) { blocking_ = v; }

 protected:
    char name_[256];
    int size_;
    int wr_cnt_;
    int rd_cnt_;
    bool blocking_;
    T *buf_;
    mutex_def rxmutex_;
    event_def rxevent_;
};


template <class T>
bool TAsyncFifo<T>::is_empty() {
    int d = wr_cnt_ - rd_cnt_;
    if (d < 0) {
        d += size_;
    }
    return d == 1;
}

template <class T>
bool TAsyncFifo<T>::is_full() {
    return wr_cnt_ == rd_cnt_;
}

template <class T>
void TAsyncFifo<T>::put(T *msg) {
    utils_mutex_lock(&rxmutex_);
    while (blocking_ && is_full()) {
        // to avoid data loss
        Sleep(1);
    }

    if (is_full()) {
        //printf("warning: fifo full\n");
    } else {
        buf_[wr_cnt_] = *msg;
        if (++wr_cnt_ >= size_) {
            wr_cnt_ = 0;
        }
    }
    utils_event_set(&rxevent_);
    utils_mutex_unlock(&rxmutex_);
}

template <class T>
bool TAsyncFifo<T>::get(T *msg) {
    if (is_empty()) {
        return false;
    }
    if (++rd_cnt_ >= size_) {
        rd_cnt_ = 0;
    }
    *msg = buf_[rd_cnt_];

    if (is_empty()) {
        utils_event_clear(&rxevent_);
    }
    return true;
}

// timeout_ms == 0, then wait INFINTE
template <class T>
bool TAsyncFifo<T>::get(T *msg, int timeout_ms) {
    if (is_empty()) {
        utils_event_wait_ms(&rxevent_, timeout_ms);
    }
    return get(msg);
}
