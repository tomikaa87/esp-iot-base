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
    Created on 2025-03-31
*/

#pragma once

#include <IPAddress.h>

#include <functional>
#include <variant>

using HostAddress = std::variant<IPAddress, const char*>;

inline void toString(const std::function<void (const char*)>& callback, const HostAddress& hostAddress)
{
    std::visit(
        [&]<typename ValueType>(const ValueType& value) {
            if constexpr (std::is_same_v<ValueType, IPAddress>) {
                callback(value.toString().c_str());
            } else {
                callback(value);
            }
        },
        hostAddress
    );
}