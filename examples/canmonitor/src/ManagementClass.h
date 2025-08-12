/*
 *  Copyright 2025 Sergey Khabarov, sergeykhbr@gmail.com
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

#include <prjtypes.h>
#include <fwobject.h>
#include <fwattribute.h>
#include <KeyInterface.h>
#include <task.h>

class ManagementClass : public FwObject,
                        public KeyListenerInterface {
 public:
    ManagementClass(TaskHandle_t taskHandle);

    // FwObject
    virtual void Init() override;
    virtual void PostInit() override;

    // KeyListenerInterface
    virtual void keyPressed() override;
    virtual void keyReleased() override {}
    virtual void keyClick() override {}
    virtual void keyDoubleClick() override {}
    virtual void keyLongClick() override {}

 public:
    void update();

 protected:
    void waitKeyPressed();
    void write_obj_attribute(const char *objname,
                             const char *atrname,
                             char *buf,
                             int sz);
    void write_int8(const char *objname,
                    const char *atrname,
                    int8_t v);
    void write_uint32(const char *objname,
                      const char *atrname,
                      uint32_t v);
    int8_t read_int8(const char *objname,
                     const char *atrname);
    uint16_t read_uint16(const char *objname,
                        const char *atrname);
    uint32_t read_uint32(const char *objname,
                         const char *atrname);
    float read_float32(const char *objname,
                         const char *atrname);

    uint32_t getTimeOfDay();

 private:
    TaskHandle_t taskHandle_;
    bool btnClick_;
    uint32_t epochCnt_;
    uint32_t epochMarker_;
};
