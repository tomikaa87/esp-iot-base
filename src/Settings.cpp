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
    _log.info("initializing");

    // Disable ASE by default to avoid unnecessary wearing when settings are not changed
    Drivers::EERAM::StatusReg sr;
    sr.value = 0;
    Drivers::EERAM::setStatus(sr);
}

bool Settings::save(AbstractSettingsData& data)
{
    _log.debug(
        "saving settings to EERAM: address=%xh, length=%u",
        Config::EeramAddresses::SettingsBase,
        data.size
    );

    data.checksum = calculateDataChecksum(data);

    bool ok = Drivers::EERAM::write(
        Config::EeramAddresses::SettingsBase,
        data.ptr,
        data.size
    );

    if (ok) {
        if (!Drivers::EERAM::getStatus().ase) {
            _log.debug("setting ASE bit");
            Drivers::EERAM::setAseEnabled(true);
        }
    } else {
        _log.error("failed to write settings to EERAM");
    }

    return ok;
}

bool Settings::load(AbstractSettingsData& data)
{
    _log.info(
        "loading settings from EERAM: address=%xh, length=%u",
        Config::EeramAddresses::SettingsBase,
        data.size
    );

    const auto ok = Drivers::EERAM::read(
        Config::EeramAddresses::SettingsBase,
        data.ptr,
        data.size
    );

    if (!ok) {
        _log.error("failed to read settings from EERAM");

        if (_loadDefaultsRequestedHandler) {
            _loadDefaultsRequestedHandler(LoadDefaultsReason::ReadFailure);
        }

        return false;
    }

    if (calculateDataChecksum(data) != data.checksum)
    {
        _log.warning("checksum error");

        if (_loadDefaultsRequestedHandler) {
            _loadDefaultsRequestedHandler(LoadDefaultsReason::ChecksumError);
        }

        return false;
    }

    return true;
}

void Settings::setLoadDefaultsRequestedHandler(LoadDefaultsRequestedHandler&& handler)
{
    _loadDefaultsRequestedHandler = std::move(handler);
}

uint32_t Settings::calculateDataChecksum(AbstractSettingsData& data)
{
    return crc32(
        data.ptr + sizeof(AbstractSettingsData::checksum),
        data.size - sizeof(AbstractSettingsData::checksum)
    );
}
