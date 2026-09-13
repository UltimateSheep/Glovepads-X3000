#include "ButtonManager.h"

void ButtonManager::begin() {
    for (uint8_t i = 0; i < BUTTON_COUNT; i++) {
        pinMode(BUTTON_MAP[i].pin, INPUT_PULLUP);

        bool raw     = digitalRead(BUTTON_MAP[i].pin);
        bool pressed = BUTTON_MAP[i].activeLow ? (raw == LOW) : (raw == HIGH);

        _states[i].stableState    = pressed;
        _states[i].lastRawState   = pressed;
        _states[i].lastChangeTime = millis();

        DEBUG_PRINTF("[Button] index %d -> GPIO%d ready\n", i, BUTTON_MAP[i].pin);
    }
}

void ButtonManager::update() {
    for (uint8_t i = 0; i < BUTTON_COUNT; i++) {
        bool raw        = digitalRead(BUTTON_MAP[i].pin);
        bool rawPressed = BUTTON_MAP[i].activeLow ? (raw == LOW) : (raw == HIGH);

        if (rawPressed != _states[i].lastRawState) {
            _states[i].lastChangeTime = millis();
            _states[i].lastRawState   = rawPressed;
        }

        bool stableEnough = (millis() - _states[i].lastChangeTime) > DEBOUNCE_DELAY_MS;

        if (stableEnough && (_states[i].stableState != rawPressed)) {
            _states[i].stableState = rawPressed;
            DEBUG_PRINTF("[Button] %d (GPIO%d) %s\n",
                         i + 1, BUTTON_MAP[i].pin, rawPressed ? "PRESSED" : "released");
        }
    }
}

bool ButtonManager::isPressed(uint8_t index) const {
    if (index >= BUTTON_COUNT) return false;
    return _states[index].stableState;
}

uint8_t ButtonManager::getGamepadButton(uint8_t index) const {
    if (index >= BUTTON_COUNT) return BUTTON_1;
    return BUTTON_MAP[index].gamepadButton;
}
