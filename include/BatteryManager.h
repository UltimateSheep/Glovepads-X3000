#pragma once
// ============================================================================
// BatteryManager
//
// Periodically samples the battery-sense ADC pin, converts it to a real
// battery voltage using the configured divider ratio, smooths it, and
// turns it into a 0-100% level. Fully inert if BATTERY_MONITORING_ENABLED
// is false in Config.h.
// ============================================================================

#include <Arduino.h>
#include "Config.h"

class BatteryManager {
public:
    void begin();
    void update();   // call every loop; internally rate-limited

    uint8_t getPercentage() const { return _percentage; }
    float   getVoltage() const { return _smoothedVoltage; }

    // True exactly once after a new reading is taken -- lets the caller
    // know it's worth pushing a fresh value over BLE.
    bool isDue() const { return _reportDue; }
    void clearDue() { _reportDue = false; }

private:
    void sampleNow();

    float    _smoothedVoltage = BATTERY_FULL_VOLTAGE;
    uint8_t  _percentage      = 100;
    uint32_t _lastUpdate      = 0;
    bool     _firstReading    = true;
    bool     _reportDue       = false;
};
