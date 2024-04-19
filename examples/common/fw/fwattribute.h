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
#include <CommonInterface.h>

/**
 * @brief FwAttribute types as enum. Attribute should provide safe conversion
 *        of raw data buffer into any value of the following types:
 */
typedef enum EKindType
{
    Attr_Invalid,
    Attr_String,
    Attr_Int8,
    Attr_UInt8,
    Attr_Int16,
    Attr_UInt16,
    Attr_Int32,
    Attr_UInt32,
    Attr_Int64,
    Attr_UInt64,
    Attr_Float,
    Attr_Double,
    // not supported:
    Attr_List,
    Attr_Data,
    Attr_Nil,
    Attr_Dict,
    Attr_PyObject,
} EKindType;

/**
 * @brief FwAttribute declaration. All FwAttribute instance should have
 *        string identificator and optional description in JSON-format
 *        to better describe attribtue parameters in DBC format
 */
class FwAttribute : public CommonInterface {
 public:
    /** 
     * @brief FwAttribute constructor
     * @param[in] name Mandatory attribute name as a the pointer to a string
     * @param[in] descr Optional attribute description in as the JSON formatted
     *             string. The fllowing json-dictionary keys supported in DBC 
     *             converter:
     *               'Min'
     *               'Max'
     *               'Scale'
     *               'enum'
     *   Example:
     *    "{'Min':4, 'Max':25, 'Scale':0.2, 'enum':'1 Value1 0 Value0'}"
     */
    explicit FwAttribute(const char *name,
                         const char *descr = 0);

    /** 
     * @brief Get attribtue name
     * @return Attribute name in human readable format
     */
    const char *name() { return name_; }

    /** 
     * @brief Get attribute type
     * @return One of supported attribute's formats as enum value
     */
    EKindType kind() { return kind_; }

    /** 
     * @brief Get Attribute description in JSON format
     * @return Pointer to json-formatted string if description was specified
     *         in attribute constructor or zero otherwise
     */
    const char *description() { return jsonDescription_; }

    /**
     * @brief Reset attribute value into invalid value
     */
    void make_nil();

    /**
     * @brief Interpret attribute as the string pointer
     * @param[in] v Pointer to a string that should be stored in attribute
     */
    void make_string(const char *v);

    /**
     * @brief Get attribute's string pointer. Attribute do not check actual
     *        type so external functional should properly handle it.
     * @return Pointer to a string that stored in the attribute
     */
    const char *to_string();

    /**
     * @brief Use attribute as a storage of int8_t type value
     * @param[in] v Signed byte value to store in the attribute
     */
    void make_int8(int8_t v);

    /**
     * @brief Interpret attribute value as int8_t type
     * @return Signed byte value stored in the attribute
     */
    int8_t to_int8();

    /**
     * @brief Use attribute as a storage of uint8_t type value
     * @param[in] v Unsigned byte value to store in the attribute
     */
    void make_uint8(uint8_t v);

    /**
     * @brief Interpret attribute value as uint8_t type
     * @return Unsigned byte value stored in the attribute
     */
    uint8_t to_uint8();

    /**
     * @brief Use attribute as a storage of int16_t type value
     * @param[in] v Signed word value to store in the attribute
     */
    void make_int16(int16_t v);

    /**
     * @brief Interpret attribute value as int16_t type
     * @return Signed word value stored in the attribute
     */
    int16_t to_int16();

    /**
     * @brief Use attribute as a storage of uint16_t type value
     * @param[in] v Unsigned word value to store in the attribute
     */
    void make_uint16(uint16_t v);

    /**
     * @brief Interpret attribute value as uint16_t type
     * @return Unsigned word value stored in the attribute
     */
    uint16_t to_uint16();

    /**
     * @brief Interpret attribute value as int32_t type
     * @return Signed double word value stored in the attribute
     */
    void make_int32(int32_t v);

    /**
     * @brief Interpret attribute value as int32_t type
     * @return Signed double word value stored in the attribute
     */
    int32_t to_int32();

    /**
     * @brief Interpret attribute value as uint32_t type
     * @return Unsigned double word value stored in the attribute
     */
    void make_uint32(uint32_t v);

    /**
     * @brief Interpret attribute value as uint32_t type
     * @return Unsigned double word value stored in the attribute
     */
    uint32_t to_uint32();

    /**
     * @brief Interpret attribute value as int64_t type
     * @return Signed quad word value stored in the attribute
     */
    void make_int64(int64_t v);

    /**
     * @brief Interpret attribute value as int64_t type
     * @return Signed quad word value stored in the attribute
     */
    int64_t to_int64();

    /**
     * @brief Interpret attribute value as uint64_t type
     * @return Unsigned quad word value stored in the attribute
     */
    void make_uint64(uint64_t v);

    /**
     * @brief Interpret attribute value as uint64_t type
     * @return Unsigned quad word value stored in the attribute
     */
    uint64_t to_uint64();

    /**
     * @brief Interpret attribute value as float type
     * @return Single precision floating point value stored in the attribute
     */
    void make_float(float v);

    /**
     * @brief Interpret attribute value as float type
     * @return Single precision floating point value stored in the attribute
     */
    float to_float();

    /**
     * @brief Interpret attribute value as double type
     * @return Double precision floating point value stored in the attribute
     */
    void make_double(double v);

    /**
     * @brief Interpret attribute value as double type
     * @return Double precision floating point value stored in the attribute
     */
    double to_double();

    /**
     * @brief Write raw byte value into specified position
     * param[in] idx Byte index when number represented as a sequence of bytes.
     *               Byte index is getting with modulo 8 to avoid memory
     *               corruption because the longest stored value in the union
     *               is the long long (8-bytes) size.
     * param[in] v Byte value
     */
    void set_byte(int idx, char v);

    /**
     * @brief modify attribute throught external interface
     */
    void write(char *buf, int sz);

    /**
     * @brief modify attribute throught external interface
     */
    void read(char *buf, int sz);

    virtual void pre_read() {}
    virtual void post_write() {}

    /**
     * @brief Detect number of bits need to represent attribute in DBC message
     * @return Number of bits need to represent attribute
     */
    int BitSize();


 protected:
    /** Union to safly convert data buffer into specified value type */
    union
    {
        const char *string;
        int8_t i8;
        uint8_t u8;
        int16_t i16;
        uint16_t u16;
        int32_t i32;
        uint32_t u32;
        int64_t i64;
        uint64_t u64;
        double d;
        float f;
        char data[8];
    } u_;
    /** Pointer to an attribute name */
    const char *name_;
    /** Pointer to an attribute description */
    const char *jsonDescription_;
    /** Attribute type */
    EKindType kind_;
};
