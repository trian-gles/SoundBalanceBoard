#pragma once

#include <Arduino.h>
#include <HX71708_ADC.h>

namespace SignalProcessing {

struct PolarCoordinates {
    long radius;
    float angle;
};

PolarCoordinates calculate_polar_balance(
    HX71708 sensors[],
    long presence_threshold,
    long balance_max
);
} // namespace SignalProcessing
