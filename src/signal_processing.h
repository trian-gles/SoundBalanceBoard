#pragma once

#include <Arduino.h>

namespace SignalProcessing {

    /**
     * @brief Struct to hold the latest read values of all four sensors
     */
struct SensorValues {
    long north;
    long east;
    long south;
    long west;
};

/**
 * @brief Struct to hold the polar coordinates (radius and angle) of the balance
 */
struct PolarCoordinates {
    long radius;
    float angle;
};

/**
 * @brief calculates the polar coordinates (radius and angle) of the balance based on sensor values
 * @param sensor_values latest corrected values of all four sensors
 * @param presence_threshold threshold to determine if there is a load present
 * @param balance_max maximum value for the balance
 * @return the polar coordinates (radius and angle) of the balance
 */
PolarCoordinates calculate_polar_balance(
    const SensorValues& sensor_values,
    long presence_threshold,
    long balance_max
);
} // namespace SignalProcessing
