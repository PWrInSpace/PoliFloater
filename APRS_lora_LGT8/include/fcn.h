#ifndef FCN_H
#define FCN_H

#include "pinout.h"
#include <Arduino.h>
#include <TinyGPS++.h>
#include <SPI.h>
#include <LoRa.h>
#include <math.h>
#include <SoftwareSerial.h>

struct GpsData {

    float lat;
    float lng;
    uint16_t alt;
    uint16_t speed;
};

void loraInit();
void loraSetConditions(uint32_t freq, uint8_t sf, uint8_t cr);
void loraSend(String txString);
uint16_t calculateAngle(GpsData gps1, GpsData gps2);
String createFrame(GpsData gpsData, GpsData oldGpsData);
bool isPoland(GpsData gpsData);

#endif
