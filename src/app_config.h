#pragma once

#include <Arduino.h>
//all configuration constants for the project are in this namespace

namespace AppConfig {

constexpr uint8_t NUM_SENSORS = 4;

constexpr uint8_t SCK_PINS[NUM_SENSORS] = {3, 5, 7, 9};
constexpr uint8_t DOUT_PINS[NUM_SENSORS] = {2, 4, 6, 8};

//Threshold if weight is on board in grams
constexpr long PRESENCE_THRESHOLD = 10000;
// maximum distance to mass center
constexpr long BALANCE_MAX = 512;

constexpr float SCALE_FACTOR= 0.0186f; // scale factor to convert raw counts to grams

// Board-level stuck detection based on the normalized polar radius.
constexpr long STUCK_BOARD_RADIUS_TOLERANCE = 15;
constexpr uint32_t STUCK_BOARD_TIME_MS = 3000;
constexpr uint16_t QUICK_TARE_SAMPLES = 128;

// Drift-compensation settings
constexpr uint16_t DRIFT_IDLE_TIME_MS = 500;      // 0.5s without load → drift correction
constexpr float DRIFT_WEIGHT_FACTOR = 1.0f;         // 20% new offset, 80% old
constexpr uint16_t DRIFT_COOLDOWN_MS = 1000;        // Minimum 1s between re-tare attempts
constexpr uint16_t DRIFT_UPDATE_INTERVAL_MS = 1000; // Check drift every 1s

}