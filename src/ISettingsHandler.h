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
    Created on 2020-08-09
*/

#pragma once

#include <cstdint>
#include <functional>

// Use this macro to declare data structs for use with Settings.
// It ensures that the struct is packed and doesn't use padding bytes.
#define DECLARE_SETTINGS_STRUCT(__NAME) \
    struct __attribute__((packed)) __NAME

class ISettingsHandler
{
public:
    static constexpr auto ReservedAreaSize{ 8u };

    enum class SaveResult
    {
        NoChange,
        Saved,
        Error
    };

    template <typename T>
    bool registerSetting(T& setting, const size_t address)
    {
        return registerSettingMemory(reinterpret_cast<uint8_t*>(&setting), sizeof(T), address);
    }

    virtual bool registerSettingMemory(uint8_t* ptr, size_t size, size_t address) = 0;

    virtual bool load() = 0;
    virtual SaveResult save(bool skipChangeCheck = false) = 0;

    enum class DefaultsLoadReason {
        ChecksumError,
        PersistenceReadError
    };

    using DefaultsLoader = std::function<void (DefaultsLoadReason)>;

    virtual void setDefaultsLoader(DefaultsLoader&& defaultsLoader) = 0;
};