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
#include "IBlynkHandler.h"
#include "ISettingsHandler.h"
#include "ISystemClock.h"

#include "Logger.h"
#include "SettingsHandler.h"
#include "SystemClock.h"

#if defined(IOT_ENABLE_PERSISTENCE) && defined (IOT_PERSISTENCE_USE_EERAM)
#include "EeramPersistence.h"
#endif

#if defined(IOT_ENABLE_PERSISTENCE) && defined (IOT_PERSISTENCE_USE_EEPROM)
#include "EepromPersistence.h"
#endif

#ifdef IOT_SYSTEM_CLOCK_HW_RTC
#include "drivers/MCP7940N.h"
#endif

#include "drivers/SimpleI2C.h"

#include "network/BlynkHandler.h"
#include "network/NtpClient.h"
#include "network/OtaUpdater.h"
#include "network/WiFiWatchdog.h"

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

    explicit CoreApplication(const ApplicationConfig& appConfig);

    void task();

    IBlynkHandler& blynkHandler();
#ifdef IOT_ENABLE_PERSISTENCE
    ISettingsHandler& settings();
#endif

    ISystemClock& systemClock();

    void setBlynkUpdateHandler(BlynkUpdateHandler&& handler);
    void setArduinoOtaEventHandler(ArduinoOtaEventHandler&& handler);

private:
    Logger log{ "CoreApplication" };
    const bool _i2cInitialized;
    const ApplicationConfig& appConfig;
    SystemClock _systemClock;
    NtpClient ntpClient;
    BlynkHandler blynk;
    OtaUpdater otaUpdater;
#ifdef IOT_PERSISTENCE_USE_EERAM
    EeramPersistence settingsPersistence;
#endif
#ifdef IOT_PERSISTENCE_USE_EEPROM
    EepromPersistence settingsPersistence;
#endif
#ifdef IOT_ENABLE_PERSISTENCE
    SettingsHandler _settings;
#endif

    WiFiWatchdog wifiWatchdog;

    bool updateChecked = false;
    uint32_t updateCheckTimer = 0;

    static constexpr auto SlowLoopUpdateIntervalMs = 500;
    uint32_t lastSlowLoopUpdate = 0;

    static constexpr auto BlynkUpdateIntervalMs = 1000;
    uint32_t lastBlynkUpdate = 0;
    BlynkUpdateHandler blynkUpdateHandler;

    ArduinoOtaEventHandler arduinoOtaEventHandler;

    static CoreApplication* instance;
    static void epochTimerIsr();
    void setupArduinoOta();

    void setupSerialPort();
    void setupWiFiStation();
    void setupEpochTimer();
    void setupFileSystem();
    void setupRtcDigitalTrimming();
};
