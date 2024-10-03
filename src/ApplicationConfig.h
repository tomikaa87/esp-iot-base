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

#include "BaseConfig.h"
#include "LogSeverity.h"
#include "VersionNumber.h"

#include <cstdint>

#include <IPAddress.h>

struct ApplicationConfig
{
    struct Logging
    {
        Log::Severity maximumLevel{ Log::Severity::Info };

        struct Syslog
        {
            bool enabled = false;
            const char* hostName = "esp-iot";
            const char* serverHostName = "";
            uint16_t serverPort = 514;
        };

        Syslog syslog;
    };

    struct Mqtt
    {
        bool enabled = false;
        const char* id = "esp-iot";
        IPAddress brokerIp;
        uint16_t brokerPort = 1883;
        const char* user = nullptr;
        const char* password = nullptr;
    };

    struct OtaUpdate
    {
        uint16_t arduinoOtaPort = 8266;
#ifdef IOT_ENABLE_HTTP_OTA_UPDATE
        uint32_t updateCheckIntervalMs = 5000;
        const char* updateUrl = "";
#endif
        const char* arduinoOtaPasswordHash = nullptr;
    };

    struct Persistence
    {
        uint32_t BaseAddress = 0;

#if defined(IOT_PERSISTENCE_USE_EERAM)
        uint32_t Size = Config::Persistence::EeramSize;
#else
        uint32_t Size = 128;
#endif
    };

    struct Rtc
    {
        bool enableDigitalTrimming = false;
        bool enableCoarseDigitalTrimming = false;
        int8_t digitalTrimValue = 0;
    };

    struct Serial
    {
        uint32_t baudRate = 74880;
    };

    struct WebServer
    {
        uint16_t port = 80;
    };

    struct WiFi
    {
        const char* ssid = "";
        const char* password = "";
    };

    struct Ntp
    {
        const char* server = "pool.ntp.org";
    };

    const VersionNumber applicationVersion{ 1, 9, 1 };
    VersionNumber firmwareVersion;

    Logging logging;
    Mqtt mqtt;
    OtaUpdate otaUpdate;
    Persistence persistence;
    Rtc rtc;
    Serial serial;
    WebServer webServer;
    WiFi wifi;
    Ntp ntp;

    const char* hostName = "";
};