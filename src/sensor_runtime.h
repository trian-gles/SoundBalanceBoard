#pragma once

#include <Arduino.h>
#include "HX71708.h"
#include "app_config.h"
#include "sensor_values.h"
//all startup-functions for the HX71708 ADCs are in this namespace

namespace SensorRuntime {

    /**
     * @brief reads all sensors for a warm-up phase before taring
     * @param sensors Array of sensor objects
     * @param num_sensors Number of sensors
     * @param warmup_reads Number of readings to perform for warm-up
     */
void _warm_up_sensors(HX71708 sensors[], uint8_t num_sensors = AppConfig::NUM_SENSORS, uint16_t warmup_reads = 1280);

    /**
     * @brief Initializes all sensors
     * @param sensors Array of sensor objects
     * @param num_sensors Number of sensors
     */
void initialize_sensors(HX71708 sensors[], uint8_t num_sensors = AppConfig::NUM_SENSORS);

    /**
     * @brief tares all sensors
     * @param sensors Array of sensor objects
     * @param num_sensors Number of sensors
     */
void tare_all_sensors(
    HX71708 sensors[],
    uint8_t num_sensors = AppConfig::NUM_SENSORS
);

    /**
     * @brief Reads all sensors
     * @param sensors Array of sensor objects
     * @param num_sensors Number of sensors
     */
void read_sensors(HX71708 sensors[], uint8_t num_sensors = AppConfig::NUM_SENSORS);

    /**
     * @brief Checks whether the complete board is idle based on the total load.
     * @param sensors Array of sensor objects with current readings
     * @param presence_threshold Minimum total load in grams considered as presence
     * @return true if the total load is at or below the threshold
     */
    bool is_board_idle(
        const HX71708 sensors[],
        long presence_threshold = AppConfig::PRESENCE_THRESHOLD,
        uint8_t num_sensors = AppConfig::NUM_SENSORS);

    /**
     * @brief Applies drift compensation to all sensors
     * @param sensors Array of sensor objects
     * @param presence_threshold Threshold used by the sensor drift logic
     */
void compensate_drift(
    HX71708 sensors[],
    long presence_threshold = AppConfig::PRESENCE_THRESHOLD
);
}
