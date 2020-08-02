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

#include "Settings.h"

#include "drivers/EERAM.h"

#include <coredecls.h> // crc32()

Settings::Settings()
{
    static_assert(
        sizeof(Data) <= Config::EeramAddresses::SchedulerDataBase - Config::EeramAddresses::SettingsBase,
        "Settings data doesn't fit in the allocated space"
    );

    _log.info("initializing");

    // Disable ASE by default to avoid unnecessary wearing when settings are not changed
    Drivers::EERAM::StatusReg sr;
    sr.value = 0;
    Drivers::EERAM::setStatus(sr);
}

void Settings::loadDefaults()
{
    _log.info("loading default settings");

    data = {};
}

void Settings::load()
{
    _log.info(
        "loading settings from EERAM: address=%xh, length=%u",
        Config::EeramAddresses::SettingsBase,
        sizeof data
    );

    const auto ok = Drivers::EERAM::read(
        Config::EeramAddresses::SettingsBase,
        reinterpret_cast<uint8_t*>(&data),
        sizeof data
    );

    if (!ok) {
        _log.error("failed to read settings from EERAM");
    }

    if (calculateDataChecksum() != data.checksum)
    {
        _log.warning("checksum error");

        loadDefaults();
        save();
    }

    // TODO print settings
}

void Settings::save()
{
    _log.debug(
        "saving settings to EERAM: address=%xh, length=%u",
        Config::EeramAddresses::SettingsBase,
        sizeof data
    );

    data.checksum = calculateDataChecksum();

    bool ok = Drivers::EERAM::write(
        Config::EeramAddresses::SettingsBase,
        reinterpret_cast<const uint8_t*>(&data),
        sizeof data
    );

    if (ok) {
        if (!Drivers::EERAM::getStatus().ase) {
            _log.debug("setting ASE bit");
            Drivers::EERAM::setAseEnabled(true);
        }
    } else {
        _log.error("failed to write settings to EERAM");
    }
}

uint32_t Settings::calculateDataChecksum() const
{
    return crc32(
        reinterpret_cast<const uint8_t*>(&data) + sizeof data.checksum,
        sizeof data - sizeof data.checksum
    );
}
