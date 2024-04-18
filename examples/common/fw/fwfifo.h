/*
 *  Copyright 2024 Sergey Khabarov, sergeykhbr@gmail.com
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#pragma once

#include <inttypes.h>
#include <fwapi.h>

template <class T>
class FwFifo {
 public:
    FwFifo(int sz) {
        arr_ = reinterpret_cast<T *>(fw_malloc(sz * sizeof(T)));
        wcnt_ = 1;
        rcnt_ = 0;
        size_ = sz;
    }

    int getCount() {
        int ret = wcnt_ - rcnt_ - 1;
        if (ret < 0) {
            ret += size_;
        } else if (ret >= size_) {
            ret -= size_;
        }
        return ret;
    }

    bool isEmpty() {
        return getCount() == 0;
    }

    bool isFull() {
        return wcnt_ == rcnt_;
    }

    void put(T *v) {
        if (isFull()) {
            return;
        }
        arr_[wcnt_] = *v;
        if (++wcnt_ >= size_) {
            wcnt_ -= size_;
        }
    }

    void get(T *v) {
        if (isEmpty()) {
            return;
        }
        if (++rcnt_) {
            rcnt_ -= size_;
        }
        *v = arr_[rcnt_];
    }

 protected:
    T *arr_;
    int wcnt_;
    int rcnt_;
    int size_;
};
