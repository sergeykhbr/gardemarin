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
#include <fwapi.h>
#include <uart.h>
#include <canframe.h>
#include "dbc.h"

/**
 * @brief DBC converter constructor
 * @param[in] name Module name used as the string identificator
 */
DbcConverter::DbcConverter(const char *name) : FwObject(name),
    tmpRx_("trx") {
}

/**
 * @brief Overrided FwObject method to register attribtues and interface
 */
void DbcConverter::Init() {
    RegisterInterface(static_cast<RawListenerInterface *>(this));
}

void DbcConverter::PostInit() {
    FwObject *obj;
    FwList *p;
    int obj_idx = 0;
    int start_bit = 0;

    // Hardcoded DBC header, do not depends of the actual values
    uart_printf("VERSION \"\"\r\n\r\n");
    // New symbol entries:
    uart_printf("NS_ :\r\n");
    uart_printf("\tBA_\r\n");
    uart_printf("\tBA_DEF\r\n");
    uart_printf("\tBA_DEF_DEF_\r\n");
    uart_printf("\tBA_DEF_DEF_REL_\r\n");
    uart_printf("\tBA_DEF_REL_\r\n");
    uart_printf("\tBA_DEF_SGTYPE_\r\n");
    uart_printf("\tBA_REL_\r\n");
    uart_printf("\tBA_SGTYPE_\r\n");
    uart_printf("\tBO_TX_BU_\r\n");
    uart_printf("\tBU_BO_REL_\r\n");
    uart_printf("\tBU_EV_REL_\r\n");
    uart_printf("\tBU_SG_REL_\r\n");
    uart_printf("\tCAT_\r\n");
    uart_printf("\tCAT_DEF_\r\n");
    uart_printf("\tCM_\r\n");
    uart_printf("\tENVVAR_DATA_\r\n");
    uart_printf("\tEV_DATA_\r\n");
    uart_printf("\tFILTER\r\n");
    uart_printf("\tNS_DESC_\r\n");
    uart_printf("\tSGTYPE_\r\n");
    uart_printf("\tSGTYPE_VAL_\r\n");
    uart_printf("\tSG_MUL_VAL_\r\n");
    uart_printf("\tSIGTYPE_VALTYPE_\r\n");
    uart_printf("\tSIG_GROUP_\r\n");
    uart_printf("\tSIG_TYPE_REF_\r\n");
    uart_printf("\tSIG_VALTYPE_\r\n");
    uart_printf("\tVAL_\r\n");
    uart_printf("\tVAL_TABLE_\r\n");

    // Baudrate definition, legacy, not used but must be presence
    uart_printf("BS_:\r\n\r\n");
    // 2 defined Nodes:
    uart_printf("BU_: GARDEMARIN ANY\r\n\r\n\r\n");

    // Debug message (always 8 bytes for now)
    uart_printf("BO_ %d GARDEMARIN_DEBUG: 8 GARDEMARIN\r\n\r\n",
                0x80000000 | CAN_MSG_ID_DBG_OUTPT);  // extended format


    p = fw_get_objects_list();
    obj_idx = 0;
    while (p) {
        obj = reinterpret_cast<FwObject *>(fwlist_get_payload(p));
        printDbcObject(obj_idx, obj, "GARDEMARIN");
        p = p->next;
        obj_idx++;
    }
    uart_printf("\r\n");

    // Add Nodes description:
    uart_printf("CM_ BU_ GARDEMARIN \"Prototype of the aeroponic "
                 "system base on ST32F407* CPU\";\r\n");
    uart_printf("CM_ BU_ ANY \"Any target like host PC or Hardware\";\r\n");
    uart_printf("CM_ BO_ %d \"Debug output strings\";\r\n",
                0x80000000 | CAN_MSG_ID_DBG_OUTPT);
    uart_printf("\r\n");

    // TODO: add enum format output
}

/**
 * @brief RawListenerInterface method called when new portion of 
 *        data available at module implementing RawInterface interface
 * @param[in] buf Pointer to buffer storing input data
 * @param[in] sz Input buffer size in Bytes
 */
void DbcConverter::RawCallback(const char *buf, int sz) {
    int obj_idx;
    int atr_idx;
    FwObject *obj;
    FwAttribute *attr;

    obj_idx = buf[0] & 0xFF;
    atr_idx = buf[1] & 0xFF;

    obj = reinterpret_cast<FwObject *>(fw_get_obj_by_index(obj_idx));
    if (obj != 0) {
        attr = reinterpret_cast<FwAttribute *>(fw_get_attr_by_index(obj, atr_idx));
        if (attr) {
            uart_printf("DBC write to %s::%s\r\n",
                        obj->ObjectName(),
                        attr->name());
            ConvertDataToAttribute(buf, attr->kind(), &tmpRx_);

            // Generate Request to object that attribtue was modified
            // externally
            obj->ModifyAttribute(attr, &tmpRx_);
        }
    }
}

/**
 * @brief Convert CAN message payload into attribute to generate modify
 *        request
 * @param[in] buf Pointer to CAN message payload
 * @param[in] kind Type of the attribute to convert
 * @param[out] out Pointer to an anttribute to store conversion data
 */
void DbcConverter::ConvertDataToAttribute(const char *buf,
                                        EKindType kind,
                                        FwAttribute *out) {
    // Bytes [0] and [1] used for identification
    switch (kind) {
    case Attr_Int8:
        out->make_int8(buf[2]);
        break;
    case Attr_UInt8:
        out->make_uint8(static_cast<uint8_t>(buf[2]));
        break;
    case Attr_Int16:
        out->make_int16(static_cast<int16_t>(buf[2])
                     | (static_cast<int16_t>(buf[3]) << 8));
        break;
    case Attr_UInt16:
        out->make_uint16(static_cast<uint16_t>(buf[2])
                      | (static_cast<uint16_t>(buf[3]) << 8));
        break;
    case Attr_Int32:
        out->make_int32(static_cast<int32_t>(buf[2])
                     | (static_cast<int32_t>(buf[3]) << 8)
                     | (static_cast<int32_t>(buf[4]) << 16)
                     | (static_cast<int32_t>(buf[5]) << 24));
        break;
    case Attr_UInt32:
        out->make_uint32(static_cast<uint32_t>(buf[2])
                     | (static_cast<uint32_t>(buf[3]) << 8)
                     | (static_cast<uint32_t>(buf[4]) << 16)
                     | (static_cast<uint32_t>(buf[5]) << 24));
        break;
    case Attr_Int64:
        out->make_int64(static_cast<int64_t>(buf[2])
                     | (static_cast<int64_t>(buf[3]) << 8)
                     | (static_cast<int64_t>(buf[4]) << 16)
                     | (static_cast<int64_t>(buf[5]) << 24)
                     | (static_cast<int64_t>(buf[6]) << 32)
                     | (static_cast<int64_t>(buf[7]) << 40)
                     | (static_cast<int64_t>(buf[8]) << 48)
                     | (static_cast<int64_t>(buf[9]) << 56));
        break;
    case Attr_UInt64:
        out->make_uint64(static_cast<uint64_t>(buf[2])
                     | (static_cast<uint64_t>(buf[3]) << 8)
                     | (static_cast<uint64_t>(buf[4]) << 16)
                     | (static_cast<uint64_t>(buf[5]) << 24)
                     | (static_cast<uint64_t>(buf[6]) << 32)
                     | (static_cast<uint64_t>(buf[7]) << 40)
                     | (static_cast<uint64_t>(buf[8]) << 48)
                     | (static_cast<uint64_t>(buf[9]) << 56));
        break;
    case Attr_Float:
        out->set_byte(0, buf[2]);
        out->set_byte(1, buf[3]);
        out->set_byte(2, buf[4]);
        out->set_byte(3, buf[5]);
        out->make_float(out->to_float());
        break;
    case Attr_Double:
        out->set_byte(0, buf[2]);
        out->set_byte(1, buf[3]);
        out->set_byte(2, buf[4]);
        out->set_byte(3, buf[5]);
        out->set_byte(4, buf[6]);
        out->set_byte(5, buf[7]);
        out->set_byte(6, buf[8]);
        out->set_byte(7, buf[9]);
        out->make_double(out->to_double());
        break;
    default:
        out->make_nil();
    }
}


/**
 * @brief CAN messages length
 * @todo find the longest message and use its size as DLC
 */
int DbcConverter::GetCanMessageDlc() {
    return 8;
}

/**
 * @brief Print SG_ lines for a attribute of the object into DBG output
 */
int DbcConverter::printDbcAttribute(int mux_idx,
                                    int start_bit,
                                    const char *objname,
                                    FwAttribute *atr,
                                    const char *dstname) {

    int bit_sz = atr->BitSize();
    uart_printf(" SG_ %s_%s", objname, atr->name());

    if (mux_idx != -1) {
        uart_printf(" m%d", mux_idx);
    }


    // @1+    = little-endian, unsigned. Don't change it
    // (1,0)  = (scale,offset)
    // [0,0]  = (min,max)
    // \"\"   = units
    uart_printf(" : %d|%d@1+ (1,0) [0|0] \"\" %s\r\n",
        start_bit,
        bit_sz,
        dstname
        );

    start_bit += bit_sz;
    return start_bit;
}

/**
 * @brief Print SG_ lines for a specific object into DBG output
 *
 * @warning Do not increment start_bit for now, transmit attribtues in a
 *          separate messages without packing (maybe changed in future)
 */
void DbcConverter::printDbcObject(int obj_idx,
                                 FwObject *obj,
                                 const char *dstname) {
    int atr_idx = 0;
    int start_bit = 0;
    int mux_idx = -1;
    int bits_total = 0;
    FwList *atrlist = obj->GetAttributes();
    FwAttribute *atr;

    while (atrlist) {
        atrlist = atrlist->next;
        if (bits_total < atr->BitSize()) {
             bits_total = atr->BitSize();
        }
        atr_idx++;
    }
    if (atr_idx > 1) {
        bits_total += 8;
        start_bit = 8;
        mux_idx = 0;
    }

    uart_printf("BO_ %d %s: %d GARDEMARIN\r\n",
                0x80000000 | (OBJ_MSG_ID(obj_idx)),
                obj->ObjectName(),
                (bits_total + 7) / 8);


    // Use multiplexer [7:0] to access attributes:
    if (atr_idx > 1) {
        uart_printf(" SG_ %s_mux M", obj->ObjectName());

        // @1+    = little-endian, unsigned. Don't change it
        // (1,0)  = (scale,offset)
        // [0,0]  = (min,max)
        // \"\"   = units
        uart_printf(" : 0|8d@1+ (1,0) [0|0] \"\" %s\r\n", dstname);
    }

    atrlist = obj->GetAttributes();
    while (atrlist) {
        atr = reinterpret_cast<FwAttribute *>(fwlist_get_payload(atrlist));

        start_bit = printDbcAttribute(mux_idx,
                                      start_bit,
                                      obj->ObjectName(),
                                      atr,
                                      dstname);
        atrlist = atrlist->next;
        if (mux_idx != -1) {
            start_bit = 8;
            mux_idx++;
        }
    }
    uart_printf("\r\n");
}
