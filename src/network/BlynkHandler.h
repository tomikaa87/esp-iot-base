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

#include "Logger.h"

#include <cstdint>
#include <functional>

class BlynkParam;
class Settings;

class BlynkHandler
{
public:
    BlynkHandler();
    ~BlynkHandler();

    void task();

    void onBlynkConnected();
    void onVirtualPinUpdated(int pin, const BlynkParam& param);
    void onButtonPressed(int pin);
    void updateVirtualPin(int pin);

private:
    Logger _log{ "Blynk" };

    template <typename T, int size>
    inline void floatToStr(const float f, T(&buf)[size]);
};
