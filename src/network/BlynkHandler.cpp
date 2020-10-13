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

#ifdef IOT_BLYNK_USE_SSL
#include <BlynkSimpleEsp8266_SSL.h>
#else
#include <BlynkSimpleEsp8266.h>
#endif

static BlynkHandler* g_blynkHandler = nullptr;

#define HANDLE_BLYNK_WRITE(__PIN) BLYNK_WRITE(__PIN) { \
    if (g_blynkHandler) \
        g_blynkHandler->onVirtualPinWritten(__PIN, param); \
}

#define HANDLE_BLYNK_READ(__PIN) BLYNK_READ(__PIN) { \
    if (g_blynkHandler) \
        g_blynkHandler->onVirtualPinRead(__PIN); \
}

BLYNK_CONNECTED()
{
    if (g_blynkHandler) {
        g_blynkHandler->onConnected();
    }
}

BLYNK_DISCONNECTED()
{
    if (g_blynkHandler) {
        g_blynkHandler->onDisconnected();
    }
}

// Register read handlers
HANDLE_BLYNK_READ(V0)
HANDLE_BLYNK_READ(V1)
HANDLE_BLYNK_READ(V2)
HANDLE_BLYNK_READ(V3)
HANDLE_BLYNK_READ(V4)
HANDLE_BLYNK_READ(V5)
HANDLE_BLYNK_READ(V6)
HANDLE_BLYNK_READ(V7)
HANDLE_BLYNK_READ(V8)
HANDLE_BLYNK_READ(V9)
HANDLE_BLYNK_READ(V10)
HANDLE_BLYNK_READ(V11)
HANDLE_BLYNK_READ(V12)
HANDLE_BLYNK_READ(V13)
HANDLE_BLYNK_READ(V14)
HANDLE_BLYNK_READ(V15)
HANDLE_BLYNK_READ(V16)
HANDLE_BLYNK_READ(V17)
HANDLE_BLYNK_READ(V18)
HANDLE_BLYNK_READ(V19)
HANDLE_BLYNK_READ(V20)
HANDLE_BLYNK_READ(V21)
HANDLE_BLYNK_READ(V22)
HANDLE_BLYNK_READ(V23)
HANDLE_BLYNK_READ(V24)
HANDLE_BLYNK_READ(V25)
HANDLE_BLYNK_READ(V26)
HANDLE_BLYNK_READ(V27)
HANDLE_BLYNK_READ(V28)
HANDLE_BLYNK_READ(V29)
HANDLE_BLYNK_READ(V30)
HANDLE_BLYNK_READ(V31)
HANDLE_BLYNK_READ(V32)
HANDLE_BLYNK_READ(V33)
HANDLE_BLYNK_READ(V34)
HANDLE_BLYNK_READ(V35)
HANDLE_BLYNK_READ(V36)
HANDLE_BLYNK_READ(V37)
HANDLE_BLYNK_READ(V38)
HANDLE_BLYNK_READ(V39)
HANDLE_BLYNK_READ(V40)
HANDLE_BLYNK_READ(V41)
HANDLE_BLYNK_READ(V42)
HANDLE_BLYNK_READ(V43)
HANDLE_BLYNK_READ(V44)
HANDLE_BLYNK_READ(V45)
HANDLE_BLYNK_READ(V46)
HANDLE_BLYNK_READ(V47)
HANDLE_BLYNK_READ(V48)
HANDLE_BLYNK_READ(V49)
HANDLE_BLYNK_READ(V50)
HANDLE_BLYNK_READ(V51)
HANDLE_BLYNK_READ(V52)
HANDLE_BLYNK_READ(V53)
HANDLE_BLYNK_READ(V54)
HANDLE_BLYNK_READ(V55)
HANDLE_BLYNK_READ(V56)
HANDLE_BLYNK_READ(V57)
HANDLE_BLYNK_READ(V58)
HANDLE_BLYNK_READ(V59)
HANDLE_BLYNK_READ(V60)
HANDLE_BLYNK_READ(V61)
HANDLE_BLYNK_READ(V62)
HANDLE_BLYNK_READ(V63)
HANDLE_BLYNK_READ(V64)
HANDLE_BLYNK_READ(V65)
HANDLE_BLYNK_READ(V66)
HANDLE_BLYNK_READ(V67)
HANDLE_BLYNK_READ(V68)
HANDLE_BLYNK_READ(V69)
HANDLE_BLYNK_READ(V70)
HANDLE_BLYNK_READ(V71)
HANDLE_BLYNK_READ(V72)
HANDLE_BLYNK_READ(V73)
HANDLE_BLYNK_READ(V74)
HANDLE_BLYNK_READ(V75)
HANDLE_BLYNK_READ(V76)
HANDLE_BLYNK_READ(V77)
HANDLE_BLYNK_READ(V78)
HANDLE_BLYNK_READ(V79)
HANDLE_BLYNK_READ(V80)
HANDLE_BLYNK_READ(V81)
HANDLE_BLYNK_READ(V82)
HANDLE_BLYNK_READ(V83)
HANDLE_BLYNK_READ(V84)
HANDLE_BLYNK_READ(V85)
HANDLE_BLYNK_READ(V86)
HANDLE_BLYNK_READ(V87)
HANDLE_BLYNK_READ(V88)
HANDLE_BLYNK_READ(V89)
HANDLE_BLYNK_READ(V90)
HANDLE_BLYNK_READ(V91)
HANDLE_BLYNK_READ(V92)
HANDLE_BLYNK_READ(V93)
HANDLE_BLYNK_READ(V94)
HANDLE_BLYNK_READ(V95)
HANDLE_BLYNK_READ(V96)
HANDLE_BLYNK_READ(V97)
HANDLE_BLYNK_READ(V98)
HANDLE_BLYNK_READ(V99)
HANDLE_BLYNK_READ(V100)
HANDLE_BLYNK_READ(V101)
HANDLE_BLYNK_READ(V102)
HANDLE_BLYNK_READ(V103)
HANDLE_BLYNK_READ(V104)
HANDLE_BLYNK_READ(V105)
HANDLE_BLYNK_READ(V106)
HANDLE_BLYNK_READ(V107)
HANDLE_BLYNK_READ(V108)
HANDLE_BLYNK_READ(V109)
HANDLE_BLYNK_READ(V110)
HANDLE_BLYNK_READ(V111)
HANDLE_BLYNK_READ(V112)
HANDLE_BLYNK_READ(V113)
HANDLE_BLYNK_READ(V114)
HANDLE_BLYNK_READ(V115)
HANDLE_BLYNK_READ(V116)
HANDLE_BLYNK_READ(V117)
HANDLE_BLYNK_READ(V118)
HANDLE_BLYNK_READ(V119)
HANDLE_BLYNK_READ(V120)
HANDLE_BLYNK_READ(V121)
HANDLE_BLYNK_READ(V122)
HANDLE_BLYNK_READ(V123)
HANDLE_BLYNK_READ(V124)
HANDLE_BLYNK_READ(V125)
HANDLE_BLYNK_READ(V126)
HANDLE_BLYNK_READ(V127)

// Register write handlers
HANDLE_BLYNK_WRITE(V0)
HANDLE_BLYNK_WRITE(V1)
HANDLE_BLYNK_WRITE(V2)
HANDLE_BLYNK_WRITE(V3)
HANDLE_BLYNK_WRITE(V4)
HANDLE_BLYNK_WRITE(V5)
HANDLE_BLYNK_WRITE(V6)
HANDLE_BLYNK_WRITE(V7)
HANDLE_BLYNK_WRITE(V8)
HANDLE_BLYNK_WRITE(V9)
HANDLE_BLYNK_WRITE(V10)
HANDLE_BLYNK_WRITE(V11)
HANDLE_BLYNK_WRITE(V12)
HANDLE_BLYNK_WRITE(V13)
HANDLE_BLYNK_WRITE(V14)
HANDLE_BLYNK_WRITE(V15)
HANDLE_BLYNK_WRITE(V16)
HANDLE_BLYNK_WRITE(V17)
HANDLE_BLYNK_WRITE(V18)
HANDLE_BLYNK_WRITE(V19)
HANDLE_BLYNK_WRITE(V20)
HANDLE_BLYNK_WRITE(V21)
HANDLE_BLYNK_WRITE(V22)
HANDLE_BLYNK_WRITE(V23)
HANDLE_BLYNK_WRITE(V24)
HANDLE_BLYNK_WRITE(V25)
HANDLE_BLYNK_WRITE(V26)
HANDLE_BLYNK_WRITE(V27)
HANDLE_BLYNK_WRITE(V28)
HANDLE_BLYNK_WRITE(V29)
HANDLE_BLYNK_WRITE(V30)
HANDLE_BLYNK_WRITE(V31)
HANDLE_BLYNK_WRITE(V32)
HANDLE_BLYNK_WRITE(V33)
HANDLE_BLYNK_WRITE(V34)
HANDLE_BLYNK_WRITE(V35)
HANDLE_BLYNK_WRITE(V36)
HANDLE_BLYNK_WRITE(V37)
HANDLE_BLYNK_WRITE(V38)
HANDLE_BLYNK_WRITE(V39)
HANDLE_BLYNK_WRITE(V40)
HANDLE_BLYNK_WRITE(V41)
HANDLE_BLYNK_WRITE(V42)
HANDLE_BLYNK_WRITE(V43)
HANDLE_BLYNK_WRITE(V44)
HANDLE_BLYNK_WRITE(V45)
HANDLE_BLYNK_WRITE(V46)
HANDLE_BLYNK_WRITE(V47)
HANDLE_BLYNK_WRITE(V48)
HANDLE_BLYNK_WRITE(V49)
HANDLE_BLYNK_WRITE(V50)
HANDLE_BLYNK_WRITE(V51)
HANDLE_BLYNK_WRITE(V52)
HANDLE_BLYNK_WRITE(V53)
HANDLE_BLYNK_WRITE(V54)
HANDLE_BLYNK_WRITE(V55)
HANDLE_BLYNK_WRITE(V56)
HANDLE_BLYNK_WRITE(V57)
HANDLE_BLYNK_WRITE(V58)
HANDLE_BLYNK_WRITE(V59)
HANDLE_BLYNK_WRITE(V60)
HANDLE_BLYNK_WRITE(V61)
HANDLE_BLYNK_WRITE(V62)
HANDLE_BLYNK_WRITE(V63)
HANDLE_BLYNK_WRITE(V64)
HANDLE_BLYNK_WRITE(V65)
HANDLE_BLYNK_WRITE(V66)
HANDLE_BLYNK_WRITE(V67)
HANDLE_BLYNK_WRITE(V68)
HANDLE_BLYNK_WRITE(V69)
HANDLE_BLYNK_WRITE(V70)
HANDLE_BLYNK_WRITE(V71)
HANDLE_BLYNK_WRITE(V72)
HANDLE_BLYNK_WRITE(V73)
HANDLE_BLYNK_WRITE(V74)
HANDLE_BLYNK_WRITE(V75)
HANDLE_BLYNK_WRITE(V76)
HANDLE_BLYNK_WRITE(V77)
HANDLE_BLYNK_WRITE(V78)
HANDLE_BLYNK_WRITE(V79)
HANDLE_BLYNK_WRITE(V80)
HANDLE_BLYNK_WRITE(V81)
HANDLE_BLYNK_WRITE(V82)
HANDLE_BLYNK_WRITE(V83)
HANDLE_BLYNK_WRITE(V84)
HANDLE_BLYNK_WRITE(V85)
HANDLE_BLYNK_WRITE(V86)
HANDLE_BLYNK_WRITE(V87)
HANDLE_BLYNK_WRITE(V88)
HANDLE_BLYNK_WRITE(V89)
HANDLE_BLYNK_WRITE(V90)
HANDLE_BLYNK_WRITE(V91)
HANDLE_BLYNK_WRITE(V92)
HANDLE_BLYNK_WRITE(V93)
HANDLE_BLYNK_WRITE(V94)
HANDLE_BLYNK_WRITE(V95)
HANDLE_BLYNK_WRITE(V96)
HANDLE_BLYNK_WRITE(V97)
HANDLE_BLYNK_WRITE(V98)
HANDLE_BLYNK_WRITE(V99)
HANDLE_BLYNK_WRITE(V100)
HANDLE_BLYNK_WRITE(V101)
HANDLE_BLYNK_WRITE(V102)
HANDLE_BLYNK_WRITE(V103)
HANDLE_BLYNK_WRITE(V104)
HANDLE_BLYNK_WRITE(V105)
HANDLE_BLYNK_WRITE(V106)
HANDLE_BLYNK_WRITE(V107)
HANDLE_BLYNK_WRITE(V108)
HANDLE_BLYNK_WRITE(V109)
HANDLE_BLYNK_WRITE(V110)
HANDLE_BLYNK_WRITE(V111)
HANDLE_BLYNK_WRITE(V112)
HANDLE_BLYNK_WRITE(V113)
HANDLE_BLYNK_WRITE(V114)
HANDLE_BLYNK_WRITE(V115)
HANDLE_BLYNK_WRITE(V116)
HANDLE_BLYNK_WRITE(V117)
HANDLE_BLYNK_WRITE(V118)
HANDLE_BLYNK_WRITE(V119)
HANDLE_BLYNK_WRITE(V120)
HANDLE_BLYNK_WRITE(V121)
HANDLE_BLYNK_WRITE(V122)
HANDLE_BLYNK_WRITE(V123)
HANDLE_BLYNK_WRITE(V124)
HANDLE_BLYNK_WRITE(V125)
HANDLE_BLYNK_WRITE(V126)
HANDLE_BLYNK_WRITE(V127)

BlynkHandler::BlynkHandler(const ApplicationConfig& appConfig)
    : _appConfig(appConfig)
{
    _log.debug("initializing: server=%s, port=%d",
        _appConfig.blynk.serverHostName,
        _appConfig.blynk.serverPort
    );

    Blynk.config(
        _appConfig.blynk.appToken,
        _appConfig.blynk.serverHostName,
        _appConfig.blynk.serverPort,
#ifdef IOT_BLYNK_USE_SSL
        _appConfig.blynk.sslFingerprint
#endif
    );

    g_blynkHandler = this;
}

BlynkHandler::~BlynkHandler()
{
    g_blynkHandler = nullptr;
}

void BlynkHandler::task()
{
    Blynk.run();
}

void BlynkHandler::onConnected()
{
    _log.debug("connected");

    if (_connectedHandler) {
        _connectedHandler();
    }
}

void BlynkHandler::onDisconnected()
{
    _log.debug("disconnected");

    if (_disconnectedHandler) {
        _disconnectedHandler();
    }
}

void BlynkHandler::onVirtualPinWritten(const int pin, const BlynkParam& param)
{
    _log.debug("virtual pin written: pin=%d", pin);

    if (_genericPinWrittenHandler) {
        _genericPinWrittenHandler(pin, toVariant(param));
    }

    const auto& handler = _pinWrittenHandlers[pin];
    if (handler) {
        handler(pin, toVariant(param));
    }
}

void BlynkHandler::onVirtualPinRead(const int pin)
{
    _log.debug("virtual pin read: pin=%d", pin);

    if (_genericPinReadHandler) {
        writeVariant(pin, _genericPinReadHandler(pin));
    }

    const auto& handler = _pinReadHandlers[pin];
    if (handler) {
        writeVariant(pin, handler(pin));
    }
}

void BlynkHandler::setConnectedHandler(ConnectedHandler&& handler)
{
    _connectedHandler = std::move(handler);
}

void BlynkHandler::setDisconnectedHandler(DisconnectedHandler&& handler)
{
    _disconnectedHandler = std::move(handler);
}

void BlynkHandler::setPinReadHandler(const int pin, PinReadHandler&& handler)
{
    _pinReadHandlers[pin] = std::move(handler);
}

void BlynkHandler::setPinReadHandler(PinReadHandler&& handler)
{
    _genericPinReadHandler = std::move(handler);
}

void BlynkHandler::setPinWrittenHandler(const int pin, PinWrittenHandler&& handler)
{
    _pinWrittenHandlers[pin] = std::move(handler);
}

void BlynkHandler::setPinWrittenHandler(PinWrittenHandler&& handler)
{
    _genericPinWrittenHandler = std::move(handler);
}

void BlynkHandler::writePin(int pin, const Variant& value)
{
    writeVariant(pin, value);
}

void BlynkHandler::writeTerminal(const int pin, const char* text)
{
    WidgetTerminal terminal{ pin };
    terminal.println(text);
    terminal.flush();
}

Variant BlynkHandler::toVariant(const BlynkParam& param)
{
    if (param.isEmpty()) {
        return {};
    }

    const auto* valstr = param.asString();

    if (!valstr) {
        return {};
    }

    const auto len = strlen(valstr);

    if (len == 0) {
        return {};
    }

    char* endptr = nullptr;

    // Try to convert into integer
    const int integerValue = std::strtol(valstr, &endptr, 10);
    if (strlen(endptr) == 0) {
        return Variant{ integerValue };
    }

    // Try to fall back to double
    const auto doubleValue = std::strtod(valstr, &endptr);
    if (strlen(endptr) == 0) {
        return Variant{ doubleValue };
    }

    // Value is not a number, return a string
    return Variant{ valstr };
}

void BlynkHandler::writeVariant(const int pin, const Variant& value)
{
    switch (value.type()) {
        case Variant::Type::Empty:
            return;

        case Variant::Type::Integer:
            Blynk.virtualWrite(pin, static_cast<int>(value));
            return;

        case Variant::Type::Float:
            Blynk.virtualWrite(pin, static_cast<float>(value));
            return;

        case Variant::Type::Double:
            Blynk.virtualWrite(pin, static_cast<double>(value));
            return;

        case Variant::Type::String:
            Blynk.virtualWrite(pin, static_cast<const char*>(value));
            return;
    }
}