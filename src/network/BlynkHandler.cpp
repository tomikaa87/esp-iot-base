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

#include "BlynkHandler.h"
#include "Config.h"

#define ENABLE_DEBUG

#ifdef ENABLE_DEBUG
#define BLYNK_PRINT     Serial
#endif // ENABLE_DEBUG

#include <BlynkSimpleEsp8266.h>

static BlynkHandler* g_blynkHandler = nullptr;

#define HANDLE_BLYNK_WRITE(__PIN) BLYNK_WRITE(__PIN) { \
    if (g_blynkHandler) \
        g_blynkHandler->onVirtualPinUpdated(__PIN, param); \
}

#define HANDLE_BLYNK_READ(__PIN) BLYNK_READ(__PIN) { \
    if (g_blynkHandler) \
        g_blynkHandler->updateVirtualPin(__PIN); \
}

#define HANDLE_BLYNK_BUTTON_PRESS(__PIN) BLYNK_WRITE(__PIN) { \
    if (g_blynkHandler && param.asInt() == 1) \
        g_blynkHandler->onButtonPressed(__PIN); \
}

BLYNK_CONNECTED()
{
    if (g_blynkHandler)
        g_blynkHandler->onBlynkConnected();
}

BlynkHandler::BlynkHandler()
{
    g_blynkHandler = this;

    Blynk.config(
        Config::Blynk::AppToken,
        Config::Blynk::ServerHostName,
        Config::Blynk::ServerPort
    );
}

BlynkHandler::~BlynkHandler()
{
    g_blynkHandler = nullptr;
}

void BlynkHandler::task()
{
    Blynk.run();
}

void BlynkHandler::onBlynkConnected()
{
    _log.debug("connected");
}

void BlynkHandler::onVirtualPinUpdated(int pin, const BlynkParam& param)
{
    _log.debug("virtual pin updated, pin: %d", pin);

    // Handle "real" data updates (not button presses)
    // here.

    // This callback should run as short as possible
    // to avoid Blynk disconnect errors
}

void BlynkHandler::onButtonPressed(int pin)
{
    // This callback should run as short as possible
    // to avoid Blynk disconnect errors
}

void BlynkHandler::updateVirtualPin(int pin)
{
    _log.debug("updating virtual pin: %d", pin);

    // Handle virtual pin reads by writing the required data
}

template <typename T, int size>
inline void BlynkHandler::floatToStr(const float f, T(&buf)[size])
{
    static_assert(size >= 4, "Output buffer is too small");
    snprintf(buf, size - 1, "%0.1f", f);
}
