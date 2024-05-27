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

#include <fwattribute.h>
#include <fwobject.h>

FwAttribute::FwAttribute(const char *name, const char *descr) :
    CommonInterface("FwAttribute") {
    kind_ = Attr_Invalid;
    name_ = name;
    jsonDescription_ = descr;
    u_.u64 = 0;
}

/**
 * @brief Reset attribute value into invalid value
 */
void FwAttribute::make_nil() {
    kind_ = Attr_Nil;
    u_.u64 = 0;
    post_write();
}

/**
 * @brief Interpret attribute as the string pointer
 * @param[in] v Pointer to a string that should be stored in attribute
 */
void FwAttribute::make_string(const char *v) {
    kind_ = Attr_String;
    u_.string = v;
    post_write();
}

/**
* @brief Get attribute's string pointer. Attribute do not check actual
*        type so external functional should properly handle it.
* @return Pointer to a string that stored in the attribute
*/
const char *FwAttribute::to_string() {
    pre_read();
    return u_.string;
}

/**
 * @brief Use attribute as a storage of int8_t type value
 * @param[in] v Signed byte value to store in the attribute
 */
void FwAttribute::make_int8(int8_t v) {
    kind_ = Attr_Int8;
    u_.i8 = v;
    post_write();
}

/**
* @brief Interpret attribute value as int8_t type
* @return Signed byte value stored in the attribute
*/
int8_t FwAttribute::to_int8() {
    pre_read();
    return u_.i8;
}

/**
* @brief Use attribute as a storage of uint8_t type value
* @param[in] v Unsigned byte value to store in the attribute
*/
void FwAttribute::make_uint8(uint8_t v) {
    kind_ = Attr_UInt8;
    u_.u8 = v;
    post_write();
}

/**
* @brief Interpret attribute value as uint8_t type
* @return Unsigned byte value stored in the attribute
*/
uint8_t FwAttribute::to_uint8() {
    pre_read();
    return u_.u8;
}

/**
* @brief Use attribute as a storage of int16_t type value
* @param[in] v Signed word value to store in the attribute
*/
void FwAttribute::make_int16(int16_t v) {
    kind_ = Attr_Int16;
    u_.i16 = v;
    post_write();
}

/**
* @brief Interpret attribute value as int16_t type
* @return Signed word value stored in the attribute
*/
int16_t FwAttribute::to_int16() {
    pre_read();
    return u_.i16;
}

/**
* @brief Use attribute as a storage of uint16_t type value
* @param[in] v Unsigned word value to store in the attribute
*/
void FwAttribute::make_uint16(uint16_t v) {
    kind_ = Attr_UInt16;
    u_.u16 = v;
    post_write();
}

/**
* @brief Interpret attribute value as uint16_t type
* @return Unsigned word value stored in the attribute
*/
uint16_t FwAttribute::to_uint16() {
    pre_read();
    return u_.u16;
}

/**
* @brief Interpret attribute value as int32_t type
* @return Signed double word value stored in the attribute
*/
void FwAttribute::make_int32(int32_t v) {
    kind_ = Attr_Int32;
    u_.i32 = v;
    post_write();
}

/**
* @brief Interpret attribute value as int32_t type
* @return Signed double word value stored in the attribute
*/
int32_t FwAttribute::to_int32() {
    pre_read();
    return u_.i32;
}

/**
* @brief Interpret attribute value as uint32_t type
* @return Unsigned double word value stored in the attribute
*/
void FwAttribute::make_uint32(uint32_t v) {
    kind_ = Attr_UInt32;
    u_.u32 = v;
    post_write();
}

/**
* @brief Interpret attribute value as uint32_t type
* @return Unsigned double word value stored in the attribute
*/
uint32_t FwAttribute::to_uint32() {
    pre_read();
    return u_.u32;
}

/**
* @brief Interpret attribute value as int64_t type
* @return Signed quad word value stored in the attribute
*/
void FwAttribute::make_int64(int64_t v) {
    kind_ = Attr_Int64;
    u_.i64 = v;
    post_write();
}

/**
* @brief Interpret attribute value as int64_t type
* @return Signed quad word value stored in the attribute
*/
int64_t FwAttribute::to_int64() {
    pre_read();
    return u_.i64;
}

/**
* @brief Interpret attribute value as uint64_t type
* @return Unsigned quad word value stored in the attribute
*/
void FwAttribute::make_uint64(uint64_t v) {
    kind_ = Attr_UInt64;
    u_.u64 = v;
    post_write();
}

/**
* @brief Interpret attribute value as uint64_t type
* @return Unsigned quad word value stored in the attribute
*/
uint64_t FwAttribute::to_uint64() {
    pre_read();
    return u_.u64;
}

/**
* @brief Interpret attribute value as float type
* @return Single precision floating point value stored in the attribute
*/
void FwAttribute::make_float(float v) {
    kind_ = Attr_Float;
    u_.f = v;
    post_write();
}

/**
* @brief Interpret attribute value as float type
* @return Single precision floating point value stored in the attribute
*/
float FwAttribute::to_float() {
    pre_read();
    return u_.f;
}

/**
* @brief Interpret attribute value as double type
* @return Double precision floating point value stored in the attribute
*/
void FwAttribute::make_double(double v) {
    kind_ = Attr_Double;
    u_.d = v;
    post_write();
}

/**
* @brief Interpret attribute value as double type
* @return Double precision floating point value stored in the attribute
*/
double FwAttribute::to_double() {
    pre_read();
    return u_.d;
}

/**
* @brief Write raw byte value into specified position
* param[in] idx Byte index when number represented as a sequence of bytes.
*               Byte index is getting with modulo 8 to avoid memory
*               corruption because the longest stored value in the union
*               is the long long (8-bytes) size.
* param[in] v Byte value
*/
void FwAttribute::set_byte(int idx, char v) {
    u_.data[idx & 0x7] = v;
}

void FwAttribute::write(char *buf, int sz, bool silence) {
    for (int i = 0; i < sz; i++) {
        u_.data[i & 0x7] = buf[i];
    }
    post_write();

    if (!silence) {
        // Send to external interface (CAN, UART) attribute was updated
    }
}

void FwAttribute::read(char *buf, int sz) {
    pre_read();
    for (int i = 0; i < BitSize() / 8; i++) {
        buf[i] = u_.data[i & 0x7];
    }
}

/**
 * @brief Detect number of bits need to represent attribute in DBC message
 * @return Number of bits need to represent attribute
 */
int FwAttribute::BitSize() {
    int ret = 8;
    switch (kind_) {
    case Attr_Int16:
        ret = 16;
        break;
    case Attr_UInt16:
        ret = 16;
        break;
    case Attr_Int32:
        ret = 32;
        break;
    case Attr_UInt32:
        ret = 32;
        break;
    case Attr_Int64:
        ret = 64;
        break;
    case Attr_UInt64:
        ret = 64;
        break;
    case Attr_Float:
        ret = 32;
        break;
    case Attr_Double:
        ret = 64;
        break;
    default:;
    }
    return ret;
}
