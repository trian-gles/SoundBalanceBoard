#ifndef HX71708_ADC_H
#define HX71708_ADC_H

#include <Arduino.h> 
#include "app_config.h"
/**
 * @brief A class to control and read the HX71708 24-bit A/D converter.
 */
class HX71708 {
private:

    // Pins
    int _pdSckPin; //clock-pin
    int _doutPin;  // data-pin
    // Offset from tare operation
    long _offset;
    //stores the last read value (offset-corrected and scaled)
    long _last_gram_value;
    // Flag to indicate if the last read operation timed out
    bool _last_read_timed_out;
    static constexpr unsigned long _READ_TIMEOUT_US = 50000;
    //Flag if sensor idles
    bool _idle;
    
    

    // Drift-compensation-state
    unsigned long _last_activity_time;
    unsigned long _last_update_time;
    unsigned long _last_tare_time;
    

    void _custom_nop_delay(void) {
        ((void)0);
    }
    
    /**
     * @brief performs a soft tare update to adjust the offset
     * @param weight_factor weighting factor for the new measurement (0.0-1.0). E.g. 0.2 = 20% new, 80% old.
     * @return void
     */
    void _soft_tare(float weight_factor = AppConfig::DRIFT_WEIGHT_FACTOR);
    

public:
    
    /**
     * @brief HX71708 constructor
     * @param pdSckPin clock-pin connected to PD_SCK of HX71708
     * @param doutPin data-pin connected to DOUT of HX71708
     *                
     */
    HX71708(int pdSckPin, int doutPin);
    /**
     * @brief initializes the HX71708 ADC after power-up
     */
    void begin(void);
    /**
     * @brief sets the offset of the sensor
     * @param offset the new offset value to set
     */
    void set_offset(long offset);
    /**
     * @brief returns offset of the sensor
     */
    long get_offset(void);
    /**
     * @brief returns the last read value (offset-corrected and scaled)
     */
    long get_last_gram_value(void) const;
    
    /**
     * @brief checks if the sensor is currently in an idle state (no load, extended period)
     * @return true if idle, false otherwise
     */
    bool is_idle() const;
    
    /**
     * @brief reads a 24-bit sensor value from the HX71708 ADC and sets the next data rate to 320Hz.
     *        The 24-bit output data is formatted in binary two's complement code,
     *        where the MSB is the sign bit.
     *        The conversion to an unsigned value is done by XOR with 0x800000.
    * @return The converted 24-bit value. In case of timeout, a safe fallback
    *         is returned and the ADC is reset briefly.
     */
    long read_raw(void);

    /**
     * @brief returns the offset-corrected sensor value and scales by the scale factor
     * @return Offset-corrected raw value as `long`.
     */
    long read_grams(void);
    
    /**
     * @brief determines the zero point of the sensor with multiple blocks of readings which are averaged 
     * and the median of the block means is used as the offset
     * @return void
     */
    void tare(void);

    /**
     * @brief performs a simple tare operation, averaging a few readings
     * @return void
     */
    void simple_tare(uint16_t num_samples = 640);

    /**
     * @brief checks the current sensor value and updates drift compensation as well as idle status.
     *        Should be called regularly (e.g., once per second)
     * @param presence_threshold Threshold above which the sensor is considered loaded.
     * @return void
     */
    void drift_correction(long presence_threshold);
};

#endif // HX71708_ADC_H