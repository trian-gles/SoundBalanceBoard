#include "signal_processing.h"

#include <math.h>

namespace SignalProcessing {

/**
 * @brief calculates the polar coordinates (radius and angle) of the balance based on sensor values
 * @param sensor_values array of sensor values
 * @param presence_threshold threshold to determine if there is a load present
 * @param balance_max maximum value for the balance
 * @param num_sensors number of sensors (default is 4)
 * @return the polar coordinates (radius and angle) of the balance
 */
PolarCoordinates calculate_polar_balance(
    HX71708 sensors[],
    long presence_threshold,
    long balance_max
) {
    if (sensors == nullptr) {
        return {0, 0.0f};
    }

    long sensor_values[4] = {0, 0, 0, 0};
    for (uint8_t i = 0; i < 4; ++i) {
        sensor_values[i] = sensors[i].read_corrected();
        sensors[i].check_idle_and_soft_tare(
            sensor_values[i],
            presence_threshold
        );
    }

    const long north = sensor_values[0];
    const long east = sensor_values[1];
    const long south = sensor_values[2];
    const long west = sensor_values[3];
    const long total_load =
        labs(north) + labs(east) + labs(south) + labs(west);

    if (total_load <= presence_threshold) {
        return {0, 0.0f};
    }

    const long x_load =
        (east + south) - (north + west);
    const long y_load =
        (north + east) - (west + south);

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
