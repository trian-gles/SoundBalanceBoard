#pragma once

#include <Arduino.h>
#include "HX71708.h"
#include "app_config.h"
//all startup-functions for the HX71708 ADCs are in this namespace

namespace SensorRuntime {

    /**
     * @brief reads all sensors for a warm-up phase before taring
     * @param sensors Array of sensor objects
     * @param num_sensors Number of sensors
     * @param warmup_reads Number of readings to perform for warm-up
     */
void _warm_up_sensors(HX71708 sensors[], uint8_t num_sensors = AppConfig::NUM_SENSORS, uint8_t warmup_reads = 1280);

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
     */
void read_sensors(HX71708 sensors[]);

    /**
     * @brief Updates the idle states of all sensors based on the presence threshold
     * @param sensors Array of sensor objects
     * @param presence_threshold Threshold to determine if there is a load present
     */
void update_idle_states(
    HX71708 sensors[],
    long presence_threshold = AppConfig::PRESENCE_THRESHOLD
);

}
