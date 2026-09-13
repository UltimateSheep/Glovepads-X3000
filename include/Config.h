#pragma once
// ============================================================================
// Config.h
//
// EVERY hardware-specific or "tunable" value in this project lives here.
// The other modules (ButtonManager, JoystickManager, BatteryManager,
// GamepadController) never contain a hardcoded pin number or magic value --
// they just read from this file. To customize the gamepad, this is the only
// file you should need to touch.
// ============================================================================

#include <Arduino.h>
#include <BleGamepad.h>   // pulls in the BUTTON_1..BUTTON_128 constants

// ----------------------------------------------------------------------------
// DEBUG
// Flip to false before "shipping" the gamepad to remove all Serial overhead.
// ----------------------------------------------------------------------------
#define DEBUG_ENABLED     true
#define SERIAL_BAUD_RATE  115200

#if DEBUG_ENABLED
    #define DEBUG_PRINT(...)    Serial.print(__VA_ARGS__)
    #define DEBUG_PRINTLN(...)  Serial.println(__VA_ARGS__)
    #define DEBUG_PRINTF(...)   Serial.printf(__VA_ARGS__)
#else
    #define DEBUG_PRINT(...)
    #define DEBUG_PRINTLN(...)
    #define DEBUG_PRINTF(...)
#endif

// Main loop pacing. BLE reports are sent once per loop while connected, so
// this is effectively your report rate (10ms = up to ~100 reports/sec).
static const uint32_t LOOP_DELAY_MS = 10;

// ----------------------------------------------------------------------------
// BLUETOOTH IDENTITY
// ----------------------------------------------------------------------------
static const char *BLE_DEVICE_NAME         = "ESP32-C3 Gamepad";
static const char *BLE_DEVICE_MANUFACTURER = "DIY Electronics";

// ----------------------------------------------------------------------------
// BUTTONS
//
// Wiring assumed: each button connects its GPIO straight to GND. Internal
// pull-ups are enabled in firmware, so no external resistors are needed.
//
// TO ADD / REMOVE / REMAP A BUTTON: edit this table only. Everything else
// (debouncing, BLE report building, button count sent to the HID
// descriptor) derives from it automatically.
//
//   pin           -> GPIO the physical button is wired to
//   gamepadButton -> BUTTON_1 .. BUTTON_128 constant reported over BLE
//   activeLow     -> true = pressed reads LOW (matches INPUT_PULLUP wiring)
// ----------------------------------------------------------------------------
struct ButtonMapping {
    uint8_t pin;
    uint8_t gamepadButton;
    bool    activeLow;
};

static const ButtonMapping BUTTON_MAP[] = {
    { 5, BUTTON_1, true },  // Button 1
    { 6, BUTTON_2, true },  // Button 2
    { 7, BUTTON_3, true },  // Button 3
    { 8, BUTTON_4, true },  // Button 4 -- GPIO8 is a strapping pin, see README
    { 9, BUTTON_5, true },  // Button 5 / joystick click -- GPIO9 is the BOOT
                            // strapping pin, see README
};
static const uint8_t BUTTON_COUNT = sizeof(BUTTON_MAP) / sizeof(BUTTON_MAP[0]);

static const uint32_t DEBOUNCE_DELAY_MS = 15;

// ----------------------------------------------------------------------------
// ANALOG JOYSTICK
//
// ADC1_CH4 = GPIO4 (X), ADC1_CH3 = GPIO3 (Y), per the ESP32-C3 ADC1 mapping.
// ----------------------------------------------------------------------------
static const uint8_t JOYSTICK_X_PIN = 4;
static const uint8_t JOYSTICK_Y_PIN = 3;

// Flip either of these to true if an axis reads backwards on your stick.
static const bool JOYSTICK_INVERT_X = false;
static const bool JOYSTICK_INVERT_Y = false;

static const int ADC_RESOLUTION_BITS = 12;      // 0..4095
static const int ADC_MIN_RAW         = 0;
static const int ADC_MAX_RAW         = 4095;

// Raw ADC counts around center that get reported as "0" -- absorbs stick
// noise / imperfect centering. Raise this if the stick reports drift when
// released, lower it if it feels unresponsive near center.
static const uint16_t JOYSTICK_DEADZONE = 120;

// Samples averaged at boot to find "center". Keep the stick untouched
// while the board starts up.
static const uint16_t JOYSTICK_CALIBRATION_SAMPLES = 200;

// Output range sent over BLE HID (signed, centered on 0).
static const int16_t GAMEPAD_AXIS_MIN = -32767;
static const int16_t GAMEPAD_AXIS_MAX =  32767;

// ----------------------------------------------------------------------------
// BATTERY MONITORING (optional feature)
//
// Your schematic didn't include a battery-voltage sense pin, so this
// defaults to GPIO0 (ADC1_CH0), which is otherwise unused. Wire a simple
// 2:1 divider from BAT+ to GND (e.g. two 100k resistors in series) with
// the midpoint going to this pin, so a 4.2V-charged cell shows ~2.1V here
// -- safely inside the ESP32's 0-3.3V ADC range.
//
// If you don't want to wire that up, set BATTERY_MONITORING_ENABLED to
// false and the gamepad will simply skip reporting a battery level.
// ----------------------------------------------------------------------------
static const bool    BATTERY_MONITORING_ENABLED    = true;
static const uint8_t BATTERY_ADC_PIN               = 0;

static const float   BATTERY_VOLTAGE_DIVIDER_RATIO = 2.0f;   // (R1+R2)/R2
static const float   BATTERY_EMPTY_VOLTAGE         = 3.30f;  // -> reports 0%
static const float   BATTERY_FULL_VOLTAGE          = 4.20f;  // -> reports 100%

static const uint8_t  BATTERY_OVERSAMPLE_COUNT     = 16;     // per reading
static const uint32_t BATTERY_UPDATE_INTERVAL_MS   = 30000;  // how often to re-check + notify
static const float    BATTERY_SMOOTHING_ALPHA      = 0.3f;   // 0..1, lower = smoother/slower
