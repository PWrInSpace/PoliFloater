#ifndef FCN_H
#define FCN_H

#include "pinout.h"
#include <Arduino.h>
#include <TinyGPS++.h>
#include <SPI.h>
#include <LoRa.h>

struct GpsData {

    float lat;
    float lng;
    float alt;
};

void loraInit();
void loraSetConditions(uint32_t freq, uint8_t sf, uint8_t cr);
void loraSend(String txString);
String createFrame(GpsData gpsData);

#endif
