#include <Arduino.h>
#include "M16.h"
#include <HX71708.h> // Die HX71708 Bibliothek für den 24-Bit A/D-Wandler
#include "Osc.h"
#include "Gain.h"
#include "app_config.h"
#include "sensor_runtime.h"
#include "signal_processing.h"
#include "SVF.h"

SVF lowCut1;
SVF lowCut2;
Osc aOsc1; // declare an instance of the oscillator class
Osc aOsc2; // declare an instance of the oscillator class
Gain outputGain(1000); // 0 = silent, 1024 = full level
unsigned long msNow = millis(); // current time since starting in milliseconds
unsigned long pitchTime = msNow; // time for the next pitch update
unsigned long pitchDelta = 1000; // time between pitch updates




// Array von Sensor-Objekten
HX71708 sensors[AppConfig::NUM_SENSORS] = {
    HX71708(AppConfig::SCK_PINS[0], AppConfig::DOUT_PINS[0]),
    HX71708(AppConfig::SCK_PINS[1], AppConfig::DOUT_PINS[1]),
    HX71708(AppConfig::SCK_PINS[2], AppConfig::DOUT_PINS[2]),
    HX71708(AppConfig::SCK_PINS[3], AppConfig::DOUT_PINS[3])
};


//Pins
// #define I2S_BCK 12
// #define I2S_DATA 11
// #define I2S_LRCK 10

#define I2S_BCK   47  // physical pin D12
#define I2S_DATA  38  // physical pin D11
#define I2S_LRCK  21  // physical pin D10

void initializeAudioOut()
{
    seti2sPins(I2S_BCK, I2S_LRCK, I2S_DATA, -1);

    delay(200); // let the Serial port get established
    aOsc1.sinGen(); // initialise the osc's built-in wavetable with a sine waveform
    aOsc2.sinGen();
    aOsc1.setPitch(31); // MIDI pitch
    aOsc2.setPitch(31); // MIDI pitch
    lowCut1.setFreq(90);
    lowCut1.setRes(0.1);
    lowCut2.setFreq(90);
    lowCut2.setRes(0.1);
    audioStart();
}

SignalProcessing::PolarCoordinates sensor_processings(
    HX71708 sensors[]
) {
    const SensorValues gram_sensor_values{
        sensors[0].get_last_gram_value(),
        sensors[1].get_last_gram_value(),
        sensors[3].get_last_gram_value(),
        sensors[2].get_last_gram_value()
    };

    if (SensorRuntime::is_board_idle(sensors)) {
        SensorRuntime::compensate_drift(sensors);
        return {0, 0.0f};
    }

    const SensorValues normalized_sensor_values =
        gram_sensor_values.normalize(AppConfig::BALANCE_MAX);
        
    const SignalProcessing::PolarCoordinates polar_coordinates =
        SignalProcessing::calculate_polar_balance(
            normalized_sensor_values,
            AppConfig::BALANCE_MAX
        );

    /*if (SensorRuntime::detect_stuck_board(
            sensors,
            polar_coordinates.radius
        )) {
        return {0, 0.0f};
    }*/

    

    return polar_coordinates;
}

void setup() {
    Serial.begin(115200);
    delay(500);
    
    Serial.println("Initialisiere Audio...");
    initializeAudioOut(); // Initialize audio output

    Serial.println("Initialisiere HX71708 ADCs...");
    SensorRuntime::initialize_sensors(sensors, AppConfig::NUM_SENSORS);
    delay(1000); // short delay to allow sensors to stabilize
    SensorRuntime::tare_all_sensors(
        sensors
    );
    Serial.println("HX71708 ADCs initialisiert.");    
    Serial.println("Initialisierung abgeschlossen.");
}

int count = 0;

void loop() {
    count++;
    //play(); // Play square wave (example)
    SensorRuntime::read_sensors(sensors);

    const SignalProcessing::PolarCoordinates polarCoordinates =
        sensor_processings(sensors);
    // Print the polar coordinates to the Serial Monitor for debugging
    Serial.println("Radius: " + String(polarCoordinates.radius) + ", Angle: " + String(polarCoordinates.angle));
        pitchTime += pitchDelta;
        float phase = TWO_PI * ((float)(count % 20000))/ 10000.0;
        float pitch = 32 + sin(phase);
        Serial.println(pitch);
        aOsc1.setPitch(pitch);
}

    pitchTime += pitchDelta;
    float phase = TWO_PI * ((float)(count % 20000))/ 10000.0;
    float pitch = 49 + sin(phase);
    Serial.println(pitch);
    aOsc1.setPitch(pitch);
}

void audioUpdate() {
    int32_t vOsc1 = aOsc1.next();
    int32_t vOsc2 = aOsc2.next();
    int32_t leftVal = lowCut1.nextHPF(outputGain.next(vOsc1 * 1.8 + vOsc2 * 0.6));
    int32_t rightVal = lowCut2.nextHPF(outputGain.next(vOsc2 * 1.8 + vOsc1 * 0.6));
  audioBlockWrite(leftVal, rightVal);
}
