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
    Created on 2020-12-31
*/

#include "WiFiWatchdog.h"

#include <ESP8266WiFi.h>

void WiFiWatchdog::task()
{
    if (millis() - _taskTimer < TaskIntervalMs) {
        return;
    }

    _taskTimer = millis();

    switch (_state) {
        case State::WaitingForInitialConnect: {
            if (!WiFi.isConnected()) {
                break;
            }

            _log.debug("WaitingForInitialConnect -> Connected");

            _state = State::Connected;

            break;
        }

        case State::Connected: {
            if (WiFi.isConnected()) {
                break;
            }

            _log.debug("Connected -> WaitingForReconnect");

            _state = State::WaitingForReconnect;
            _reconnectTimer = millis();

            break;
        }

        case State::WaitingForReconnect: {
            if (WiFi.isConnected()) {
                _log.debug("WaitingForReconnect -> Connected");

                _state = State::Connected;

                break;
            }

            if (millis() - _reconnectTimer > ReconnectTimeoutMs) {
                _log.debug("automatic reconnect timed out, forcing manual");

                if (!WiFi.reconnect()) {
                    _log.warning("manual reconnect failed");
                }

                _reconnectTimer = millis();
            }

            break;
        }
    }
}

bool WiFiWatchdog::isConnected() const
{
    return _state == State::Connected;
}