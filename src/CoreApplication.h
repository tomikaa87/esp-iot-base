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

#include "IBlynkHandler.h"
#include "ISystemClock.h"
#include "Logger.h"
#include "Settings.h"
#include "SystemClock.h"

#include "network/BlynkHandler.h"
#include "network/NtpClient.h"
#include "network/OtaUpdater.h"

#include <functional>

class CoreApplication
{
public:
    using BlynkUpdateHandler = std::function<void ()>;

    enum class ArduinoOtaEvent {
        FlashUpdateStarted,
        FileSystemUpdateStarted,
        AuthenticationError,
        BeginError,
        ConnectError,
        EndError,
        ReceiveError,
        Ended
    };

    using ArduinoOtaEventHandler = std::function<void (ArduinoOtaEvent)>;

    CoreApplication();

    void task();
    void epochTimerIsr();

    IBlynkHandler& blynkHandler();
    ISettings& settings();
    ISystemClock& systemClock();

    void setBlynkUpdateHandler(BlynkUpdateHandler&& handler);
    void setArduinoOtaEventHandler(ArduinoOtaEventHandler&& handler);

    static const std::string& firmwareVersion();
    static const std::string& applicationVersion();

private:
    Logger _log{ "CoreApplication" };
    SystemClock _systemClock;
    NtpClient _ntpClient;
    BlynkHandler _blynk;
    Settings _settings;
    OtaUpdater _otaUpdater;

    bool _updateChecked = false;
    uint32_t _updateCheckTimer = 0;

    static constexpr auto SlowLoopUpdateIntervalMs = 500;
    uint32_t _lastSlowLoopUpdate = 0;

    static constexpr auto BlynkUpdateIntervalMs = 1000;
    uint32_t _lastBlynkUpdate = 0;
    BlynkUpdateHandler _blynkUpdateHandler;

    ArduinoOtaEventHandler _arduinoOtaEventHandler;

    void setupArduinoOta();
};

