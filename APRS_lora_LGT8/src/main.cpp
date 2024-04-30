#include "fcn.h"

#define WAIT_TIME 52000

TinyGPSPlus gps;
GpsData gpsData, oldGpsData;
uint32_t frameTimer = WAIT_TIME;
bool isSecChannelTime;
SoftwareSerial Serial1 = SoftwareSerial(GPS_TX_PIN, GPS_RX_PIN);

void setup() {

    //Serial.begin(57600);
    Serial1.begin(9600);

    loraInit();
    Serial1.println("$PMTK886,3*2B");
    pinMode(LED_BUILTIN, OUTPUT);

    while (analogRead(A0) < 900) { 
        Serial.println(analogRead(A0));
        delay(1000);
    }

    digitalWrite(LED_BUILTIN, 1);
    delay(200);
    digitalWrite(LED_BUILTIN, 0);
}

void loop() {

    if (Serial1.available()) {

        char c = Serial1.read();
        Serial.print(c);
        if(gps.encode(c)) {

            gpsData.lat = gps.location.lat();
            gpsData.lng = gps.location.lng();
            gpsData.alt = gps.altitude.feet();
            gpsData.speed = gps.speed.knots();

            if (gpsData.lat > 1) {

                digitalWrite(LED_BUILTIN, 1);
                delay(200);
                digitalWrite(LED_BUILTIN, 0);
            }

            if (gpsData.lat > 1 && millis() - frameTimer > WAIT_TIME) {

                if (oldGpsData.lat < 1) oldGpsData = gpsData;

                if (isSecChannelTime) {

                    isSecChannelTime = false;
                    loraSetConditions(433775000, 12, 5);
                }
                else {

                    isSecChannelTime = true;
                    loraSetConditions(434855000, 9, 7);
                }

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
