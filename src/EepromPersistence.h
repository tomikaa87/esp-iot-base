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
    Created on 2020-12-21
*/

#pragma once

#if defined(IOT_ENABLE_PERSISTENCE) && defined(IOT_PERSISTENCE_USE_EEPROM)

#include "ISettingsPersistence.h"
#include "Logger.h"

class EepromPersistence : public ISettingsPersistence
{
public:
    explicit EepromPersistence(uint32_t baseAddress, size_t size);
    ~EepromPersistence();

    bool allocate(uint32_t address, size_t size) override;
    bool write(uint32_t address, const uint8_t* data, size_t size) override;
    bool read(uint32_t address, uint8_t* data, size_t size) override;

private:
    Logger _log{ "EEPROM" };
    const uint32_t _baseAddress = 0;
    const size_t _size = 0;
};

#endif // defined(IOT_ENABLE_PERSISTENCE) && defined (IOT_EEPROM_PERSISTENCE)