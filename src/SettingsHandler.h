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

#include "ISettingsHandler.h"
#include "ISettingsPersistence.h"
#include "Logger.h"

#include <cstdint>
#include <cstdlib>
#include <functional>
#include <vector>

class SettingsHandler : public ISettingsHandler
{
public:
    static constexpr auto SaveIntervalMs = 5000;

    explicit SettingsHandler(ISettingsPersistence& persistence);

    bool registerSettingMemory(uint8_t* ptr, size_t size) override;

    void task();

    bool load() override;
    bool save() override;

    void setDefaultsLoader(DefaultsLoader&& defaultsLoader) override;

private:
    Logger _log{ "SettingsHandler" };
    ISettingsPersistence& _persistence;
    int _nextAddress = 0;
    int32_t _saveTimer = 0;
    DefaultsLoader _defaultsLoader;

    struct Header {
        uint16_t checksum;
    };

    struct SettingMetaData {
        uint8_t* ptr;
        size_t size;
        int address;
        uint16_t checksum;
    };

    Header _header;
    std::vector<SettingMetaData> _settings;
};