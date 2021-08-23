#include "MemoryMonitor.h"

#include <Arduino.h>
#include <Esp.h>

namespace
{
    constexpr uint32_t CheckIntervalMs = 5000;
}

MemoryMonitor::MemoryMonitor()
{
    _log.info_p(PSTR("created"));
    logMemoryStats(PSTR("constructor"));
}

void MemoryMonitor::task()
{
    const auto timestamp = millis();

    if (timestamp - _lastCheckTimestamp >= CheckIntervalMs) {
        _lastCheckTimestamp = timestamp;
        logMemoryStats();
    }
}

void MemoryMonitor::logMemoryStats(PGM_P step)
{
    _log.info_p(
        PSTR("step: %s, free heap: %u B, fragmentation: %u, largest free block: %u B"),
        step ? step : PSTR("task"),
        ESP.getFreeHeap(),
        ESP.getHeapFragmentation(),
        ESP.getMaxFreeBlockSize()
    );

    system_show_malloc();
}