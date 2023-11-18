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

#include "network/MQTT/MqttClient.h"
#include "network/NtpClient.h"
#include "network/OtaUpdater.h"
#include "network/WiFiWatchdog.h"

#include <Arduino.h>
#include <ArduinoOTA.h>
#include <LittleFS.h>

struct CoreApplication::Private
{
    Private(
        const ApplicationConfig& appConfig
    )
        : appConfig(appConfig)
        , ntpClient(systemClock)
#ifdef IOT_ENABLE_HTTP_OTA_UPDATE
        , otaUpdater(appConfig, systemClock)
#endif
#ifdef IOT_ENABLE_PERSISTENCE
        , settingsPersistence(
            appConfig.persistence.BaseAddress,
            appConfig.persistence.Size
        )
        , settings(settingsPersistence)
#endif
        , mqttClient(appConfig)
    {
        instance = this;

        // Serial port
        setupSerialPort();

        // Epoch timer
        setupEpochTimer();

        // WiFI
        setupWiFiStation();

        // File system
        setupFileSystem();

        // I2C bus
        Drivers::I2C::init();

        // RTC
#ifdef IOT_SYSTEM_CLOCK_HW_RTC
        setupRtcDigitalTrimming();
#endif

        // Arduino OTA
        setupArduinoOta();

        // TODO de-init before SystemClock is destroyed
        Logger::setup(appConfig, systemClock);
    }

    ~Private()
    {
        instance = nullptr;
    }

    Logger log{ "CoreApplication" };
    const ApplicationConfig& appConfig;
    SystemClock systemClock;
    NtpClient ntpClient;
#ifdef IOT_ENABLE_HTTP_OTA_UPDATE
    OtaUpdater otaUpdater;
    bool updateChecked = false;
    uint32_t updateCheckTimer = 0;
#endif
#ifdef IOT_PERSISTENCE_USE_EERAM
    EeramPersistence settingsPersistence;
#endif
#ifdef IOT_PERSISTENCE_USE_EEPROM
    EepromPersistence settingsPersistence;
#endif
#ifdef IOT_ENABLE_PERSISTENCE
    SettingsHandler settings;
#endif

    WiFiWatchdog wifiWatchdog;

    static constexpr auto SlowLoopUpdateIntervalMs = 500;
    uint32_t lastSlowLoopUpdate = 0;

    ArduinoOtaEventHandler arduinoOtaEventHandler;

#ifdef IOT_ENABLE_MQTT
    static constexpr auto MqttUpdateIntervalMs = 1000;
    uint32_t lastMqttUpdate = 0;
    MqttUpdateHandler mqttUpdateHandler;
#endif

    MqttClient mqttClient;

    static Private* instance;
    static void epochTimerIsr();
    void setupArduinoOta();

    void setupSerialPort();
    void setupWiFiStation();
    void setupEpochTimer();
    void setupFileSystem();
    void setupRtcDigitalTrimming();
};

CoreApplication::Private* CoreApplication::Private::instance = nullptr;

CoreApplication::CoreApplication(const ApplicationConfig& appConfig)
    : _p(new Private(appConfig))
{
}

CoreApplication::~CoreApplication() = default;

void CoreApplication::task()
{
    _p->wifiWatchdog.task();

    _p->systemClock.task();
    _p->ntpClient.task();

#ifdef IOT_ENABLE_HTTP_OTA_UPDATE
    _p->otaUpdater.task();
#endif
#ifdef IOT_ENABLE_PERSISTENCE
    _p->settings.task();
#endif

    _p->mqttClient.task();

    const auto currentTime = millis();

    ArduinoOTA.handle();

    // Slow loop
    if (_p->lastSlowLoopUpdate == 0 || currentTime - _p->lastSlowLoopUpdate >= Private::SlowLoopUpdateIntervalMs) {
        _p->lastSlowLoopUpdate = currentTime;

#ifdef IOT_ENABLE_HTTP_OTA_UPDATE
        if (!_p->updateChecked && _p->wifiWatchdog.isConnected() && currentTime - _p->updateCheckTimer >= 5000) {
            _p->updateChecked = true;
            _p->otaUpdater.forceUpdate();
        }
#endif
    }

#ifdef IOT_ENABLE_MQTT
    // MQTT update loop
    if (_p->appConfig.mqtt.enabled && (_p->lastMqttUpdate == 0 || currentTime - _p->lastMqttUpdate >= Private::MqttUpdateIntervalMs)) {
        _p->lastMqttUpdate = currentTime;

        if (_p->mqttUpdateHandler) {
            _p->mqttUpdateHandler();
        }
    }
#endif
}

#ifdef IOT_ENABLE_PERSISTENCE
ISettingsHandler& CoreApplication::settings()
{
    return _p->settings;
}
#endif

ISystemClock& CoreApplication::systemClock()
{
    return _p->systemClock;
}

#ifdef IOT_ENABLE_MQTT
MqttClient& CoreApplication::mqttClient()
{
    return _p->mqttClient;
}
#endif

void CoreApplication::setArduinoOtaEventHandler(ArduinoOtaEventHandler&& handler)
{
    _p->arduinoOtaEventHandler = std::move(handler);
}

#ifdef IOT_ENABLE_MQTT
void CoreApplication::setMqttUpdateHandler(MqttUpdateHandler&& handler)
{
    _p->mqttUpdateHandler = std::move(handler);
}
#endif

void IRAM_ATTR CoreApplication::Private::epochTimerIsr()
{
    if (instance) {
        instance->systemClock.timerIsr();
    }
}

void CoreApplication::Private::setupArduinoOta()
{
    if (appConfig.otaUpdate.arduinoOtaPasswordHash) {
        ArduinoOTA.setPasswordHash(appConfig.otaUpdate.arduinoOtaPasswordHash);
    } else {
        log.warning_P(PSTR("no password hash set for Arduino OTA"));
    }

    ArduinoOTA.onStart([this] {
        auto type = "";
        if (ArduinoOTA.getCommand() == U_FLASH) {
            type = "flash";
        } else { // U_FS
            type = "file system";
        }

        log.info_P(PSTR("ArduinoOTA: starting update, type=%s"), type);

        if (arduinoOtaEventHandler) {
            arduinoOtaEventHandler(
                ArduinoOTA.getCommand() == U_FLASH
                    ? ArduinoOtaEvent::FlashUpdateStarted
                    : ArduinoOtaEvent::FileSystemUpdateStarted
            );
        }
    });

    ArduinoOTA.onEnd([this] {
        log.info_P(PSTR("ArduinoOTA: ended"));

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

        log.error_P(PSTR("ArduinoOTA: update failed, error: %s"), errorStr);
    });

    ArduinoOTA.begin();
}

void CoreApplication::Private::setupSerialPort()
{
    Serial.begin(appConfig.serial.baudRate);
}

void CoreApplication::Private::setupWiFiStation()
{
    log.info_P(PSTR("Setting up WiFi station: SSID=%s"), appConfig.wifi.ssid);

    WiFi.mode(WIFI_STA);
    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);
    WiFi.setOutputPower(20.5);
    if (appConfig.hostName != nullptr && strlen(appConfig.hostName) > 0) {
        wifi_station_set_hostname(appConfig.hostName);
    }
    WiFi.begin(appConfig.wifi.ssid, appConfig.wifi.password);
}

void CoreApplication::Private::setupEpochTimer()
{
    log.info_P(PSTR("Setting up Timer1 as epoch timer"));

    timer1_isr_init();
    timer1_attachInterrupt(CoreApplication::Private::epochTimerIsr);
    timer1_enable(TIM_DIV256, TIM_EDGE, TIM_LOOP);
    timer1_write(100);
}

void CoreApplication::Private::setupFileSystem()
{
    log.info_P(PSTR("Setting up file system"));

    LittleFS.begin();
}

#ifdef IOT_SYSTEM_CLOCK_HW_RTC
void CoreApplication::Private::setupRtcDigitalTrimming()
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