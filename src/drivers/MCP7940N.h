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
    Created on 2020-02-24
*/

#pragma once

#include "Logger.h"

#include <cstdint>

namespace Drivers
{

class MCP7940N
{
public:
    struct DateTime
    {
        uint8_t year = 0;
        uint8_t month = 0;
        uint8_t date = 0;
        uint8_t weekday = 0;
        uint8_t hours = 0;
        uint8_t minutes = 0;
        uint8_t seconds = 0;
        bool is12Hours = false;
        bool pm = false;
        bool leapYear = false;

        DateTime() = default;

        DateTime(
            const uint8_t year,
            const uint8_t month,
            const uint8_t date,
            const uint8_t weekday,
            const uint8_t hours,
            const uint8_t minutes,
            const uint8_t seconds,
            const bool is12Hours = false,
            const bool pm = false
        )
            : year{ year }
            , month{ month }
            , date{ date }
            , weekday{ weekday }
            , hours{ hours }
            , minutes{ minutes }
            , seconds{ seconds }
            , is12Hours{ is12Hours }
            , pm{ pm }
        {}
    };

    struct Alarm
    {
        uint8_t month = 0;
        uint8_t date = 0;
        uint8_t weekday = 0;
        uint8_t hours = 0;
        uint8_t minutes = 0;
        uint8_t seconds = 0;
    };

    struct PowerFailTimestamp
    {
        uint8_t month = 0;
        uint8_t date = 0;
        uint8_t hours = 0;
        uint8_t minutes = 0;
    };

    enum class AlarmModule : uint8_t
    {
        Module0,
        Module1
    };

    enum class OutputConfig : uint8_t
    {
        GeneralPurpose,
        Alarm0          = 0b010,
        Alarm1          = 0b001,
        BothAlarms      = 0b011,
        SquareWave      = 0b100
    };

    enum class SquareWaveFrequency : uint8_t
    {
        Output1Hz,
        Output4096Hz,
        Output8192Hz,
        Output32768Hz
    };

    static bool isOscillatorRunning();

    static void setOscillatorEnabled(bool enabled);
    static bool isOscillatorEnabled();
    static void setExternalOscillatorEnabled(bool enabled);

    static void clearPowerFailFlag();
    static bool getPowerFailFlag();

    static void setBatteryEnabled(bool enabled);
    static bool isBatteryEnabled();

    static void set12HoursEnabled(bool enabled);
    static bool is12HoursEnabled();

    static void setDateTime(const DateTime& dt);
    static DateTime getDateTime();

    static void setAlarm(AlarmModule module, const Alarm& alarm);
    static Alarm getAlarm(AlarmModule module);

    static void setOutputConfig(OutputConfig config);
    static OutputConfig getOutputConfig();
    static void setSquareWaveOutputFrequency(SquareWaveFrequency frequency);

    static void writeGpo(bool high);

    static void setDigitalTrimming(int8_t trimval);
    static int8_t getDigitalTrimming();
    static void setCoarseTrimmingEnabled(bool enabled);

    static PowerFailTimestamp getPowerDownTimestamp();
    static PowerFailTimestamp getPowerUpTimestamp();

    static bool writeSram(uint8_t address, uint8_t value);
    static uint8_t writeSram(uint8_t address, const uint8_t* buffer, uint8_t length);

    static uint8_t readSram(uint8_t address);
    static uint8_t readSram(uint8_t address, uint8_t* buffer, uint8_t length);

    static uint8_t fromBcd(uint8_t value);
    static uint8_t toBcd(uint8_t value);

private:
    static constexpr uint8_t ControlByte = 0b1101111;

    static Logger _log;

    enum class Register : uint8_t
    {
        // Timekeeping
        RTCSEC,
        RTCMIN,
        RTCHOUR,
        RTCWKDAY,
        RTCDATE,
        RTCMTH,
        RTCYEAR,
        CONTROL,
        OSCTRIM,

        // Alarm 0
        ALM0SEC     = 0x0a,
        ALM0MIN,
        ALM0HOUR,
        ALM0WKDAY,
        ALM0DATE,
        ALM0MTH,

        // Alarm 1
        ALM1SEC     = 0x11,
        ALM1MIN,
        ALM1HOUR,
        ALM1WKDAY,
        ALM1DATE,
        ALM1MTH,

        // Power-Down Timestamp
        PWRDNMIN    = 0x18,
        PWRDNHOUR,
        PWRDNDATE,
        PWRDNMTH,

        // Power-Up Timestamp
        PWRUPMIN,
        PWRUPHOUR,
        PWRUPDATE,
        PWRUPMTH,

        // SRAM
        SramStart   = 0x20,
        SramEnd     = SramStart + 64
    };

    static uint8_t write(uint8_t address, const uint8_t* buffer, uint8_t length);
    static uint8_t write(Register reg, const uint8_t* buffer, uint8_t length);
    static bool write(Register reg, uint8_t value);

    static uint8_t read(uint8_t address, uint8_t* buffer, uint8_t length);
    static uint8_t read(Register reg, uint8_t* buffer, uint8_t length);
    static uint8_t read(Register reg);
};

}