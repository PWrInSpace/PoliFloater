#include "gpsData.h"

bool GpsData::isInSquare(float N, float S, float W, float E) {

    return (
        this->lat > S && this->lat < N &&
        this->lng > W && this->lng < E
    );
}

/************************************************************* */

bool GpsData::isPoland() {

    const float poland_N = 54.8;
    const float poland_S = 50.4;
    const float poland_W = 14.5;
    const float poland_E = 24;

    return isInSquare(poland_N, poland_S, poland_W, poland_E);
}