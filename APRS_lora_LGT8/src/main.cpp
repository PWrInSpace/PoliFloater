#include "fcn.h"

#define WAIT_TIME 73214

TinyGPSPlus gps;
GpsData gpsData, oldGpsData;
uint32_t frameTimer = WAIT_TIME;
SoftwareSerial Serial1 = SoftwareSerial(GPS_TX_PIN, GPS_RX_PIN);

void setup() {

    Serial.begin(57600);
    Serial1.begin(9600);

    loraInit();
    Serial1.println("$PMTK886,3*2B");

    delay(100);

    Serial.println("dalej");
}

void loop() {

    if (Serial1.available()) {

        char c = Serial1.read();
        //Serial.print(c);
        if(gps.encode(c)) {

            gpsData.lat = gps.location.lat();
            gpsData.lng = gps.location.lng();
            gpsData.alt = gps.altitude.feet();
            gpsData.speed = gps.speed.knots();

            Serial.println(gps.satellites.value());

            if (millis() > 2500 && gpsData.lat > 1) {

                if (oldGpsData.lat < 1) oldGpsData = gpsData;

                if (isPoland(gpsData)) loraSetConditions(434855000, 9, 7);
                else loraSetConditions(433775000, 12, 5);
                delay(100);

                frameTimer = millis();
                String txStr = createFrame(gpsData, oldGpsData);
                Serial.println(txStr);
                loraSend(txStr);

                delay(6000);
                LoRa.setTxPower(14);
                delay(1000);
                loraSend(txStr);
                delay(100);

                LoRa.sleep();

                oldGpsData = gpsData;
                delay(WAIT_TIME);
            }
        }
    }
    delay(1);
}
