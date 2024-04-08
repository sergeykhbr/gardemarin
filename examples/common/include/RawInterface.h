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

#include "CommonInterface.h"

/**
 * @brief Declaration of the callback interface for the RawInterface.
 *        This class should be implemented by modules that intend to process
 *        received data buffer.
 */
class RawListenerInterface : public CommonInterface {
 public:
    RawListenerInterface() : CommonInterface("RawListenerInterface") {}

    /**
     * @brief Callback method called when new portion of data available at
     *        module implementing RawInterface interface
     * @param[in] buf Pointer to buffer storing input data
     * @param[in] sz Input buffer size in Bytes
     */
    virtual void RawCallback(const char *buf, int sz) = 0;
};


/**
 * @brief Declaration of the generic RawInterface virtual class.
 *        This class should be inherited and implemented by modules that
 *        transmit or receive unformatted data.
 */
class RawInterface : public CommonInterface {
 public:
    RawInterface() : CommonInterface("RawInterface") {}

    /**
     * @brief Write binary data buffer into module to transmit it into
     *        communication port or for the processing.
     * param[in] buf Pointer to the processing buffer data
     * param[in] sz Processing buffer data size in bytes
     */
    virtual void WriteData(const char *buf, int sz) = 0;

    /**
     * @brief Register callback interface called when new read data available
     * @param[in] iface Pointer to callback interface
     */
    virtual void RegisterRawListener(RawListenerInterface *iface) = 0;
};
