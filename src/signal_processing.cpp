#include "signal_processing.h"

#include <math.h>

namespace SignalProcessing {

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
) {
    if (balance_max <= 0) {
        return {0, 0.0f};
    }

    const long total_load =
        labs(sensor_values.north) +
        labs(sensor_values.east) +
        labs(sensor_values.south) +
        labs(sensor_values.west);

    if (total_load <= presence_threshold) {
        return {0, 0.0f};
    }

    const long x_load =
        (sensor_values.east + sensor_values.south) -
        (sensor_values.north + sensor_values.west);
    const long y_load =
        (sensor_values.north + sensor_values.east) -
        (sensor_values.west + sensor_values.south);

    const float normalized_x =
        static_cast<float>(x_load) / total_load;
    const float normalized_y =
        static_cast<float>(y_load) / total_load;
    const float normalized_radius =
        sqrtf(normalized_x * normalized_x + normalized_y * normalized_y);
    const long radius = static_cast<long>(
        normalized_radius * balance_max
    );

    return {
        radius > balance_max ? balance_max : radius,
        atan2f(normalized_y, normalized_x)
    };
}

} // namespace SignalProcessing
