#pragma once
// ============================================================================
// ButtonManager
//
// Reads every GPIO listed in Config.h's BUTTON_MAP, debounces it, and
// exposes a simple isPressed(index) / getGamepadButton(index) interface.
// Knows nothing about Bluetooth -- it only tracks physical button state.
// ============================================================================

#include <Arduino.h>
#include "Config.h"

class ButtonManager {
public:
    void begin();
    void update();

    bool    isPressed(uint8_t index) const;
    uint8_t getGamepadButton(uint8_t index) const;
    uint8_t getButtonCount() const { return BUTTON_COUNT; }

private:
    struct ButtonState {
        bool     stableState;     // debounced, "true" = pressed
        bool     lastRawState;
        uint32_t lastChangeTime;
    };

    ButtonState _states[BUTTON_COUNT];
};
