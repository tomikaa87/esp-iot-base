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

#include "BaseConfig.h"
#include "CoreApplication.h"

#include <Arduino.h>
#include <ArduinoOTA.h>
#include <LittleFS.h>

CoreApplication* CoreApplication::instance = nullptr;

CoreApplication::CoreApplication(const ApplicationConfig& appConfig)
    : _i2cInitialized(
        [] {
            // Initialize the hardware before creating the application
            Drivers::I2C::init();
            return true;
        }()
    )
    , appConfig(appConfig)
    , ntpClient(_systemClock)
    , blynk(appConfig)
    , otaUpdater(appConfig, _systemClock)
#ifdef IOT_ENABLE_PERSISTENCE
    , settingsPersistence(
        appConfig.persistence.BaseAddress,
        appConfig.persistence.Size
    )
    , _settings(settingsPersistence)
#endif
{
    // Serial port
    setupSerialPort();

    Serial.printf_P(PSTR("Free heap before CA init: %u\n"), ESP.getFreeHeap());

    static WiFiClientSecure wcs;

    Serial.printf_P(PSTR("Free heap after WCS init: %u\n"), ESP.getFreeHeap());

    // Epoch timer
    setupEpochTimer();

    // WiFI
    setupWiFiStation();

    // File system
    setupFileSystem();

    // RTC
#ifdef IOT_SYSTEM_CLOCK_HW_RTC
    setupRtcDigitalTrimming();
#endif

    // Arduino OTA
    setupArduinoOta();

    // TODO de-init before SystemClock is destroyed
    Logger::setup(appConfig, _systemClock);

    Serial.printf_P(PSTR("Free heap after CA init: %u\n"), ESP.getFreeHeap());

    instance = this;
}

void CoreApplication::task()
{
    wifiWatchdog.task();

    _systemClock.task();
    ntpClient.task();

    otaUpdater.task();
#ifdef IOT_ENABLE_PERSISTENCE
    _settings.task();
#endif

    ArduinoOTA.handle();

    // Blynk library tends to freeze if there is no WiFi connection.
    // This is caused by an infinite loop in an connect() call in
    // Blynk.run() if the NTP server is unreachable.
    bool blynkTaskSucceeded = false;
    bool blynkTaskCanRun = false;
    if (wifiWatchdog.isConnected()) {
        // Check if system time is set before letting Blynk try to connect
        if (blynk.isConnected()) {
            blynkTaskCanRun = true;
        } else {
            // There is a check in connect() which checks if current time
            // is less than 100000
            if (time(nullptr) >= 100000) {
                blynkTaskCanRun = true;
            } else {
                // Time must be synchronized first. NtpClient and SystemClock
                // will do that automatically if possible
            }
        }

        if (blynkTaskCanRun) {
            blynkTaskSucceeded = blynk.task();
        }
    }

    // Slow loop
    if (lastSlowLoopUpdate == 0 || millis() - lastSlowLoopUpdate >= SlowLoopUpdateIntervalMs) {
        lastSlowLoopUpdate = millis();

        if (!updateChecked && wifiWatchdog.isConnected() && millis() - updateCheckTimer >= 5000) {
            updateChecked = true;
            otaUpdater.forceUpdate();
        }
    }

    // Blynk update loop
    if (lastBlynkUpdate == 0 || millis() - lastBlynkUpdate >= BlynkUpdateIntervalMs) {
        lastBlynkUpdate = millis();

        if (blynkTaskSucceeded && blynkUpdateHandler) {
            blynkUpdateHandler();
        }
    }
}

IBlynkHandler& CoreApplication::blynkHandler()
{
    return blynk;
}

#ifdef IOT_ENABLE_PERSISTENCE
ISettingsHandler& CoreApplication::settings()
{
    return _settings;
}
#endif

ISystemClock& CoreApplication::systemClock()
{
    return _systemClock;
}

void CoreApplication::setBlynkUpdateHandler(BlynkUpdateHandler&& handler)
{
    blynkUpdateHandler = std::move(handler);
}

void CoreApplication::setArduinoOtaEventHandler(ArduinoOtaEventHandler&& handler)
{
    arduinoOtaEventHandler = std::move(handler);
}

void IRAM_ATTR CoreApplication::epochTimerIsr()
{
    if (instance) {
        instance->_systemClock.timerIsr();
    }
}

void CoreApplication::setupArduinoOta()
{
    if (appConfig.otaUpdate.arduinoOtaPasswordHash) {
        ArduinoOTA.setPasswordHash(appConfig.otaUpdate.arduinoOtaPasswordHash);
    } else {
        log.warning("no password hash set for Arduino OTA");
    }

    ArduinoOTA.onStart([this] {
        auto type = "";
        if (ArduinoOTA.getCommand() == U_FLASH) {
            type = "flash";
        } else { // U_FS
            type = "file system";
        }

        log.info("ArduinoOTA: starting update, type=%s", type);

        if (arduinoOtaEventHandler) {
            arduinoOtaEventHandler(
                ArduinoOTA.getCommand() == U_FLASH
                    ? ArduinoOtaEvent::FlashUpdateStarted
                    : ArduinoOtaEvent::FileSystemUpdateStarted
            );
        }
    });

    ArduinoOTA.onEnd([this] {
        log.info("ArduinoOTA: ended");

        if (arduinoOtaEventHandler) {
            arduinoOtaEventHandler(ArduinoOtaEvent::Ended);
        }
    });

    ArduinoOTA.onError([this](const ota_error_t error) {
        auto errorStr = "unknown error";
        switch (error) {
            case OTA_AUTH_ERROR:
                errorStr = "authentication error";

                if (arduinoOtaEventHandler) {
                    arduinoOtaEventHandler(ArduinoOtaEvent::AuthenticationError);
                }
                break;

            case OTA_BEGIN_ERROR:
                errorStr = "begin failed";

                if (arduinoOtaEventHandler) {
                    arduinoOtaEventHandler(ArduinoOtaEvent::BeginError);
                }
                break;

            case OTA_CONNECT_ERROR:
                errorStr = "connect failed";

                if (arduinoOtaEventHandler) {
                    arduinoOtaEventHandler(ArduinoOtaEvent::ConnectError);
                }
                break;

            case OTA_END_ERROR:
                errorStr = "end failed";

                if (arduinoOtaEventHandler) {
                    arduinoOtaEventHandler(ArduinoOtaEvent::EndError);
                }
                break;

            case OTA_RECEIVE_ERROR:
                errorStr = "receive failed";

                if (arduinoOtaEventHandler) {
                    arduinoOtaEventHandler(ArduinoOtaEvent::ReceiveError);
                }
                break;
        }

        log.error("ArduinoOTA: update failed, error: %s", errorStr);
    });

    ArduinoOTA.begin();
}

void CoreApplication::setupSerialPort()
{
    Serial.begin(appConfig.serial.baudRate);
}

void CoreApplication::setupWiFiStation()
{
    log.info("Setting up WiFi station: SSID=%s", appConfig.wifi.ssid);

    WiFi.mode(WIFI_STA);
    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);
    WiFi.setOutputPower(20.5);
    if (appConfig.hostName != nullptr && strlen(appConfig.hostName) > 0) {
        wifi_station_set_hostname(appConfig.hostName);
    }
    WiFi.begin(appConfig.wifi.ssid, appConfig.wifi.password);
}

void CoreApplication::setupEpochTimer()
{
    log.info("Setting up Timer1 as epoch timer");

    timer1_isr_init();
    timer1_attachInterrupt(CoreApplication::epochTimerIsr);
    timer1_enable(TIM_DIV256, TIM_EDGE, TIM_LOOP);
    timer1_write(100);
}

void CoreApplication::setupFileSystem()
{
    log.info("Setting up file system");

    LittleFS.begin();
}

#ifdef IOT_SYSTEM_CLOCK_HW_RTC
void CoreApplication::setupRtcDigitalTrimming()
{
    if (!appConfig.rtc.enableDigitalTrimming) {
        return;
    }

    log.info("Setting up RTC digital trimming: value=%d, coarse=%s",
        appConfig.rtc.digitalTrimValue,
        appConfig.rtc.enableCoarseDigitalTrimming ? "yes" : "no"
    );

    Drivers::MCP7940N::setCoarseTrimmingEnabled(appConfig.rtc.enableCoarseDigitalTrimming);
    Drivers::MCP7940N::setDigitalTrimming(appConfig.rtc.digitalTrimValue);
}
#endif