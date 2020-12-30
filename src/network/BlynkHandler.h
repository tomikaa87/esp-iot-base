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
    Created on 2017-01-12
*/

#pragma once

#include "ApplicationConfig.h"
#include "IBlynkHandler.h"
#include "Logger.h"

#include <functional>
#include <map>

class BlynkParam;

class BlynkHandler : public IBlynkHandler
{
public:
    BlynkHandler(const ApplicationConfig& appConfig);
    ~BlynkHandler();

    bool task();

    // Internal handlers for Blynk events
    void onConnected();
    void onDisconnected();
    void onVirtualPinWritten(int pin, const BlynkParam& param);
    void onVirtualPinRead(int pin);

    void setConnectedHandler(ConnectedHandler&& handler) override;
    void setDisconnectedHandler(DisconnectedHandler&& handler) override;
    void setPinReadHandler(int pin, PinReadHandler&& handler) override;
    void setPinReadHandler(PinReadHandler&& handler) override;
    void setPinWrittenHandler(int pin, PinWrittenHandler&& handler) override;
    void setPinWrittenHandler(PinWrittenHandler&& handler) override;

    void writePin(int pin, const Variant& value) override;

    void writeTerminal(int pin, const char* text) override;

private:
    Logger _log{ "Blynk" };
    const ApplicationConfig& _appConfig;

    ConnectedHandler _connectedHandler;
    DisconnectedHandler _disconnectedHandler;

    std::map<int, PinReadHandler> _pinReadHandlers;
    PinReadHandler _genericPinReadHandler;
    std::map<int, PinWrittenHandler> _pinWrittenHandlers;
    PinWrittenHandler _genericPinWrittenHandler;

    static Variant toVariant(const BlynkParam& param);
    static void writeVariant(int pin, const Variant& value);
};
