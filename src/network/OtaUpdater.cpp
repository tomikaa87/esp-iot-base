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

#ifdef IOT_ENABLE_HTTP_OTA_UPDATE

#include "OtaUpdater.h"
#include "SystemClock.h"

#include <asyncHTTPrequest.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266httpUpdate.h>

OtaUpdater::OtaUpdater(const ApplicationConfig& appConfig, const SystemClock& systemClock)
    : _appConfig(appConfig)
    , _systemClock(systemClock)
{
    _log.info_P(PSTR("initializing"));
}

OtaUpdater::~OtaUpdater() = default;

void OtaUpdater::task()
{
    switch (_state) {
        case State::Idle:
            if (
#ifdef IOT_ENABLE_PERIODIC_HTTP_UPDATE_CHECK
                (/*_lastCheckedForUpdates > 0 && */_systemClock.utcTime() - _lastCheckedForUpdates >= _updateCheckIntervalSeconds) ||
#endif
                _forceUpdateCheck
            ) {
                _log.info_P(PSTR("checking for updates"));

                _state = State::GetAvailableVersion;
                _forceUpdateCheck = false;

                _lastCheckedForUpdates = _systemClock.utcTime();

                if (!WiFi.isConnected()) {
                    _log.warning_P(PSTR("cannot check for updates, WiFi is not connected"));
                    _state = State::Idle;
                    break;
                }

                if (_updateStateChangedHandler) {
                    _updateStateChangedHandler(UpdateState::CheckingForUpdate);
                }

                _requestStartTimestamp = _systemClock.utcTime();

                createVersionInfoRequest();
                if (!_httpClient->send()) {
                    _log.error_P(PSTR("failed to send HTTP request"));

                    _httpClient.reset();
                    _state = State::Idle;

                    if (_updateStateChangedHandler) {
                        _updateStateChangedHandler(UpdateState::CheckFailed);
                    }
                }
            }
            break;

        case State::GetAvailableVersion:
            if (_httpClient->readyState() != 4) {
                if (_systemClock.utcTime() - _requestStartTimestamp >= 10) {
                    _log.warning_P(PSTR("version check request timed out"));

                    _state = State::Idle;
                    _httpClient.reset();

                    if (_updateStateChangedHandler) {
                        _updateStateChangedHandler(UpdateState::CheckFailed);
                    }
                }
                break;
            }
            if (_httpClient->responseHTTPcode() != 200) {
                _log.warning_P(PSTR("HTTP request failed, response code: %d"), _httpClient->responseHTTPcode());
                _state = State::Idle;
                _httpClient.reset();
                break;
            }
            switch (checkVersion(_httpClient->responseText().c_str())) {
                case VersionCheckResult::NoUpdateNeeded:
                    _log.info_P(PSTR("firmware is up-to-date"));

                    _state = State::Idle;
                    _httpClient.reset();

                    if (_updateStateChangedHandler) {
                        _updateStateChangedHandler(UpdateState::NoUpdateNeeded);
                    }
                    break;

                case VersionCheckResult::CannotCheckVersion: {
                    _log.warning_P(PSTR("cannot check update version"));

                    _state = State::Idle;
                    _httpClient.reset();

                    if (_updateStateChangedHandler) {
                        _updateStateChangedHandler(UpdateState::CheckFailed);
                    }
                    break;
                }

                case VersionCheckResult::UpdateNeeded: {
                    _log.info_P(PSTR("firmware update is necessary"));

                    _state = State::DownloadUpdate;

                    if (_updateStateChangedHandler) {
                        _updateStateChangedHandler(UpdateState::DownloadingUpdate);
                    }
                    break;
                }

                default:
                    _log.error_P(PSTR("invalid version check result"));

                    _state = State::Idle;
                    _httpClient.reset();

                    break;
            }
            break;

        case State::DownloadUpdate: {
            _log.info_P(PSTR("downloading and updating firmware"));

            WiFiClient wifiClient;

            switch (ESPhttpUpdate.update(wifiClient, getFwUpdateUrl().c_str())) {
                case HTTP_UPDATE_OK:
                    _log.info_P(PSTR("update succeeded, procedding to reboot"));

                    _rebootTimestamp = _systemClock.utcTime();
                    _state = State::Reboot;

                    if (_updateStateChangedHandler) {
                        _updateStateChangedHandler(UpdateState::RebootingAfterUpdate);
                    }

                    break;

                default:
                    _log.error_P(PSTR("update failed, error: %s"), ESPhttpUpdate.getLastErrorString().c_str());

                    _state = State::Idle;

                    if (_updateStateChangedHandler) {
                        _updateStateChangedHandler(UpdateState::DownloadFailed);
                    }

                    break;
            }
        }

        case State::Reboot: {
            if (_systemClock.utcTime() - _rebootTimestamp >= 5) {
                break;
            }

            _log.info_P(PSTR("rebooting"));

            ESP.restart();

            break;
        }
    }
}

void OtaUpdater::forceUpdate()
{
    _log.info_P(PSTR("forcing update check"));

    _forceUpdateCheck = true;
}

void OtaUpdater::setUpdateStateChangedHandler(UpdateStateChangedHandler&& handler)
{
    _updateStateChangedHandler = std::move(handler);
}

void OtaUpdater::createVersionInfoRequest()
{
    if (_httpClient) {
        _log.warning_P(PSTR("HTTP client already created"));
    }

    std::string url{ _appConfig.otaUpdate.updateUrl };
    url.append("/").append(WiFi.macAddress().c_str()).append("/version");

    _log.debug_P(PSTR("creating version info request, URL: %s"), url.c_str());

    _httpClient.reset(new asyncHTTPrequest);
    _httpClient->setDebug(false);
    _httpClient->setTimeout(5); // seconds
    _httpClient->open("GET", url.c_str());
}

OtaUpdater::VersionCheckResult OtaUpdater::checkVersion(const std::string& versionCheckResponse)
{
    if (versionCheckResponse.empty()) {
        return VersionCheckResult::CannotCheckVersion;
    }

    unsigned major = 0;
    unsigned minor = 0;
    unsigned patch = 0;

    sscanf(versionCheckResponse.c_str(), "%u.%u.%u", &major, &minor, &patch);

    if (VersionNumber{ major, minor, patch } > _appConfig.firmwareVersion) {
        return VersionCheckResult::UpdateNeeded;
    }

    return VersionCheckResult::NoUpdateNeeded;
}

std::string OtaUpdater::getFwUpdateUrl() const
{
    std::string url{ _appConfig.otaUpdate.updateUrl };
    url.append("/").append(WiFi.macAddress().c_str()).append("/firmware.bin");

    return url;
}

#endif