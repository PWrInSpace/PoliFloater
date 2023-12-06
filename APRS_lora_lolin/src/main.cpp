#include "fcn.h"

#define WAIT_TIME 120000

TinyGPSPlus gps;
GpsData gpsData;
uint32_t frameTimer = WAIT_TIME;

void setup() {

    Serial.begin(115200);
    Serial1.begin(9600, SERIAL_8N1, GPS_TX_PIN, GPS_RX_PIN);
    pinMode(BLUE_PIN, OUTPUT);
    pinMode(RED_PIN, OUTPUT);

    loraInit();
}

void loop() {

    if (Serial1.available()) {

        char c = Serial1.read();
        if(gps.encode(c)) {

            gpsData.lat = gps.location.lat();
            gpsData.lng = gps.location.lng();
            gpsData.alt = gps.altitude.meters();

            if (gpsData.lat > 1 && millis() - frameTimer > WAIT_TIME) {

                digitalWrite(RED_PIN, 1);
                frameTimer = millis();
                String txStr = createFrame(gpsData);
                Serial.println(txStr);
                loraSend(txStr);
                digitalWrite(RED_PIN, 0);

                digitalWrite(BLUE_PIN, 1);
                vTaskDelay(2000 / portTICK_PERIOD_MS);
                digitalWrite(BLUE_PIN, 0);
            }
        }
    }
    vTaskDelay(1 / portTICK_PERIOD_MS);
}
