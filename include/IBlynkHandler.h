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
    Created on 2020-08-04
*/

#pragma once

#include <Blynk/BlynkParam.h>

#include <functional>

class IBlynkHandler
{
public:
    using ConnectedHandler = std::function<void ()>;
    using DisconnectedHandler = std::function<void ()>;
    using PinWrittenHandler = std::function<void (int pin, const BlynkParam& param)>;
    using PinReadHandler = std::function<BlynkParam (int pin)>;

    virtual void setConnectedHandler(ConnectedHandler&& handler) = 0;
    virtual void setDisconnectedHandler(DisconnectedHandler&& handler) = 0;
    virtual void setPinReadHandler(int pin, PinReadHandler&& handler) = 0;
    virtual void setPinWrittenHandler(int pin, PinWrittenHandler&& handler) = 0;
};