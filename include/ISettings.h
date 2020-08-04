/*
    This file is part of esp-iot-base.

    esp-iot-base is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    esp-iot-base is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with esp-iot-base.  If not, see <http://www.gnu.org/licenses/>.

    Author: Tamas Karpati
    Created on 2020-08-04
*/

#pragma once

#include "Config.h"

#include <cstdint>
#include <cstdlib>
#include <functional>

// Use this macro to declare data structs for use with Settings.
// It ensures that the struct is packed and doesn't use padding bytes.
#define DECLARE_SETTINGS_STRUCT(__NAME) \
    struct __attribute__((packed)) __NAME

class ISettings
{
public:
    struct AbstractSettingsData {
        uint32_t checksum = 0;
        uint8_t* const ptr = nullptr;
        const size_t size = 0;

    protected:
        AbstractSettingsData(uint8_t* const ptr, const size_t size)
            : ptr{ ptr }
            , size{ size }
        {}
    };

    template <typename DataType>
    struct SettingsData : public AbstractSettingsData {
        SettingsData()
            : AbstractSettingsData{ reinterpret_cast<uint8_t*>(&data), sizeof(DataType) }
        {
            static_assert(
                sizeof(DataType) <= Config::EeramAddresses::SettingsSize,
                "Settings data doesn't fit in the allocated space"
            );
        }

        DataType data;
    };

    enum class LoadDefaultsReason {
        ChecksumError,
        ReadFailure
    };

    using LoadDefaultsRequestedHandler = std::function<void (LoadDefaultsReason reason)>;

    virtual bool save(AbstractSettingsData& data) = 0;
    virtual bool load(AbstractSettingsData& data) = 0;

    virtual void setLoadDefaultsRequestedHandler(LoadDefaultsRequestedHandler&& handler) = 0;
};