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
    Created on 2020-12-31
*/

#pragma once

#include "Logger.h"

#include <cstdint>

class WiFiWatchdog
{
    static constexpr auto TaskIntervalMs = 100;
    static constexpr auto ReconnectTimeoutMs = 10000;

public:
    void task();

    bool isConnected() const;

private:
    Logger _log{ "WiFiWatchdog" };

    enum class State
    {
        WaitingForInitialConnect,
        Connected,
        WaitingForReconnect
    };

    State _state = State::WaitingForInitialConnect;
    uint32_t _taskTimer = 0;
    uint32_t _reconnectTimer = 0;
};