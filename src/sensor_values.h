#pragma once

#include <Arduino.h>
#include "app_config.h"

/**
 * simples struct to hold the values of all four sensors in grams
 * 
 */


struct SensorValues {
    long north;
    long east;
    long south;
    long west;

    SensorValues normalize(long maximum) const {
        if (maximum <= 0) {
            return {0, 0, 0, 0};
        }

        const long positive_north = north > 0 ? north : 0;
        const long positive_east = east > 0 ? east : 0;
        const long positive_south = south > 0 ? south : 0;
        const long positive_west = west > 0 ? west : 0;
        const long total =
            positive_north +
            positive_east +
            positive_south +
            positive_west;

        if (total == 0) {
            return {0, 0, 0, 0};
        }

        return {
            static_cast<long>(
                static_cast<int64_t>(positive_north) * maximum / total
            ),
            static_cast<long>(
                static_cast<int64_t>(positive_east) * maximum / total
            ),
            static_cast<long>(
                static_cast<int64_t>(positive_south) * maximum / total
            ),
            static_cast<long>(
                static_cast<int64_t>(positive_west) * maximum / total
            )
        };
    }
};
