/*
 *  Copyright 2019 Sergey Khabarov, sergeykhbr@gmail.com
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

#include <cstdio>
#include <cstring>  // memcpy definition
#include "autobuffer.h"

AutoBuffer::AutoBuffer() {
    buf_ = NULL;
    buf_len_ = 0;
    buf_size_ = 0;
}

AutoBuffer::~AutoBuffer() {
    if (buf_) {
        delete [] buf_;
    }
}

void AutoBuffer::write_bin(const char *p, int sz) {
    if (buf_len_ + sz >= buf_size_) {
        if (buf_size_ == 0) {
            buf_size_ = 1024;
            while (buf_size_ < sz) {
                buf_size_ <<= 1;
            }
            buf_ = new char[buf_size_];
        } else {
            while (buf_len_ + sz >= buf_size_) {
                buf_size_ <<= 1;
            }
            char *t1 = new char[buf_size_];
            memcpy(t1, buf_, buf_len_);
            delete [] buf_;
            buf_ = t1;
        }
    }
    memcpy(&buf_[buf_len_], p, sz);
    buf_len_ += sz;
    buf_[buf_len_] = '\0';
}

void AutoBuffer::write_string(const char s) {
    write_bin(&s, 1);
}

void AutoBuffer::write_string(const char *s) {
    write_bin(s, static_cast<int>(strlen(s)));
}

void AutoBuffer::write_uint64(uint64_t v) {
    char tmp[128];
#if defined(_WIN32) || defined(__CYGWIN__)
    int sz = sprintf_s(tmp, sizeof(tmp), "0x%I64x", v);
#else
    int sz = sprintf(tmp, "0x%llx", static_cast<long long unsigned int>(v));
#endif
    write_bin(tmp, sz);
}

void AutoBuffer::write_byte(uint8_t v) {
    char tmp[8];
#if defined(_WIN32) || defined(__CYGWIN__)
    int sz = sprintf_s(tmp, sizeof(tmp), "0x%02X", v);
#else
    int sz = sprintf(tmp, "0x%02X", v);
#endif
    write_bin(tmp, sz);
}
