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

#include "SystemClock.h"
#include "NtpClient.h"

NtpClient::NtpClient(SystemClock& systemClock)
    : _systemClock(systemClock)
{
    _log.info_P(PSTR("initializing"));
}

void NtpClient::task()
{
    switch (_state) {
        case State::Idle:
            if (_lastUpdate == 0 || _systemClock.utcTime() - _lastUpdate > UpdateInterval) {
                _log.info_P(PSTR("update needed, starting"));
                _state = State::SendPacket;

                _socket.reset(new WiFiUDP);
                _socket->begin(NtpDefaultLocalPort);
            }
            break;

        case State::SendPacket:
            _sendTimestamp = millis();
            sendPacket();
            _log.info_P(PSTR("waiting for response"));
            _state = State::WaitResponse;
            break;

        case State::WaitResponse: {
            const auto elapsed = millis() - _sendTimestamp;

            if (_socket->parsePacket() == 0) {
                if (elapsed > 1000) {
                    _log.warning_P(PSTR("socket timeout, retrying in %d second(s)"), RetryIntervalSeconds);
                    _lastUpdate = _systemClock.utcTime() - UpdateInterval + RetryIntervalSeconds;
                    _state = State::Idle;
                    _socket.reset();
                }

                break;
            }

            uint8_t packet[NtpPacketSize] = { 0 };
            if (!_socket->read(packet, sizeof(packet))) {
                _log.warning_P(PSTR("socket read failed"));
            }

            const uint32_t hi = word(packet[40], packet[41]);
            const uint32_t lo = word(packet[42], packet[43]);
            const time_t secsSince1900 = hi << 16 | lo;
            const time_t epoch = secsSince1900 - SeventyYears;

            _systemClock.setUtcTime(epoch);

            _log.info_P(PSTR("finished, epoch: %ld"), epoch);
            _lastUpdate = _systemClock.utcTime();
            _state = State::Idle;
            _socket.reset();

            break;
        }
    }
}

void NtpClient::setUpdatedCallback(UpdatedHandler&& handler)
{
    _log.debug_P(PSTR("setting Updated callback"));
    _updatedHandler = std::move(handler);
}

void NtpClient::sendPacket()
{
    _log.debug_P(PSTR("sending NTP packet"));

    if (!_socket) {
        _log.error_P(PSTR("socket is null"));
        return;
    }

    uint8_t packet[NtpPacketSize] = { 0 };
    packet[0] = 0b11100011;     // LI, Version, Mode
    packet[1] = 0;              // Stratum or type of clock
    packet[2] = 6;              // Polling interval
    packet[3] = 0xEC;           // Peer Clock Precision
    // 8 bytes of zero for Root Delay & Root Dispersion
    packet[12]  = 49;
    packet[13]  = 0x4E;
    packet[15]  = 52;

    if (!_socket->beginPacket(Server, 123)) {
        _log.warning_P(PSTR("cannot send NTP packet, beginPacket() failed"));
        return;
    }

    if (!_socket->write(packet, sizeof(packet))) {
        _log.warning_P(PSTR("cannot write NTP packet to the socket"));
    }

    if (!_socket->endPacket()) {
        _log.warning_P(PSTR("cannot sent NTP packet, endPacket() failed"));
    }
}