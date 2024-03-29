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
    Created on 2020-08-13
*/

#include "VersionNumber.h"

VersionNumber::VersionNumber(
    const unsigned major,
    const unsigned minor,
    const unsigned patch
)
    : _parts({ major, minor, patch })
{}

std::string VersionNumber::toString() const
{
    std::string s;

    for (auto i = 0u; i < _parts.size(); ++i) {
        s += std::to_string(_parts[i]);
        if (i < _parts.size() - 1) {
            s += '.';
        }
    }

    return s;
}

bool VersionNumber::operator==(const VersionNumber& other) const
{
    return _parts == other._parts;
}

bool VersionNumber::operator!=(const VersionNumber& other) const
{
    return !(*this == other);
}

bool VersionNumber::operator<(const VersionNumber& other) const
{
    for (auto i = 0u; i < _parts.size(); ++i) {
        if (_parts[i] > other._parts[i]) {
            return false;
        }
    }

    return true;
}

bool VersionNumber::operator>(const VersionNumber& other) const
{
    return !(*this == other) && !(*this < other);
}