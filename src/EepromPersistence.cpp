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

#include "EepromPersistence.h"

#if defined(IOT_ENABLE_PERSISTENCE) && defined(IOT_PERSISTENCE_USE_EEPROM)

#include <EEPROM.h>

EepromPersistence::EepromPersistence(const int baseAddress, const int size)
    : _baseAddress{ baseAddress }
    , _size{ size }
{
    _log.debug("creating: baseAddress=%d, size=%d", _baseAddress, size);

    EEPROM.begin(_size);
}

EepromPersistence::~EepromPersistence()
{
    EEPROM.end();
}

bool EepromPersistence::allocate(int address, size_t size)
{
    _log.debug("allocating: address=%d, size=%u", address, size);

    if (_baseAddress < 0 || address < 0 || _baseAddress + address + size >= _size) {
        _log.error("allocation failed, address or size too large");
        return false;
    }

    return true;
}

bool EepromPersistence::write(int address, const uint8_t* data, size_t size)
{
    _log.debug("writing: address=%d, size=%u, data=%p", address, size, data);

    if (_baseAddress < 0 || address < 0 || _baseAddress + address + size >= _size) {
        _log.error("write failed, address or size too large");
        return false;
    }

    memcpy(EEPROM.getDataPtr() + _baseAddress + address, data, size);

    if (!EEPROM.commit()) {
        _log.error("commit failed");
        return false;
    }

    return true;
}

bool EepromPersistence::read(int address, uint8_t* data, size_t size)
{
    _log.debug("reading: address=%d, size=%u, data=%p", address, size, data);

    if (_baseAddress < 0 || address < 0 || _baseAddress + address + size >= _size) {
        _log.error("write failed, address or size too large");
        return false;
    }

    memcpy(data, EEPROM.getConstDataPtr() + _baseAddress + address, size);

    return true;
}

#endif // defined(IOT_ENABLE_PERSISTENCE) && defined(IOT_PERSISTENCE_USE_EEPROM)