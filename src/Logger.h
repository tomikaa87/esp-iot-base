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
#include "LogSeverity.h"

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

    template <typename... Params>
    void log(Log::Severity severity, const char* fmt, Params... params) const
    {
        if (_p && static_cast<int>(severity) > static_cast<int>(_p->appConfig.logging.maximumLevel)) {
            return;
        }

        StreamString ss;

        if (!_inBlock) {
            ss.printf("[%c][%s]: ", severityIndicator(severity), _category.c_str());
        }

        if (sizeof...(params) == 0) {
            ss.print(fmt);
        } else {
            ss.printf(fmt, params...);
        }

        if (_p && _p->appConfig.logging.syslog.enabled) {
            _p->sendToSyslogServer(_p->appConfig.logging.syslog.hostName, ss.c_str());
        }

        // Print new line after sending the message to Syslog server
        if (!_inBlock) {
            ss.println();
        }

        Serial.print(ss);
    }

    template <typename... Params>
    void log_P(Log::Severity severity, PGM_P fmt, Params... params) const
    {
        if (_p && static_cast<int>(severity) > static_cast<int>(_p->appConfig.logging.maximumLevel)) {
            return;
        }

        StreamString ss;

        if (!_inBlock) {
            ss.printf_P(PSTR("[%c][%s]: "), severityIndicator(severity), _category.c_str());
        }

        if (sizeof...(params) == 0) {
            ss.printf_P(PSTR("%s"), fmt);
        } else {
            ss.printf_P(fmt, params...);
        }

        if (_p && _p->appConfig.logging.syslog.enabled) {
            _p->sendToSyslogServer(_p->appConfig.logging.syslog.hostName, ss.c_str());
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
        log(Log::Severity::Error, fmt, params...);
    }

    template <typename... Params>
    void warning(const char* fmt, Params... params) const
    {
        log(Log::Severity::Warning, fmt, params...);
    }

    template <typename... Params>
    void info(const char* fmt, Params... params) const
    {
        log(Log::Severity::Info, fmt, params...);
    }

    template <typename... Params>
    void debug(const char* fmt, Params... params) const
    {
        log(Log::Severity::Debug, fmt, params...);
    }

    template <typename... Params>
    void error_P(PGM_P fmt, Params... params) const
    {
        log_P(Log::Severity::Error, fmt, params...);
    }

    template <typename... Params>
    void warning_P(PGM_P fmt, Params... params) const
    {
        log_P(Log::Severity::Warning, fmt, params...);
    }

    template <typename... Params>
    void info_P(PGM_P fmt, Params... params) const
    {
        log_P(Log::Severity::Info, fmt, params...);
    }

    template <typename... Params>
    void debug_P(PGM_P fmt, Params... params) const
    {
        log_P(Log::Severity::Debug, fmt, params...);
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
    Block logBlock(Log::Severity severity, const char* fmt, Params... params) const
    {
        if (_p && static_cast<int>(severity) > static_cast<int>(_p->appConfig.logging.maximumLevel)) {
            return Block{ _inBlock };
        }

        _inBlock = true;

        StreamString ss;

        ss.printf("[%c][%s]: ", severityIndicator(severity), _category.c_str());

        if (sizeof...(params) == 0) {
            ss.print(fmt);
        } else {
            ss.printf(fmt, params...);
        }

        Serial.print(ss);

        if (_p && _p->appConfig.logging.syslog.enabled) {
            _p->sendToSyslogServer(_p->appConfig.logging.syslog.hostName, ss.c_str());
        }

        return Block{ _inBlock };
    }

    template <typename... Params>
    Block logBlock_P(Log::Severity severity, PGM_P fmt, Params... params) const
    {
        if (_p && static_cast<int>(severity) > static_cast<int>(_p->appConfig.logging.maximumLevel)) {
            return Block{ _inBlock };
        }

        _inBlock = true;

        StreamString ss;

        ss.printf_P(PSTR("[%c][%s]: "), severityIndicator(severity), _category.c_str());

        if (sizeof...(params) == 0) {
            ss.printf_P(PSTR("%s"), fmt);
        } else {
            ss.printf_P(fmt, params...);
        }

        Serial.print(ss);

        if (_p && _p->appConfig.logging.syslog.enabled) {
            _p->sendToSyslogServer(_p->appConfig.logging.syslog.hostName, ss.c_str());
        }

        return Block{ _inBlock };
    }

    template <typename... Params>
    Block debugBlock(const char* fmt, Params... params) const
    {
        return logBlock(Log::Severity::Debug, fmt, params...);
    }

    template <typename... Params>
    Block debugBlock_P(PGM_P fmt, Params... params) const
    {
        return logBlock_P(Log::Severity::Debug, fmt, params...);
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

    static Private* _p;

    static char severityIndicator(Log::Severity severity);
};