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
    Created on 2020-08-09
*/

#include "Hash.h"

// https://en.bitcoinwiki.org/wiki/Fletcher%27s_checksum#Fletcher-16

uint16_t Hash::Fletcher16::calculate(const uint8_t* const data, size_t size)
{
    Fletcher16 f;
    f.data(data, size);
    return f.result();
}

void Hash::Fletcher16::reset()
{
    _sum1 = 0;
    _sum2 = 0;
}

uint16_t Hash::Fletcher16::result() const
{
    return (_sum2 << 8u) | _sum1;
}

void Hash::Fletcher16::data(const uint8_t* const data, size_t size)
{
    for (auto i = 0u; i < size; ++i) {
        _sum1 = (_sum1 + data[i]) % 255;
        _sum2 = (_sum2 + _sum1) % 255;
    }
}
