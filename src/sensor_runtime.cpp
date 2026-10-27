#include "sensor_runtime.h"
namespace SensorRuntime {

    void _warm_up_sensors(HX71708 sensors[], uint8_t num_sensors, uint8_t warmup_reads) {
        Serial.println("Warmup vor Tare...");
        for (uint8_t i = 0; i < num_sensors; i++) {
            for (uint8_t reading = 0; reading < warmup_reads; reading++) {
                sensors[i].read_raw();
                delay(5);
            }
        }
    }

    void initialize_sensors(HX71708 sensors[], uint8_t num_sensors) {
        for (uint8_t i = 0; i < num_sensors; i++) {
            Serial.print("Initialisiere Sensor ");
            Serial.println(i + 1);

            sensors[i].begin();
            delay(100);
        }
        _warm_up_sensors(sensors);
    }

    void read_sensors(HX71708 sensors[]) {
        if (sensors == nullptr) {
            return;
        }

        for (uint8_t i = 0; i < AppConfig::NUM_SENSORS; ++i) {
            sensors[i].read_grams();
        }
    }

    void update_idle_states(
        HX71708 sensors[],
        long presence_threshold
    ) {
        if (sensors == nullptr) {
            return;
        }

        for (uint8_t i = 0; i < AppConfig::NUM_SENSORS; ++i) {
            sensors[i].drift_correction(presence_threshold);
        }
    }
    
    void tare_all_sensors(
        HX71708 sensors[],
        uint8_t num_sensors
    ) {
        for (uint8_t i = 0; i < num_sensors; i++) {
            Serial.print("Kalibriere Sensor ");
            Serial.println(i + 1);

            delay(250); // wait for the sensor to settle before taring
            sensors[i].tare();
            Serial.print("Sensor ");
            Serial.print(i + 1);
            Serial.print(" Tare abgeschlossen. Offset: ");
            Serial.println(sensors[i].get_offset());
        }
    }
}
