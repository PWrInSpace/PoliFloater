#ifndef FCN_H
#define FCN_H

#include "pinout.h"
#include <Arduino.h>
#include <TinyGPS++.h>
#include <SPI.h>
#include <LoRa.h>
#include <EEPROM.h>
#include "gpsData.h"

void loraInit();
void loraSetConditions(uint32_t freq, uint8_t sf, uint8_t cr);
void loraSend(String txString);
uint16_t calculateAngle(GpsData gps1, GpsData gps2);
String createFrame(GpsData gpsData, GpsData oldGpsData);
void goToSleep(uint16_t seconds);
float getVoltage();
bool isDistanceEnough(GpsData gpsData, GpsData oldGpsData);

#endif
