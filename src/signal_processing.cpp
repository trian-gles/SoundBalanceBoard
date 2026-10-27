#include "signal_processing.h"

#include <math.h>

namespace SignalProcessing {

/**
 * @brief calculates the polar coordinates (radius and angle) of the balance based on sensor values
 * @param sensor_values latest corrected values of all four sensors
 * @param balance_max maximum value for the balance
 * @return the polar coordinates (radius and angle) of the balance
 */
PolarCoordinates calculate_polar_balance(
    const ::SensorValues& sensor_values,
    long balance_max
) {
    if (balance_max <= 0) {
        return {0, 0.0f};
    }

    const long total_load =
        sensor_values.north +
        sensor_values.east +
        sensor_values.south +
        sensor_values.west;

    if (total_load == 0) {
        return {0, 0.0f};
    }

    const long x_load =
        (sensor_values.east + sensor_values.south) -
        (sensor_values.north + sensor_values.west);
    const long y_load =
        (sensor_values.north + sensor_values.east) -
        (sensor_values.west + sensor_values.south);

    const float radius = sqrtf(
        static_cast<float>(x_load * x_load) +
        static_cast<float>(y_load * y_load)
    );

    return {
        static_cast<long>(radius > balance_max ? balance_max : radius),
        atan2f(static_cast<float>(y_load), static_cast<float>(x_load))
    };
}

} // namespace SignalProcessing
