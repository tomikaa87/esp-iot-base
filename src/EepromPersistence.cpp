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

EepromPersistence::EepromPersistence(const uint32_t baseAddress, const size_t size)
    : _baseAddress{ baseAddress }
    , _size{ size }
{
    _log.debug_P(PSTR("creating: baseAddress=%u, size=%u"), _baseAddress, size);

    EEPROM.begin(_size);
}

EepromPersistence::~EepromPersistence()
{
    EEPROM.end();
}

bool EepromPersistence::allocate(const uint32_t address, const size_t size)
{
    _log.debug_P(PSTR("allocating: address=%u, size=%u"), address, size);

    if (_baseAddress < 0 || address < 0 || _baseAddress + address + size >= _size) {
        _log.error_P(PSTR("allocation failed, address or size too large"));
        return false;
    }

    return true;
}

bool EepromPersistence::write(const uint32_t address, const uint8_t* data, const size_t size)
{
    _log.debug_P(PSTR("writing: address=%d, size=%u, data=%p"), address, size, data);

    if (_baseAddress < 0 || address < 0 || _baseAddress + address + size >= _size) {
        _log.error_P(PSTR("write failed, address or size too large"));
        return false;
    }

    memcpy(EEPROM.getDataPtr() + _baseAddress + address, data, size);

    if (!EEPROM.commit()) {
        _log.error_P(PSTR("commit failed"));
        return false;
    }

    return true;
}

bool EepromPersistence::read(const uint32_t address, uint8_t* data, const size_t size)
{
    _log.debug_P(PSTR("reading: address=%u, size=%u, data=%p"), address, size, data);

    if (_baseAddress < 0 || address < 0 || _baseAddress + address + size >= _size) {
        _log.error_P(PSTR("write failed, address or size too large"));
        return false;
    }

    memcpy(data, EEPROM.getConstDataPtr() + _baseAddress + address, size);

    return true;
}

#endif // defined(IOT_ENABLE_PERSISTENCE) && defined(IOT_PERSISTENCE_USE_EEPROM)