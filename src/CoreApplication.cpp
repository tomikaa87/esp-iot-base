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

#include "CoreApplication.h"
#include "FirmwareVersion.h"

#include "drivers/SimpleI2C.h"

#include <ArduinoOTA.h>

CoreApplication::CoreApplication()
    : _ntpClient(_systemClock)
    , _otaUpdater(Config::OtaUpdate::FirmwareUpdateUrl, _systemClock)
{
    // TODO de-init before SystemClock is destroyed
    Logger::setup(_systemClock);

    Drivers::I2C::init();

    setupArduinoOta();
}

void CoreApplication::task()
{
    _systemClock.task();
    _ntpClient.task();
    _otaUpdater.task();
    _blynk.task();

    ArduinoOTA.handle();

    // Slow loop
    if (_lastSlowLoopUpdate == 0 || millis() - _lastSlowLoopUpdate >= SlowLoopUpdateIntervalMs) {
        _lastSlowLoopUpdate = millis();

        if (!_updateChecked && WiFi.isConnected() && millis() - _updateCheckTimer >= 5000) {
            _updateChecked = true;
            _otaUpdater.forceUpdate();
        }
    }

    // Blynk update loop
    if (_lastBlynkUpdate == 0 || millis() - _lastBlynkUpdate >= BlynkUpdateIntervalMs) {
        _lastBlynkUpdate = millis();

        if (_blynkUpdateHandler) {
            _blynkUpdateHandler();
        }
    }
}

void ICACHE_RAM_ATTR CoreApplication::epochTimerIsr()
{
    _systemClock.timerIsr();
}

IBlynkHandler& CoreApplication::blynkHandler()
{
    return _blynk;
}

ISettings& CoreApplication::settings()
{
    return _settings;
}

ISystemClock& CoreApplication::systemClock()
{
    return _systemClock;
}

void CoreApplication::setBlynkUpdateHandler(BlynkUpdateHandler&& handler)
{
    _blynkUpdateHandler = std::move(handler);
}

void CoreApplication::setArduinoOtaEventHandler(ArduinoOtaEventHandler&& handler)
{
    _arduinoOtaEventHandler = std::move(handler);
}

const std::string& CoreApplication::firmwareVersion()
{
    static const std::string v{ FW_VER_STR };
    return v;
}

const std::string& CoreApplication::applicationVersion()
{
    static const std::string v{ FW_CORE_APP_VER_STR };
    return v;
}

void CoreApplication::setupArduinoOta()
{
    ArduinoOTA.onStart([this] {
        auto type = "";
        if (ArduinoOTA.getCommand() == U_FLASH) {
            type = "flash";
        } else { // U_FS
            type = "file system";
        }

        _log.info("ArduinoOTA: starting update, type=%s", type);

        if (_arduinoOtaEventHandler) {
            _arduinoOtaEventHandler(
                ArduinoOTA.getCommand() == U_FLASH
                    ? ArduinoOtaEvent::FlashUpdateStarted
                    : ArduinoOtaEvent::FileSystemUpdateStarted
            );
        }
    });

    ArduinoOTA.onEnd([this] {
        _log.info("ArduinoOTA: ended");

        if (_arduinoOtaEventHandler) {
            _arduinoOtaEventHandler(ArduinoOtaEvent::Ended);
        }
    });

    ArduinoOTA.onError([this](const ota_error_t error) {
        auto errorStr = "unknown error";
        switch (error) {
            case OTA_AUTH_ERROR:
                errorStr = "authentication error";

                if (_arduinoOtaEventHandler) {
                    _arduinoOtaEventHandler(ArduinoOtaEvent::AuthenticationError);
                }
                break;

            case OTA_BEGIN_ERROR:
                errorStr = "begin failed";

                if (_arduinoOtaEventHandler) {
                    _arduinoOtaEventHandler(ArduinoOtaEvent::BeginError);
                }
                break;

            case OTA_CONNECT_ERROR:
                errorStr = "connect failed";

                if (_arduinoOtaEventHandler) {
                    _arduinoOtaEventHandler(ArduinoOtaEvent::ConnectError);
                }
                break;

            case OTA_END_ERROR:
                errorStr = "end failed";

                if (_arduinoOtaEventHandler) {
                    _arduinoOtaEventHandler(ArduinoOtaEvent::EndError);
                }
                break;

            case OTA_RECEIVE_ERROR:
                errorStr = "receive failed";

                if (_arduinoOtaEventHandler) {
                    _arduinoOtaEventHandler(ArduinoOtaEvent::ReceiveError);
                }
                break;
        }

        _log.error("ArduinoOTA: update failed, error: %s", errorStr);
    });

    ArduinoOTA.begin();
}