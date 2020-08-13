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

#pragma once

// Define your version numbers here
#define FW_VER_MAJOR                1
#define FW_VER_MINOR                0
#define FW_VER_PATCH                0

#define FW_CORE_APP_VER_MAJOR       1
#define FW_CORE_APP_VER_MINOR       0
#define FW_CORE_APP_VER_PATCH       0

// Utility macros
#define _VER_XSTR(_S) _VER_STR(_S)
#define _VER_STR(_S) #_S

#define MAKE_VER_STR(_MAJOR, _MINOR, _PATCH) \
    _VER_XSTR(_MAJOR) "." _VER_XSTR(_MINOR) "." _VER_XSTR(_PATCH)

// Define version string macros here
#define FW_VER_STR \
    MAKE_VER_STR(FW_VER_MAJOR, FW_VER_MINOR, FW_VER_PATCH)

#define FW_CORE_APP_VER_STR \
    MAKE_VER_STR(FW_CORE_APP_VER_MAJOR, FW_CORE_APP_VER_MINOR, FW_CORE_APP_VER_PATCH)

// End