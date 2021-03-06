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

#include <memory>

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

    CoreApplication(const ApplicationConfig& appConfig);
    ~CoreApplication();

    void task();

    IBlynkHandler& blynkHandler();
#ifdef IOT_ENABLE_PERSISTENCE
    ISettingsHandler& settings();
#endif

    ISystemClock& systemClock();

    void setBlynkUpdateHandler(BlynkUpdateHandler&& handler);
    void setArduinoOtaEventHandler(ArduinoOtaEventHandler&& handler);

private:
    struct Private;
    std::unique_ptr<Private> _p;
};
