#pragma once

#include <Arduino.h>
#include <HX71708_ADC.h>
#include "app_config.h"
//all startup-functions for the HX71708 ADCs are in this namespace

namespace SensorRuntime {

    /**
     * @brief Initializes all sensors
     * @param sensors Array of sensor objects
     * @param num_sensors Number of sensors
     */
void initialize_sensors(HX71708 sensors[], uint8_t num_sensors = AppConfig::NUM_SENSORS);

    /**
     * @brief does a few reads to warm up all sensors
     * @param sensors Array of sensor objects
     * @param num_sensors Number of sensors
     * @param warmup_reads Number of warmup reads
     */
void warm_up_sensors(HX71708 sensors[], uint8_t num_sensors = AppConfig::NUM_SENSORS, uint8_t warmup_reads = 8);

    /**
     * @brief Calibrates all sensors by performing a tare operation
     * @param sensors Array of sensor objects
     * @param num_sensors Number of sensors
     * @param pre_tare_settle_ms Time in milliseconds to wait before performing tare
     */
void calibrate_all_sensors(
    HX71708 sensors[],
    uint8_t num_sensors = AppConfig::NUM_SENSORS
);
}
