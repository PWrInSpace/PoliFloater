#ifndef GPSDATA_H
#define GPSDATA_H

#include <stdint.h>

class GpsData {

public:

    float lat;
    float lng;
    uint16_t alt;
    uint16_t speed;

    bool isInSquare(float N, float S, float W, float E);
    bool isPoland();
};

#endif