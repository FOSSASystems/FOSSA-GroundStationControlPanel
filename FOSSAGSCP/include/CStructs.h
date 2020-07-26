#ifndef CSTRUCTS_H
#define CSTRUCTS_H

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

#endif // CSTRUCTS_H
