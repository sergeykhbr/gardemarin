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
    enum EState {
        WaitInit,
        CheckWateringInterval,
        DrainBefore,
        OxygenSaturation,
        Watering,
        DrainAfter,
        AdjustLights,
        Servicing,
        States_Total
    };

    bool isPeriodExpired(uint32_t period);
    void switchToState(EState newstate);
    void switchToService();
    void switchToNormal();
    void setDayLights(uint32_t tow);
    void enableHighPressurePump();
    void disableHighPressurePump();
    void enableRelaisLight();
    void disableRelaisLight();
    void enableDrainPump();
    void disableDrainPump();
    void enableOxyPump();
    void disableOxyPump();

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

    float getPlantWeight();
    float getMixWeight();
    float getSewerWeight();
    uint16_t getMoisture();
    uint32_t getTimeOfDay();

 private:
    EState  estate_;
    static const char *STATES_NAMES[States_Total];

    TaskHandle_t taskHandle_;
    bool btnClick_;
    uint32_t epochCnt_;
    uint32_t epochMarker_;
    uint32_t stateSwitchedLast_;

    struct NormalContextType {
        EState estate;
    } normal_;

    float plants_gram_;
    float sewer_gram_;
    float mix_gram_;
    int confirmCnt_;
    int8_t shortWateringCnt_;      // Watering count before drain enabled
};
