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

#include "Hash.h"
#include "ISettingsHandler.h"
#include "ISettingsPersistence.h"
#include "Logger.h"

#include <cstdint>
#include <cstdlib>
#include <functional>
#include <vector>

template <typename ValueType>
class Setting
{
    friend class SettingsHandler;

public:
    static constexpr auto RequiredMemorySize = sizeof(ValueType) + sizeof(uint16_t);

    bool save() const
    {
        auto* data = reinterpret_cast<const uint8_t*>(&_value);

        if (!_persistence.write(_address, data, sizeof(ValueType))) {
            return false;
        }

        Hash::Crc16 crc;
        crc.update(data, sizeof(ValueType));
        const auto crcValue = crc.value();

        if (
            !_persistence.write(
                _address + sizeof(ValueType),
                reinterpret_cast<const uint8_t*>(&crcValue),
                sizeof(crcValue)
            )
        ) {
            return false;
        }

        return true;
    }

    bool load()
    {
        ValueType value{};

        if (!
            _persistence.read(
                _address,
                reinterpret_cast<uint8_t*>(&value),
                sizeof(ValueType)
            )
        ) {
            return false;
        }

        Hash::Crc16 crc;
        crc.update(reinterpret_cast<const uint8_t*>(&value), sizeof(ValueType));

        uint16_t crcValue{ 0xFFFF };

        if (
            !_persistence.read(
                _address + sizeof(ValueType),
                reinterpret_cast<uint8_t*>(&crcValue),
                sizeof(crcValue)
            )
        ) {
            return false;
        }

        crc.update(crcValue >> 8);
        crc.update(crcValue & 0xFF);

        if (crc.value() == 0) {
            _value = value;
            return true;
        }

        return false;
    }

    ValueType& value()
    {
        return _value;
    }

    const ValueType& value() const
    {
        return _value;
    }

private:
    ValueType _value{};
    std::size_t _address{};
    ISettingsPersistence& _persistence;
    bool _allocated{ false };

    explicit Setting(
        const std::size_t address,
        ISettingsPersistence& persistence
    )
        : _address{ address }
        , _persistence{ persistence }
    {
        if (
            !_persistence.allocate(
                address,
                RequiredMemorySize
            )
        ) {
            abort();
        }
    }
};

class SettingsHandler : public ISettingsHandler
{
public:
    static constexpr auto SaveIntervalMs = 5000;

    explicit SettingsHandler(ISettingsPersistence& persistence);

    bool registerSettingMemory(uint8_t* ptr, size_t size, size_t address) override;

    void task();

    bool load() override;
    SaveResult save(bool skipChangeCheck = false) override;

    void setDefaultsLoader(DefaultsLoader&& defaultsLoader) override;

    template <typename ValueType>
    Setting<ValueType> registerSetting(const size_t fixedSize = 0)
    {
        const auto size = fixedSize > 0 ? fixedSize : Setting<ValueType>::RequiredMemorySize;

        if (size < Setting<ValueType>::RequiredMemorySize) {
            _log.error_P(PSTR("the specified fixed size is not enough to store the value"));
            abort();
        }

        auto address = _nextAddress;
        _nextAddress += size;

        return Setting<ValueType>{ address, _persistence };
    }

private:
    Logger _log{ "SettingsHandler" };
    ISettingsPersistence& _persistence;
    uint32_t _nextAddress = 0;
    int32_t _saveTimer = 0;
    DefaultsLoader _defaultsLoader;

    struct Header {
        uint16_t checksum;
    };

    struct SettingMetaData {
        uint8_t* ptr;
        size_t size;
        uint32_t address;
        uint16_t checksum;
    };

    Header _header;
    std::vector<SettingMetaData> _settings;
};