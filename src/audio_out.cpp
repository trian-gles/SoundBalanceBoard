#include "i2s.h"
#include <I2S.h>
//Pins
#define I2S_BCK 12
#define I2S_DATA 11
#define I2S_LRCK 10
const int frequency = 440; // frequency of square wave in Hz

const int amplitude = 500; // amplitude of square wave

const int sampleRate = 44100; // sample rate in Hz

const int halfWavelength = (sampleRate / frequency) / 2; // half wavelength of square wave

short sample = amplitude; // current sample value
int count = 0;
    void initializeAudioOut()
    {
        I2S.setDataPin(I2S_DATA);
        I2S.setSckPin(I2S_BCK);
        I2S.setFsPin(I2S_LRCK);
        //I2S.setAllPins(I2S_BCK, I2S_LRCK, I2S_DATA, -1, -1); // Set the pins for I2S
        I2S.begin(I2S_PHILIPS_MODE, 44100, 16);
    }

    void playSquareWave()
    {
        if (count % halfWavelength == 0) {

            // invert the sample every half wavelength count multiple to generate square wave
            sample = -1 * sample;

        }

        I2S.write(sample);

        // increment the counter for the next sample

        count++;
    }
