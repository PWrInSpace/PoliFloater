#include "fcn.h"

#define WAIT_TIME 60000

TinyGPSPlus gps;
GpsData gpsData, oldGpsData;
uint32_t frameTimer = WAIT_TIME;
bool isSecChannelTime;

void setup() {

    Serial.begin(115200);
    Serial1.begin(9600, SERIAL_8N1, GPS_TX_PIN, GPS_RX_PIN);
    pinMode(RED_PIN, OUTPUT);
    pinMode(GREEN_PIN, OUTPUT);
    pinMode(BLUE_PIN, OUTPUT);

    loraInit();
}

void loop() {

    if (Serial1.available()) {

        char c = Serial1.read();
        if(gps.encode(c)) {

            gpsData.lat = gps.location.lat();
            gpsData.lng = gps.location.lng();
            gpsData.alt = gps.altitude.feet();

            if (gpsData.lat > 1 && millis() - frameTimer > WAIT_TIME) {

                if (oldGpsData.lat < 1) oldGpsData = gpsData;

                if (isSecChannelTime) {

                    isSecChannelTime = false;
                    loraSetConditions(434855000, 9, 7);
                    digitalWrite(RED_PIN, 1);
                }
                else {

                    isSecChannelTime = true;
                    loraSetConditions(433775000, 12, 5);
                    digitalWrite(GREEN_PIN, 1);
                }

                vTaskDelay(2000 / portTICK_PERIOD_MS);
                digitalWrite(RED_PIN, 0);
                digitalWrite(GREEN_PIN, 0);

                digitalWrite(BLUE_PIN, 1);
                frameTimer = millis();
                String txStr = createFrame(gpsData, oldGpsData);
                Serial.println(txStr);
                loraSend(txStr);
                digitalWrite(BLUE_PIN, 0);

                oldGpsData = gpsData;
            }
        }
    }
    vTaskDelay(1 / portTICK_PERIOD_MS);
}
