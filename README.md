# ESP32-C3 Super Mini -- BLE Gamepad
###### Disclaimer: This README file has been AI-generated.

A modular PlatformIO firmware that turns an ESP32-C3 Super Mini into a
Bluetooth LE HID gamepad: 5 buttons + 1 analog stick, plus battery-level
reporting to the host. Built to be easy to read and easy to debug, with
every pin/tunable value centralized in one file.

## Project layout

```
include/
  Config.h              <- ALL pins, button mapping, calibration, timing live here
  ButtonManager.h
  JoystickManager.h
  BatteryManager.h
  GamepadController.h
src/
  main.cpp              <- just wires the modules together
  ButtonManager.cpp      <- debounced digital button reading
  JoystickManager.cpp    <- ADC reading, auto-center calibration, deadzone
  BatteryManager.cpp     <- battery voltage -> percentage
  GamepadController.cpp  <- the only file that touches the BLE library
platformio.ini
```

Each module only knows about its own hardware; `GamepadController` is the
single place that turns that state into a BLE HID report. To change
*anything* about the button layout or joystick behavior you only need to
edit `Config.h`.

## Wiring (as specified)

| Signal              | ESP32-C3 pin       |
|---------------------|--------------------|
| Button 1            | GPIO5              |
| Button 2            | GPIO6              |
| Button 3            | GPIO7              |
| Button 4            | GPIO8              |
| Button 5 (stick click) | GPIO9           |
| Joystick X          | GPIO4 (ADC1_CH4)   |
| Joystick Y          | GPIO3 (ADC1_CH3)   |
| Board power         | 5V/GND from USB-C module (fed by LiPo) |

Buttons are wired GPIO -> switch -> GND; firmware uses internal pull-ups,
so no external resistors are needed.

### One thing not in the original schematic: battery-percentage sensing

A USB-C/charger module gives the board power, but reporting an actual
**battery percentage** to the host requires the firmware to measure the
battery voltage itself, and no sense pin was specified for that. This
firmware defaults to **GPIO0 (ADC1_CH0)**, which was otherwise unused:

```
BAT+ --[100k]--+--[100k]-- GND
               |
             GPIO0
```

That 2:1 divider brings a 4.2V-charged cell down to ~2.1V, safely inside
the ESP32's ADC range. If you don't want to add this, set
`BATTERY_MONITORING_ENABLED` to `false` in `Config.h` and the firmware
just won't send battery updates.

## Hardware notes worth knowing

- **GPIO8 and GPIO9 are strapping pins.** GPIO9 in particular is wired to
  the Super Mini's own BOOT button on most of these boards. Your external
  buttons just add a second switch to the same node, which is fine --
  just don't power on or reset the board while physically holding
  button 4 or button 5, since that can change the boot mode.
- **GPIO8 often drives the onboard status LED** on Super Mini boards. You
  may see the LED flicker when button 4 is pressed -- that's expected and
  harmless, just a side effect of sharing the pin.
- **ESP32-C3 is BLE-only** (no classic Bluetooth/BR-EDR), so this shows up
  to the host as a Bluetooth *Low Energy* HID gamepad. Windows 10/11,
  modern Android, iOS/iPadOS, macOS, and Linux (BlueZ) all support this
  natively.

## Customizing

**Remap or add a button** -- edit the `BUTTON_MAP` array in `Config.h`:
```cpp
static const ButtonMapping BUTTON_MAP[] = {
    { 5, BUTTON_1, true },
    { 6, BUTTON_2, true },
    // add a new row here, e.g.: { 10, BUTTON_6, true },
};
```
Nothing else needs to change -- the button count sent to the host and the
debounce loop both derive from this array's length.

**Joystick feels off** -- also in `Config.h`:
- `JOYSTICK_DEADZONE` -- raise it if the stick drifts near center, lower it
  if it feels unresponsive.
- `JOYSTICK_INVERT_X` / `JOYSTICK_INVERT_Y` -- flip if an axis moves
  backwards from what you expect.
- Keep the stick centered/untouched for the first second after power-up --
  that's when `JoystickManager` samples the resting position.

**Battery curve** -- `BATTERY_EMPTY_VOLTAGE` / `BATTERY_FULL_VOLTAGE` set
where 0%/100% land. 3.30V/4.20V are typical single-cell LiPo values but
adjust to taste; the mapping is a straight line, not a true discharge
curve, which is normal for a simple project like this.

## Debugging

Everything logs over Serial (115200 baud) when `DEBUG_ENABLED` is `true`
in `Config.h`: button presses/releases, the joystick's calibrated center,
and periodic battery readings. Set it to `false` once things work to
remove the overhead.

## Building & flashing

```
pio run -t upload
pio device monitor
```

If the upload doesn't start automatically (common on some C3 boards):
hold **BOOT**, tap **RESET**, release **BOOT**, then retry the upload.

## Pairing

Power on the board, then look for **"ESP32-C3 Gamepad"** in the host's
Bluetooth device list like any other BLE gamepad. No app or code changes
are needed on the host side.
