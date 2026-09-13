#pragma once
// ============================================================================
// GamepadController
//
// The only file that talks to the ESP32-BLE-Gamepad library directly.
// Translates the plain-old-data state coming out of ButtonManager /
// JoystickManager / BatteryManager into BLE HID gamepad reports.
// ============================================================================

#include <Arduino.h>
#include <BleGamepad.h>
#include "Config.h"
#include "ButtonManager.h"
#include "JoystickManager.h"
#include "BatteryManager.h"

class GamepadController {
public:
    void begin();
    bool isConnected();

    void updateButtons(ButtonManager &buttons);
    void updateAxes(JoystickManager &joystick);
    void updateBattery(BatteryManager &battery);
    void sendReport();

private:
    BleGamepad _bleGamepad{BLE_DEVICE_NAME, BLE_DEVICE_MANUFACTURER, 100};
};
