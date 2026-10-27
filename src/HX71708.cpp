#include "HX71708.h" 
#include "app_config.h"

/**
 * @brief constructor for HX71708_ADC class
 * @param pdSckPin The GPIO pin connected to the PD_SCK pin of the HX71708.
 * @param doutPin The GPIO pin connected to the DOUT pin of the HX71708.
 */
HX71708::HX71708(int pdSckPin, int doutPin) {
    _pdSckPin = pdSckPin;
    _doutPin = doutPin;
    _offset = 0; // inital offset as 0
    _last_gram_value = 0;
    _last_read_timed_out = false;
    _last_activity_time = millis();
    _last_update_time = 0;
    _last_tare_time = 0;
    _idle = false;
}

void HX71708::begin(void) {
    //configure pins as OUTPUT/INPUT
    pinMode(_pdSckPin, OUTPUT);
    pinMode(_doutPin, INPUT);

    // reset the ADC by setting PD_SCK HIGH for 100us at startup
    digitalWrite(_pdSckPin, HIGH);
    delayMicroseconds(200); // A delay of 200us is greater than the required 100us
    digitalWrite(_pdSckPin, LOW);
    delay(100); // wait for at least 4 data cycles
}

long HX71708::get_offset(void) {
    return _offset;
}
void HX71708::set_offset(long offset) {
        _offset = offset;
    }

long HX71708::get_last_gram_value(void) const {
    return _last_gram_value;
}

bool HX71708::is_idle() const {
    return _idle;
}

void HX71708::tare() {
    constexpr int block_count = 8;
    constexpr int block_size = 40;
    constexpr int max_attempts_per_block = block_size * 6;
    long block_means[block_count] = {0};
    int valid_block_count = 0;

    auto collect_block_mean = [&](long &out_mean, int &out_valid_samples) -> bool {
        long block_sum = 0;
        int valid_samples = 0;
        int attempts = 0;

        while (valid_samples < block_size && attempts < max_attempts_per_block) {
            long sample = read_raw();
            attempts++;

            if (_last_read_timed_out) {
                // if a timeout occurred, we skip this sample and try again after a short delay to allow the ADC to recovers
                delay(20);
                continue;
            }

            block_sum += sample;
            valid_samples++;
            delay(10);
        }

        out_valid_samples = valid_samples;
        if (valid_samples == block_size) {
            out_mean = block_sum / block_size;
            return true;
        }
        return false;
    };

    auto sort_ascending = [&](long values[], int count) {
        for (int i = 1; i < count; i++) {
            long current_value = values[i];
            int position = i - 1;
            while (position >= 0 && values[position] > current_value) {
                values[position + 1] = values[position];
                position--;
            }
            values[position + 1] = current_value;
        }
    };

    for (int block = 0; block < block_count; block++) {
        long block_mean = 0;
        int valid_samples = 0;
        if (collect_block_mean(block_mean, valid_samples)) {
            block_means[valid_block_count] = block_mean;
            valid_block_count++;
        } else {
            Serial.print("WARN: Tare-Block verworfen (Sensor DOUT ");
            Serial.print(_doutPin);
            Serial.print(") valide Samples: ");
            Serial.print(valid_samples);
            Serial.print("/");
            Serial.println(block_size);
        }
    }

    if (valid_block_count == 0) {
        Serial.print("WARN: Tare abgebrochen, keine validen Samples an DOUT ");
        Serial.println(_doutPin);
        return;
    }

    sort_ascending(block_means, valid_block_count);

    int trim_per_side = (valid_block_count >= 5) ? 1 : 0;
    int start_index = trim_per_side;
    int end_index = valid_block_count - trim_per_side;

    int used_blocks = end_index - start_index;
    if (used_blocks <= 0) {
        Serial.print("WARN: Tare abgebrochen, keine gueltigen Bloecke nach Trim an DOUT ");
        Serial.println(_doutPin);
        return;
    }

    int median_index = start_index + (used_blocks / 2);
    if (used_blocks % 2 == 1) {
        _offset = block_means[median_index];
    } else {
        long lower = block_means[median_index - 1];
        long upper = block_means[median_index];
        _offset = (lower + upper) / 2;
    }
    Serial.print("INFO: Tare abgeschlossen an DOUT ");
    Serial.print(_doutPin);
    Serial.print(", Offset gesetzt auf ");
    Serial.println(_offset);
}

void HX71708::simple_tare() {
    constexpr int num_samples = 1280; // 640 samples for a simple tare
    long sum = 0;
    int valid_samples = 0;

    for (int i = 0; i < num_samples; i++) {
        long sample = read_raw();
        if (!_last_read_timed_out) {
            sum += sample;
            valid_samples++;
        }
        delay(10);
    }

    if (valid_samples > 0) {
        _offset = sum / valid_samples;
        Serial.print("INFO: Tare finished at DOUT ");
        Serial.print(_doutPin);
        Serial.print(", Offset: ");
        Serial.println(_offset);
    } else {
        Serial.print("WARN: Tare aborted, no valid samples at DOUT ");
        Serial.println(_doutPin);
    }
}


long HX71708::read_raw(void) {
    unsigned char i;
    unsigned long bcd = 0; // for storing the 24-bit internal code
    _last_read_timed_out = false;

    // set PD_SCK low to start the read process
    digitalWrite(_pdSckPin, LOW);

    const unsigned long wait_start = micros();
    while (digitalRead(_doutPin) == HIGH) {
        if (micros() - wait_start >= _READ_TIMEOUT_US) {
            _last_read_timed_out = true;
            digitalWrite(_pdSckPin, HIGH);
            delayMicroseconds(200);
            digitalWrite(_pdSckPin, LOW);
            return 0;
        }
        delayMicroseconds(2);
    }

    // delay after the falling edge of DOUT before the first PD_SCK pulse (T1 > 1us)
    delayMicroseconds(1);


    // readout of the 24 data bits. The HX71708 outputs MSB (Most Significant Bit) first
    for (i = 0; i < 24; i++) {
        digitalWrite(_pdSckPin, HIGH);
        delayMicroseconds(1);      // High-time T3 < 50us 
        digitalWrite(_pdSckPin, LOW);  // PD_SCK Low-level 
        delayMicroseconds(1);            // Low-time T4 > 0.2us 
        bcd = bcd << 1; // shifts the previously read value to the left to make room for the next bit
        if (digitalRead(_doutPin) == HIGH) {
            bcd++; // on dout high, set the current bit to 1
        }
    }
    // sends the additional clock pulses to select the next output data rate 
    // for 320Hz, a total of 28 clock pulses are required (24 data pulses + 4 additional pulses)
    for (i = 0; i < 4; i++) { // loop for N=4 additional clock pulses
        digitalWrite(_pdSckPin, HIGH);
        _custom_nop_delay();
        digitalWrite(_pdSckPin, LOW);
        _custom_nop_delay();
    }

    // conversion of the 24-bit value to an unsigned value.
    bcd = bcd ^ 0x800000;

    return bcd;
}

long HX71708::read_grams() {
    long rawValue = read_raw(); // Lese den Rohwert vom ADC
    _last_gram_value =
        (rawValue - _offset) * AppConfig::SCALE_FACTOR;
    return _last_gram_value;
}

void HX71708::_soft_tare(float weight_factor) {
    if (weight_factor <= 0.0f || weight_factor > 1.0f) {
        Serial.println("WARN: _soft_tare() weight_factor muss zwischen 0.0 und 1.0 sein");
        return;
    }

    long sample_sum = 0;
    int sample_count = 0;
    const int num_samples = 8;
    for (int i = 0; i < num_samples; i++) {
        long raw = read_raw();
        if (!_last_read_timed_out) {
            sample_sum += raw;
            sample_count++;
        }
        delay(5); 
    }

    if (sample_count == 0) {
        Serial.print("WARN: Soft-Tare abgebrochen, keine gueltigen Samples an DOUT ");
        Serial.println(_doutPin);
        return;
    }

    long new_sample = sample_sum / sample_count;

    // weighted offset
    float old_factor = 1.0f - weight_factor;
    long new_offset = (long)((_offset * old_factor) + (new_sample * weight_factor));


    _offset = new_offset;
}

void HX71708::drift_correction(long presence_threshold) {
    unsigned long now = millis();
    bool sensor_idle =
        (std::labs(_last_gram_value) <= presence_threshold);
    if (sensor_idle) {
        unsigned long idle_duration = now - _last_activity_time;
        _idle = true;

        // Wenn idle UND Cooldown abgelaufen: Trigger sanfte Korrektur
        if (_idle && idle_duration > AppConfig::DRIFT_IDLE_TIME_MS) {
            if ((now - _last_tare_time) > AppConfig::DRIFT_COOLDOWN_MS) {
                _soft_tare();
                _last_tare_time = now;
            }
        }
    } else {
        // System aktiv: Reset idle state
        if (_idle) {
            _idle = false;
        }
        _last_activity_time = now;
    }
}
