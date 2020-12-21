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

#if defined(IOT_ENABLE_PERSISTENCE) && defined (IOT_PERSISTENCE_USE_EERAM)

#include "ISettingsPersistence.h"
#include "Logger.h"

class EeramPersistence : public ISettingsPersistence
{
public:
    explicit EeramPersistence(int baseAddress, int size);

    bool allocate(int address, size_t size) override;
    bool write(int address, const uint8_t* data, size_t size) override;
    bool read(int address, uint8_t* data, size_t size) override;

private:
    Logger _log{ "EERAM" };
    const int _baseAddress = 0;
    const int _size = 0;
};

#endif // defined(IOT_ENABLE_PERSISTENCE) && defined (IOT_PERSISTENCE_USE_EERAM)