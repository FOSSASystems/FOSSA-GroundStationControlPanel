#ifndef CSTRUCTS_H
#define CSTRUCTS_H

#include <cstdint>

struct ephemerides_t
{
    float solar_x;
    float solar_y;
    float solar_z;
    float magnetic_x;
    float magnetic_y;
    float magnetic_z;
    uint8_t controllerId;
};


struct sleep_interval_t
{
    int16_t firstSleepIntervalVoltageLevel;
    int16_t firstSleepIntervalLength;
    int16_t secondSleepIntervalLength;
    int16_t thirdSleepIntervalLength;
};


#endif // CSTRUCTS_H
