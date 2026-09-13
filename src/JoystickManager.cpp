#include "JoystickManager.h"

void JoystickManager::begin() {
    pinMode(JOYSTICK_X_PIN, INPUT);
    pinMode(JOYSTICK_Y_PIN, INPUT);

    analogReadResolution(ADC_RESOLUTION_BITS);
    analogSetAttenuation(ADC_11db); // allows reading the full 0-3.3V-ish range

    DEBUG_PRINTLN("[Joystick] Calibrating center, keep the stick released...");

    uint32_t sumX = 0, sumY = 0;
    for (uint16_t i = 0; i < JOYSTICK_CALIBRATION_SAMPLES; i++) {
        sumX += analogRead(JOYSTICK_X_PIN);
        sumY += analogRead(JOYSTICK_Y_PIN);
        delayMicroseconds(200);
    }

    _centerX = sumX / JOYSTICK_CALIBRATION_SAMPLES;
    _centerY = sumY / JOYSTICK_CALIBRATION_SAMPLES;

    DEBUG_PRINTF("[Joystick] Center calibrated: X=%d Y=%d\n", _centerX, _centerY);
}

// Maps [center +/- deadzone .. ADC_MIN/MAX] onto [0 .. GAMEPAD_AXIS_MIN/MAX],
// independently for each side of center so an off-center calibration still
// reaches full deflection in both directions.
int JoystickManager::applyDeadzoneAndScale(int raw, int center, bool invert) const {
    int diff = raw - center;

    if (abs(diff) <= (int)JOYSTICK_DEADZONE) {
        return 0;
    }

    int32_t scaled;
    if (diff > 0) {
        int span = ADC_MAX_RAW - center - JOYSTICK_DEADZONE;
        if (span <= 0) span = 1;
        int adjusted = diff - JOYSTICK_DEADZONE;
        scaled = (int32_t)adjusted * GAMEPAD_AXIS_MAX / span;
    } else {
        int span = center - ADC_MIN_RAW - JOYSTICK_DEADZONE;
        if (span <= 0) span = 1;
        int adjusted = -diff - JOYSTICK_DEADZONE;
        scaled = (int32_t)adjusted * GAMEPAD_AXIS_MIN / span; // MIN is negative
    }

    scaled = constrain(scaled, (int32_t)GAMEPAD_AXIS_MIN, (int32_t)GAMEPAD_AXIS_MAX);
    return invert ? -scaled : (int)scaled;
}

void JoystickManager::update() {
    int rawX = analogRead(JOYSTICK_X_PIN);
    int rawY = analogRead(JOYSTICK_Y_PIN);

    _outX = (int16_t)applyDeadzoneAndScale(rawX, _centerX, JOYSTICK_INVERT_X);
    _outY = (int16_t)applyDeadzoneAndScale(rawY, _centerY, JOYSTICK_INVERT_Y);
}
