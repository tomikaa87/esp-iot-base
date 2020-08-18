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

#include "BaseConfig.h"
#include "EeramPersistence.h"

#ifdef IOT_ENABLE_PERSISTENCE

#include "drivers/EERAM.h"

EeramPersistence::EeramPersistence(const int baseAddress)
    : _baseAddress{ baseAddress }
{
    _log.debug("creating: baseAddress=%d", _baseAddress);
}

bool EeramPersistence::allocate(int address, size_t size)
{
    _log.debug("allocating: address=%d, size=%u", address, size);

    if (_baseAddress < 0 || address < 0 || _baseAddress + address + size >= Config::Persistence::EeramSize) {
        _log.error("allocation failed, address or size too large");
        return false;
    }

    return true;
}

bool EeramPersistence::write(int address, const uint8_t* data, size_t size)
{
    _log.debug("writing: address=%d, size=%u, data=%p", address, size, data);

    if (_baseAddress < 0 || address < 0 || _baseAddress + address + size >= Config::Persistence::EeramSize) {
        _log.error("write failed, address or size too large");
        return false;
    }

    const auto ok = Drivers::EERAM::write(_baseAddress + address, data, size);
    if (!ok) {
        _log.error("write failed, communication error");
        return false;
    }

    if (!Drivers::EERAM::getStatus().ase) {
        _log.debug("enabling ASE");
        Drivers::EERAM::setAseEnabled(true);
    }

    return true;
}

bool EeramPersistence::read(int address, uint8_t* data, size_t size)
{
    _log.debug("reading: address=%d, size=%u, data=%p", address, size, data);

    if (_baseAddress < 0 || address < 0 || _baseAddress + address + size >= Config::Persistence::EeramSize) {
        _log.error("write failed, address or size too large");
        return false;
    }

    const auto ok = Drivers::EERAM::read(_baseAddress + address, data, size);
    if (!ok) {
        _log.error("read failed, communication error");
        return false;
    }

    return true;
}

#endif // IOT_ENABLE_PERSISTENCE