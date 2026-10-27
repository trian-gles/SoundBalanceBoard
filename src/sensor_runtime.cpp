#include "sensor_runtime.h"
namespace SensorRuntime {
    void initialize_sensors(HX71708 sensors[], uint8_t num_sensors) {
        for (uint8_t i = 0; i < num_sensors; i++) {
            Serial.print("Initialisiere Sensor ");
            Serial.println(i + 1);

            sensors[i].begin();
            delay(100);
        }
    }
    void warm_up_sensors(HX71708 sensors[], uint8_t num_sensors, uint8_t warmup_reads) {
        Serial.println("Warmup vor Tare...");
        for (uint8_t i = 0; i < AppConfig::NUM_SENSORS; i++) {
            for (uint8_t reading = 0; reading < warmup_reads; reading++) {
                sensors[i].read();
                delay(5);
            }
        }
    }
    void calibrate_all_sensors(
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
