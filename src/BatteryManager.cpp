#include "BatteryManager.h"

void BatteryManager::begin() {
    if (!BATTERY_MONITORING_ENABLED) {
        DEBUG_PRINTLN("[Battery] Monitoring disabled in Config.h");
        return;
    }

    pinMode(BATTERY_ADC_PIN, INPUT);
    analogSetAttenuation(ADC_11db);

    _firstReading = true;
    sampleNow();
    _lastUpdate = millis();
    _reportDue  = true; // push the initial reading once BLE connects

    DEBUG_PRINTF("[Battery] Initial reading: %.2fV (%d%%)\n", _smoothedVoltage, _percentage);
}

void BatteryManager::sampleNow() {
    uint32_t sumMillivolts = 0;
    for (uint8_t i = 0; i < BATTERY_OVERSAMPLE_COUNT; i++) {
        sumMillivolts += analogReadMilliVolts(BATTERY_ADC_PIN);
    }
    float measuredVolts = (sumMillivolts / (float)BATTERY_OVERSAMPLE_COUNT) / 1000.0f;
    float batteryVolts  = measuredVolts * BATTERY_VOLTAGE_DIVIDER_RATIO;

    if (_firstReading) {
        _smoothedVoltage = batteryVolts;
        _firstReading    = false;
    } else {
        _smoothedVoltage = (BATTERY_SMOOTHING_ALPHA * batteryVolts) +
                           ((1.0f - BATTERY_SMOOTHING_ALPHA) * _smoothedVoltage);
    }

    float pct = (_smoothedVoltage - BATTERY_EMPTY_VOLTAGE) /
                (BATTERY_FULL_VOLTAGE - BATTERY_EMPTY_VOLTAGE) * 100.0f;
    pct = constrain(pct, 0.0f, 100.0f);
    _percentage = (uint8_t)(pct + 0.5f);
}

void BatteryManager::update() {
    if (!BATTERY_MONITORING_ENABLED) return;

    if (millis() - _lastUpdate >= BATTERY_UPDATE_INTERVAL_MS) {
        _lastUpdate = millis();
        sampleNow();
        _reportDue = true;
        DEBUG_PRINTF("[Battery] %.2fV -> %d%%\n", _smoothedVoltage, _percentage);
    }
}
