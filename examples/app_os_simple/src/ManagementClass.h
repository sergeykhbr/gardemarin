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

enum EPlantStage {
    Seeding,    // the sprout grows into a baby plant producing roots ad true leaves (<4)
    Vegetative, // Plants develop sturdy stems and leafy growth with the help of nitrogen
    Budding,    // Plants start producing reproductive parts such as buds, flowers and fruits
    Flowering,  // The burds become flowers and negin forming fruits using phosphorous
    Ripening,   // A fully matured plant with flowers and fruits
    PlantStage_total
};

enum EPlantType {
    Lattuce,
    Strawberry,
    PlantType_total
};

struct ProfileItemType {
    uint32_t dayStartSec;
    uint32_t dayEndSec;
    uint8_t dutyBlue;       // 0..100
    uint8_t dutyRed;        // 0..100
    uint8_t dutyWhite;      // 0..100
};

static const ProfileItemType PlantProfile_[PlantType_total][PlantStage_total] = {
    // Lattuce
    {
        {6*3600, 22*3600, 50,  0, 0},   // Seeding
        {6*3600, 22*3600, 100, 0, 10},  // Vegetative
        {6*3600, 22*3600, 100, 0, 20},  // Budding
        {},                             // Flowering
        {},                             // Rippening
    },
    // Strawberry
    {}
};

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
        CheckMoisture,
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
    void setDayLights(EPlantType plant, EPlantStage stage, uint32_t tow);

    void waitKeyPressed();
    void write_obj_attribute(const char *objname,
                             const char *atrname,
                             char *buf,
                             int sz);
    void write_int8(const char *objname,
                    const char *atrname,
                    int8_t v);
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
    FwAttribute requestToService_;
    FwAttribute lastWatering_;

    EState  estate_;

    TaskHandle_t taskHandle_;
    bool btnClick_;
    uint32_t epochCnt_;
    uint32_t epochMarker_;

    struct NormalContextType {
        EState estate;
    } normal_;

    float plants_gram_;
    float sewer_gram_;
    float mix_gram_;
    int confirmCnt_;
};
