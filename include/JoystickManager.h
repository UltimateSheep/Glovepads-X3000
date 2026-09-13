#pragma once
// ============================================================================
// JoystickManager
//
// Reads the two ADC pins for the analog stick, auto-calibrates the center
// point at boot, applies a deadzone, and scales the result to the signed
// -32767..32767 range the BLE HID gamepad report expects.
// ============================================================================

#include <Arduino.h>
#include "Config.h"

class JoystickManager {
public:
    void begin();
    void update();

    int16_t getX() const { return _outX; }
    int16_t getY() const { return _outY; }

private:
    int applyDeadzoneAndScale(int raw, int center, bool invert) const;

    int     _centerX = ADC_MAX_RAW / 2;
    int     _centerY = ADC_MAX_RAW / 2;
    int16_t _outX    = 0;
    int16_t _outY    = 0;
};
