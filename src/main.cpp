// ============================================================================
// ESP32-C3 Super Mini -- Modular BLE Gamepad
//
// setup()/loop() only wire the modules together; all real logic lives in
// ButtonManager, JoystickManager, BatteryManager, and GamepadController.
// All pins and tunables live in include/Config.h.
// ============================================================================

#include <Arduino.h>
#include "Config.h"
#include "ButtonManager.h"
#include "JoystickManager.h"
#include "BatteryManager.h"
#include "GamepadController.h"

ButtonManager      buttons;
JoystickManager    joystick;
BatteryManager     battery;
GamepadController  gamepad;

void setup() {
    Serial.begin(SERIAL_BAUD_RATE);
    delay(300); // give the C3's native USB-CDC serial time to enumerate

    DEBUG_PRINTLN("\n=== ESP32-C3 BLE Gamepad ===");

    buttons.begin();
    joystick.begin();
    battery.begin();
    gamepad.begin();

    DEBUG_PRINTLN("Setup complete. Advertising over BLE, waiting for a host to connect...");
}

void loop() {
    // Hardware is always polled, connected or not, so debug prints and
    // debouncing stay accurate even before pairing.
    buttons.update();
    joystick.update();
    battery.update();

    if (gamepad.isConnected()) {
        gamepad.updateButtons(buttons);
        gamepad.updateAxes(joystick);
        gamepad.updateBattery(battery);
        gamepad.sendReport();
    }

    delay(LOOP_DELAY_MS);
}
