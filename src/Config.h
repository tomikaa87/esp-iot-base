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

/*

PrivateConfig.h must contain the following:

#pragma once

namespace Config
{
    namespace Blynk
    {
        static constexpr auto AppToken = "<your Blynk application token>";
        static constexpr auto ServerHostName = "<your blynk server's host name or blynk-cloud.cc for hosted>";
        static constexpr auto ServerPort = 8080;
    }

    namespace WiFi
    {
        static constexpr auto SSID = "<SSID of you access point>";
        static constexpr auto Password = "<password of your access point>";
    }

    namespace OtaUpdate
    {
        static constexpr auto FirmwareUpdateUrl = "<URL to your update server>";
        static constexpr auto ArduinoOtaPort = 8266;
    }

    namespace Logging
    {
        static constexpr auto SyslogServerHost = "<host name of your Syslog server>";
        static constexpr auto SyslogServerPort = <port of your Syslog server, 514 by default>;
        static constexpr auto SyslogHostName = "<host name for the current device, e.g. espiotapp1>";
    }
}

*/

#include "PrivateConfig.h"

#include <Arduino.h>

namespace Config
{
    namespace Blynk
    {
        static constexpr auto SettingsSaveTimeoutMs = 2000;
    }

    namespace Network
    {
        static constexpr auto WebServerPort = 80;
        static constexpr auto SyslogEnabled = true;
    }

    namespace EeramAddresses
    {
        static constexpr auto EeramSize = 16384u;

        static constexpr auto SettingsBase = 0u;

        static constexpr auto SchedulerDataHeaderBase = 0x80u;
        static constexpr auto SchedulerDataBase = 0x90u;

        static constexpr auto SchedulerDataHeaderSize = SchedulerDataBase - SchedulerDataHeaderBase;
        static constexpr auto SchedulerDataSize = EeramSize - SchedulerDataBase;
    }
}
