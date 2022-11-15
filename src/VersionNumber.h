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
    Created on 2020-08-14
*/

#pragma once

#include <array>
#include <string>

class VersionNumber
{
public:
    VersionNumber() = default;
    VersionNumber(unsigned major, unsigned minor, unsigned patch);

    VersionNumber(const VersionNumber&) = default;
    VersionNumber(VersionNumber&&) = default;

    std::string toString() const;

    VersionNumber& operator=(const VersionNumber&) = default;
    VersionNumber& operator=(VersionNumber&&) = default;

    bool operator==(const VersionNumber& other) const;
    bool operator!=(const VersionNumber& other) const;
    bool operator<(const VersionNumber& other) const;
    bool operator>(const VersionNumber& other) const;

private:
    // This overly verbose initialization is needed for xtensa gcc
    std::array<unsigned, 3> _parts = decltype(_parts){ 0 };
};