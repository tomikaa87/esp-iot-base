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
    Created on 2020-08-02
*/

#pragma once

#include "Logger.h"

#include <cstdint>

#define SETTINGS_PACKED __attribute__((packed))

class Settings
{
public:
    struct Data
    {
        uint32_t checksum = 0;

        // Add data structures here
    } SETTINGS_PACKED data;

    Settings();

    void loadDefaults();
    void load();
    void save();

private:
    Logger _log{ "Settings" };

    uint32_t calculateDataChecksum() const;
};