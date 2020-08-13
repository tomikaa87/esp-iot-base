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

#ifdef IOT_ENABLE_PERSISTENCE

namespace Config
{
    namespace Persistence
    {
// EERAM -->
#if defined(IOT_PERSISTENCE_EERAM_47L16) || defined(IOT_PERSISTENCE_EERAM_47L04)
#define IOT_PERSISTENCE_USE_EERAM
#if defined(IOT_PERSISTENCE_EERAM_47L16)
        static constexpr auto EeramSize = 16384u;
#elif defined(IOT_PERSISTENCE_EERAM_47L04)
        static constexpr auto EeramSize = 4096u;
#endif
// SPIFFS -->
#elif defined(IOT_PERSISTENCE_SPIFFS)
#define IOT_PERSISTENCE_USE_SPIFFS
        static constexpr auto SettingsFileName = "settings.bin";
#endif
    }
}

#endif // IOT_ENABLE_PERSISTENCE
