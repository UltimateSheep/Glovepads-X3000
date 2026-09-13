#include "GamepadController.h"

void GamepadController::begin() {
    BleGamepadConfiguration config;

    config.setAutoReport(false);                 // we send one combined report per loop instead
    config.setControllerType(CONTROLLER_TYPE_GAMEPAD);
    config.setButtonCount(BUTTON_COUNT);          // derived from Config.h's BUTTON_MAP
    config.setHatSwitchCount(0);                  // no D-pad hat in this build

    // Only expose X/Y -- matches the single analog stick on this hardware.
    config.setWhichAxes(/*X*/ true, /*Y*/ true, /*Z*/ false,
                         /*RX*/ false, /*RY*/ false, /*RZ*/ false,
                         /*Slider1*/ false, /*Slider2*/ false);
    config.setAxesMin(GAMEPAD_AXIS_MIN);
    config.setAxesMax(GAMEPAD_AXIS_MAX);

    _bleGamepad.begin(&config);

    DEBUG_PRINTLN("[Gamepad] BLE HID gamepad started, advertising...");
}

bool GamepadController::isConnected() {
    return _bleGamepad.isConnected();
}

void GamepadController::updateButtons(ButtonManager &buttons) {
    for (uint8_t i = 0; i < buttons.getButtonCount(); i++) {
        uint8_t gamepadButton = buttons.getGamepadButton(i);
        if (buttons.isPressed(i)) {
            _bleGamepad.press(gamepadButton);
        } else {
            _bleGamepad.release(gamepadButton);
        }
    }
}

void GamepadController::updateAxes(JoystickManager &joystick) {
    _bleGamepad.setX(joystick.getX());
    _bleGamepad.setY(joystick.getY());
}

void GamepadController::updateBattery(BatteryManager &battery) {
    if (!BATTERY_MONITORING_ENABLED) return;

    if (battery.isDue()) {
        _bleGamepad.setBatteryLevel(battery.getPercentage());
        battery.clearDue();
    }
}

void GamepadController::sendReport() {
    _bleGamepad.sendReport();
}
