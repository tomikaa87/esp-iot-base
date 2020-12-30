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

#include "Logger.h"

#include <Arduino.h>
#include <WiFiUdp.h>

#include <ctime>
#include <functional>
#include <memory>

class SystemClock;

class NtpClient
{
public:
    // TODO move these to ApplicationConfig
    static constexpr auto Server = "europe.pool.ntp.org";
#ifdef IOT_SYSTEM_CLOCK_HW_RTC
    static constexpr auto UpdateInterval = 24 * 60 * 60;
#else
    // Software RTC needs more frequent updates to be accurate
    static constexpr auto UpdateInterval = 10 * 60;
#endif
    static constexpr auto RetryIntervalSeconds = 30;
    static constexpr auto NtpPacketSize = 48;
    static constexpr auto SeventyYears = 2208988800ul;
    static constexpr auto NtpPort = 123;
    static constexpr auto NtpDefaultLocalPort = 1337;

    NtpClient(SystemClock& clock);

    void task();

    using UpdatedHandler = std::function<void(std::time_t)>;
    void setUpdatedCallback(UpdatedHandler&& handler);

private:
    SystemClock& _systemClock;
    Logger _log{ "NtpClient" };
    UpdatedHandler _updatedHandler;
    std::unique_ptr<WiFiUDP> _socket = nullptr;
    std::time_t _lastUpdate = 0;
    uint32_t _sendTimestamp = 0;

    enum class State
    {
        Idle,
        SendPacket,
        WaitResponse
    };
    State _state = State::Idle;

    void sendPacket();
};