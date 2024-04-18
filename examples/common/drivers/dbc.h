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
#include <fwobject.h>
#include <FwAttribute.h>
#include <RawInterface.h>

// CAN Data Base Converter
class DbcConverter : public FwObject,
                     public RawListenerInterface {
 public:
    explicit DbcConverter(const char *name);

    // FwObject interface:
    virtual void Init() override;
    virtual void PostInit() override;

    // RawListenerInterface
    virtual void RawCallback(const char *buf, int sz) override;


 private:
    int GetCanMessageDlc();

    uint32_t str2hex32(char *buf, int sz);

    void processRxCanFrame(can_frame_type *frame);

    /**
     * @brief Print SG_ lines for a attribute of the object into DBG output
     */
    int printDbcAttribute(int mux_idx,
                         int start_bit,
                         const char *objname,
                         FwAttribute *atr,
                         const char *dstname);

    /**
     * @brief Print SG_ lines for a specific object into DBG output
     *
     * @warning Do not increment start_bit for now, transmit attribtues in a
     *          separate messages without packing (maybe changed in future)
     */
    void printDbcObject(int obj_idx,
                        FwObject *obj,
                        const char *dstname);

    /**
     * @brief Convert CAN message payload into attribute to generate modify
     *        request
     * @param[in] buf Pointer to CAN message payload
     * @param[in] kind Type of the attribute to convert
     * @param[out] out Pointer to an anttribute to store conversion data
     */
    void ConvertDataToAttribute(const char *buf,
                                EKindType kind,
                                FwAttribute *out);

 private:
    /** Temporary attribute to convert CAN message into modify request. No need
      * to register it in attribute list */
    FwAttribute tmpRx_;
    enum ERawState {
        State_PRM1,     // 1 B = ">"
        State_PRM2,     // 1 B = "!"
        State_CanId,    // 8 B = "12345678" hex in string format
        State_Comma1,   // 1 B = ","
        State_DLC,      // 1 B = "1"..."8"
        State_Comma2,   // 1 B = ","
        State_Payload   // 16 B
    } erawstate_;
    char rawid_[11];
    char rawdlc_;
    char rawpayload_[17];
    int rawcnt_;
};
