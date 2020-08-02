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

#include <ctime>
#include <functional>
#include <memory>
#include <string>

class asyncHTTPrequest;
class SystemClock;

class OtaUpdater
{
public:
    OtaUpdater(std::string updateBaseUrl, const SystemClock& systemClock);
    ~OtaUpdater();

    void task();

    void forceUpdate();

    enum class UpdateState
    {
        CheckingForUpdate,
        NoUpdateNeeded,
        DownloadingUpdate,
        RebootingAfterUpdate,
        CheckFailed,
        DownloadFailed
    };

    using UpdateStateChangedHandler = std::function<void(UpdateState)>;
    void setUpdateStateChangedHandler(UpdateStateChangedHandler&& handler);

private:
    const std::string _updateBaseUrl;
    const SystemClock& _systemClock;
    Logger _log{ "OtaUpdater" };
    UpdateStateChangedHandler _updateStateChangedHandler;

    enum class State
    {
        Idle,
        GetAvailableVersion,
        DownloadUpdate,
        Reboot
    };
    State _state = State::Idle;

    std::time_t _lastCheckedForUpdates = 0;
    std::time_t _updateCheckIntervalSeconds = 60;
    std::time_t _rebootTimestamp = 0;
    std::time_t _requestStartTimestamp = 0;
    bool _forceUpdateCheck = false;
    std::unique_ptr<asyncHTTPrequest> _httpClient;

    void createVersionInfoRequest();

    enum class VersionCheckResult
    {
        NoUpdateNeeded,
        UpdateNeeded,
        CannotCheckVersion
    };
    VersionCheckResult checkVersion(const std::string& versionCheckResponse);

    std::string getFwUpdateUrl() const;
};