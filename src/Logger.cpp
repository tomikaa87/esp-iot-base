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

#include "Config.h"
#include "FirmwareVersion.h"
#include "Logger.h"
#include "SystemClock.h"

#include <Arduino.h>

#include <ctime>

Logger::Private* Logger::_p = nullptr;

void Logger::setup(const SystemClock& systemClock)
{
    if (Logger::_p) {
        delete Logger::_p;
    }

    Logger::_p = new Logger::Private(systemClock);
}

Logger::Logger(std::string category)
    : _category(std::move(category))
{}

char Logger::severityIndicator(const Severity severity)
{
    switch (severity) {
        case Severity::Error:
            return 'E';
            break;
        case Severity::Warning:
            return 'W';
            break;
        case Severity::Info:
            return 'I';
            break;
        case Severity::Debug:
            return 'D';
            break;
    }

    return '?';
}

void Logger::Private::sendToSyslogServer(
    const char* hostName,
    const char* message,
    const char* procId,
    const char* msgid
) {
    if (!udp.beginPacket(Config::Logging::SyslogServerHost, Config::Logging::SyslogServerPort)) {
#if 0
        Serial.println("[W][Logger] can't send Syslog payload, failed to begin UDP packet");
#endif
        return;
    }

    const auto t = systemClock.utcTime();
    const auto tm = gmtime(&t);

    StreamString payload;
    payload.printf("<191>1 %04u-%02u-%02uT%02u:%02u:%02uZ %s v%d.%d.%d %s %s - %s",
        tm->tm_year + 1900,
        tm->tm_mon + 1,
        tm->tm_mday,
        tm->tm_hour,
        tm->tm_min,
        tm->tm_sec,
        hostName,
        FW_VER_MAJOR,
        FW_VER_MINOR,
        FW_VER_PATCH,
        procId,
        msgid,
        message
    );

#if 0
    Serial.print("Sending Syslog payload: ");
    Serial.print(payload);
#endif

    udp.write(payload.c_str());
    udp.endPacket();
}