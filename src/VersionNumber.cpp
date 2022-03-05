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

bool VersionNumber::operator==(const VersionNumber& v) const
{
    return _parts == v._parts;
}

bool VersionNumber::operator!=(const VersionNumber& v) const
{
    return !(*this == v);
}

// FIXME this algorithm considers 1.4.1 to be less than 1.2.15

bool VersionNumber::operator<(const VersionNumber& v) const
{
    for (auto i = 0u; i < _parts.size(); ++i) {
        if (_parts[i] < v._parts[i]) {
            return true;
        }
    }

    return false;
}

bool VersionNumber::operator>(const VersionNumber& v) const
{
    return !(*this == v) && !(*this < v);
}