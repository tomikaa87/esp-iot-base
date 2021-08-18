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

#include "ApplicationConfig.h"

#include "utils/PlacementHelper.h"

#include <string>

#include <Arduino.h>
#include <StreamString.h>
#include <WiFiUdp.h>

class ISystemClock;

class Logger
{
public:
    static void setup(const ApplicationConfig& appConfig, const ISystemClock& systemClock);

    explicit Logger(std::string category);

    enum class Severity
    {
        Error,
        Warning,
        Info,
        Debug
    };

    template <typename... Params>
    void log(Severity severity, const char* fmt, Params... params) const
    {
        StreamString ss;

        if (!_inBlock) {
            ss.printf("[%c][%s]: ", severityIndicator(severity), _category.c_str());
        }

        if (sizeof...(params) == 0) {
            ss.print(fmt);
        } else {
            ss.printf(fmt, params...);
        }

        PlacementAccessor<Private> pa(_pContainer);

        if (!pa.empty() && pa->appConfig.logging.syslog.enabled) {
            pa->sendToSyslogServer(pa->appConfig.logging.syslog.hostName, ss.c_str());
        }

        // Print new line after sending the message to Syslog server
        if (!_inBlock) {
            ss.println();
        }

        Serial.print(ss);
    }

    template <typename... Params>
    void error(const char* fmt, Params... params) const
    {
        log(Severity::Error, fmt, params...);
    }

    template <typename... Params>
    void warning(const char* fmt, Params... params) const
    {
        log(Severity::Warning, fmt, params...);
    }

    template <typename... Params>
    void info(const char* fmt, Params... params) const
    {
        log(Severity::Info, fmt, params...);
    }

    template <typename... Params>
    void debug(const char* fmt, Params... params) const
    {
        log(Severity::Debug, fmt, params...);
    }

    struct Block
    {
        Block(bool& inBlock)
            : _inBlock(inBlock)
        {}

        ~Block()
        {
            _inBlock = false;
            Serial.println();
        }

    private:
        bool& _inBlock;
    };

    template <typename... Params>
    Block logBlock(Severity severity, const char* fmt, Params... params) const
    {
        _inBlock = true;

        StreamString ss;

        ss.printf("[%c][%s]: ", severityIndicator(severity), _category.c_str());

        if (sizeof...(params) == 0) {
            ss.print(fmt);
        } else {
            ss.printf(fmt, params...);
        }

        Serial.print(ss);

        PlacementAccessor<Private> pa(_pContainer);

        if (!pa.empty() && pa->appConfig.logging.syslog.enabled) {
            pa->sendToSyslogServer(pa->appConfig.logging.syslog.hostName, ss.c_str());
        }

        return Block{ _inBlock };
    }

    template <typename... Params>
    Block debugBlock(const char* fmt, Params... params) const
    {
        return logBlock(Severity::Debug, fmt, params...);
    }

private:
    const std::string _category;
    mutable bool _inBlock = false;

    struct Private
    {
        Private(const ApplicationConfig& appConfig, const ISystemClock& systemClock)
            : appConfig(appConfig)
            , systemClock(systemClock)
        {}

        WiFiUDP udp;
        const ApplicationConfig& appConfig;
        const ISystemClock& systemClock;

        void sendToSyslogServer(
            const char* hostName,
            const char* message,
            const char* procId = "-",
            const char* msgid = "-"
        );
    };

    static PlacementContainer<Private> _pContainer;

    static char severityIndicator(Severity severity);
};