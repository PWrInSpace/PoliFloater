#include "fcn.h"

#define WAIT_TIME 52000

TinyGPSPlus gps;
GpsData gpsData, oldGpsData;
uint32_t frameTimer = WAIT_TIME;
SoftwareSerial Serial1 = SoftwareSerial(GPS_TX_PIN, GPS_RX_PIN);

void setup() {

    Serial.begin(57600);
    Serial1.begin(9600);

    loraInit();
    Serial1.println("$PMTK886,3*2B");
    pinMode(A0, INPUT);

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

            if (gpsData.lat > 1 && millis() - frameTimer > WAIT_TIME) {

                if (oldGpsData.lat < 1) oldGpsData = gpsData;

                loraSetConditions(434855000, 9, 7);
                delay(100);

                frameTimer = millis();
                String txStr = createFrame(gpsData, oldGpsData);
                Serial.println(txStr);
                loraSend(txStr);

                oldGpsData = gpsData;
            }
        }
    }
    delay(1);
}
