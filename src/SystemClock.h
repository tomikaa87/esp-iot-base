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
    Created on 2016-12-30
*/

#pragma once

#include "ISystemClock.h"
#include "Logger.h"

#include <ctime>

class SystemClock : public ISystemClock
{
public:
    static constexpr auto NtpSyncIntervalSec = 1800;
    static constexpr auto RtcSyncIntervalSec = 60;

    SystemClock();

    void task();
    void timerIsr();

    std::time_t localTime() const override;
    std::time_t utcTime() const override;

    void setUtcTime(std::time_t t);

private:
    Logger _log{ "SystemClock" };
    std::time_t _epoch = 0;
    std::time_t _lastRtcSync = 0;
    bool _ntpSyncing = false;
    bool _rtcSynced = false;
    int _isrCounter = 0;
    int _localTimeOffsetMinutes = 60;
    int _localTimeDstOffsetMinutes = 60;

#ifdef IOT_SYSTEM_CLOCK_HW_RTC
    void setupHwRtc();
    void updateFromRtc();
    void updateRtc();
#endif

    static bool isDst(std::time_t t);
};