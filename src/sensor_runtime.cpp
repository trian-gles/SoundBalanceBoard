#include "sensor_runtime.h"

namespace SensorRuntime {

    void _warm_up_sensors(HX71708 sensors[], uint8_t num_sensors, uint16_t warmup_reads) {
        Serial.println("Warmup vor Tare...");
        for (uint8_t i = 0; i < num_sensors; i++) {
            for (uint16_t reading = 0; reading < warmup_reads; reading++) {
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

    void read_sensors(HX71708 sensors[], uint8_t num_sensors) {
        if (sensors == nullptr) {
            return;
        }

        for (uint8_t i = 0; i < num_sensors; ++i) {
            sensors[i].read_grams();
        }
    }

    bool is_board_idle(
        const HX71708 sensors[],
        long presence_threshold,
        uint8_t num_sensors
    ) {
        if (sensors == nullptr) {
            return true;
        }

        int64_t total_load = 0;
        for (uint8_t i = 0; i < num_sensors; ++i) {
            const long sensor_value = sensors[i].get_last_gram_value();
            if (sensor_value > 0) {
                total_load += sensor_value;
            }
        }

        return total_load <= presence_threshold;
    }

    void compensate_drift(
        HX71708 sensors[],
        long presence_threshold,
        uint8_t num_sensors
    ) {
        if (sensors == nullptr) {
            return;
        }

        for (uint8_t i = 0; i < num_sensors; ++i) {
            sensors[i].drift_correction(presence_threshold);
        }
    }

    bool detect_stuck_board(
        HX71708 sensors[],
        long radius,
        uint8_t num_sensors
    ) {
        if (sensors == nullptr) {
            return false;
        }

        static long previous_radius = 0;
        static unsigned long candidate_since = 0;
        static bool initialized = false;
        const unsigned long now = millis();

        if (!initialized) {
            previous_radius = radius;
            initialized = true;
            return false;
        }

        const long radius_change = labs(radius - previous_radius);
        const bool is_candidate =
            radius > 0 &&
            radius_change <= AppConfig::STUCK_BOARD_RADIUS_TOLERANCE;

        if (!is_candidate) {
            candidate_since = 0;
        } else if (candidate_since == 0) {
            candidate_since = now;
        } else if (now - candidate_since >= AppConfig::STUCK_BOARD_TIME_MS) {
            Serial.println("WARN: Statischer Balance-Radius erkannt, fuehre schnellen Tare aus.");
            for (uint8_t i = 0; i < num_sensors; ++i) {
                sensors[i].simple_tare(AppConfig::QUICK_TARE_SAMPLES);
            }
            candidate_since = 0;
            previous_radius = 0;
            return true;
        }

        previous_radius = radius;
        return false;
    }
}
