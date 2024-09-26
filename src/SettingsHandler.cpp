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

#include "Hash.h"
#include "SettingsHandler.h"

#include <Arduino.h>

SettingsHandler::SettingsHandler(ISettingsPersistence& persistence)
    : _persistence(persistence)
    , _nextAddress(sizeof(Header))
    , _saveTimer(millis())
{
}

void SettingsHandler::task()
{
    if (millis() - _saveTimer >= SaveIntervalMs) {
        _saveTimer = millis();
        save(false);
    }
}

bool SettingsHandler::registerSettingMemory(uint8_t* ptr, const size_t size, const size_t address)
{
    // Can't register setting in the reserved area
    if (address < 8) {
        return false;
    }

    for (const auto& smd : _settings) {
        // Check if the ranges doesn't overlap: x2 < y1 || x1 > y2
        if (!((smd.address + smd.size) < address || smd.address > (address + size))) {
            return false;
        }
    }

    SettingMetaData smd{};
    smd.ptr = ptr;
    smd.size = size;
    smd.address = address;
    smd.checksum = Hash::Fletcher16::calculate(ptr, size);

    _settings.push_back(smd);

    return _persistence.allocate(smd.address, smd.size);
}

bool SettingsHandler::load()
{
    if (!_persistence.read(0, reinterpret_cast<uint8_t*>(&_header), sizeof(Header))) {
        if (_defaultsLoader) {
            _defaultsLoader(DefaultsLoadReason::PersistenceReadError);
        }
        return false;
    }

    Hash::Fletcher16 hdrCksum;

    for (auto& s : _settings) {
        if (!_persistence.read(s.address, s.ptr, s.size)) {
            // log error
            continue;
        }
        s.checksum = Hash::Fletcher16::calculate(s.ptr, s.size);
        hdrCksum.data(reinterpret_cast<uint8_t*>(&s.checksum), sizeof(s.checksum));
    }

    const auto checksum = hdrCksum.result();

    if (checksum != _header.checksum) {
        // log error

        if (_defaultsLoader) {
            _defaultsLoader(DefaultsLoadReason::ChecksumError);
        }
    }

    return true;
}

SettingsHandler::SaveResult SettingsHandler::save(const bool skipChangeCheck)
{
    Hash::Fletcher16 hdrCksum;
    auto dataChanged = skipChangeCheck;

    for (auto& s : _settings) {
        const auto checksum = Hash::Fletcher16::calculate(s.ptr, s.size);
        hdrCksum.data(reinterpret_cast<const uint8_t*>(&checksum), sizeof(checksum));
        if (skipChangeCheck || s.checksum != checksum) {
            s.checksum = checksum;
            dataChanged = true;
            if (!_persistence.write(s.address, s.ptr, s.size)) {
                return SaveResult::Error;
            }
        }
    }

    if (dataChanged) {
        const auto checksum = hdrCksum.result();

        // Update the header
        _header.checksum = checksum;
        if (!_persistence.write(0, reinterpret_cast<uint8_t*>(&_header), sizeof(Header))) {
            return SaveResult::Error;
        }

        return SaveResult::Saved;
    }

    return SaveResult::NoChange;
}

void SettingsHandler::setDefaultsLoader(SettingsHandler::DefaultsLoader&& defaultsLoader)
{
    _defaultsLoader = std::move(defaultsLoader);
}