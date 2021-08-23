#pragma once

#include "Logger.h"

#include <pgmspace.h>

#include <cstdint>

class MemoryMonitor
{
public:
    MemoryMonitor();

    void task();

    void logMemoryStats(PGM_P step = nullptr);

private:
    Logger _log{ "MemoryMonitor" };
    uint32_t _lastCheckTimestamp = 0;
};