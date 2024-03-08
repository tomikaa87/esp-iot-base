#pragma once

#include <cstdint>

#include <Arduino.h>

class ElapsedTimer
{
public:
    ElapsedTimer()
    {
        reset();
    }

    void reset()
    {
        _startMs = millis();
    }

    [[nodiscard]] uint32_t elapsed() const
    {
        return millis() - _startMs;
    }

private:
    uint32_t _startMs{};
};