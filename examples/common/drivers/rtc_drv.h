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

#include <gardemarin.h>
#include <prjtypes.h>
#include <fwobject.h>
#include <FwAttribute.h>

class RtcDriver : public FwObject {
 public:
    RtcDriver(const char *name);

    // FwObject interface:
    virtual void Init() override;

 protected:
    class DateAttribute : public FwAttribute {
     public:
        DateAttribute(bool *initDone) : FwAttribute("Date", "yyyy.mm.dd"),
            pinitDone_(initDone) {}

        virtual void pre_read() override;
        virtual void post_write() override;
     private:
        bool *pinitDone_;
    };

    class TimeAttribute : public FwAttribute {
     public:
        TimeAttribute(bool *initDone) : FwAttribute("Time", "00hh.mm.ss"),
            pinitDone_(initDone) {}

        virtual void pre_read() override;
        virtual void post_write() override;
     private:
        bool *pinitDone_;
    };

 protected:
    DateAttribute date_;    // hex as dec format: yyyy.mm.dd
    TimeAttribute time_;    // hex as dec format: 00hh.mm.ss
    bool initDone_;
};

