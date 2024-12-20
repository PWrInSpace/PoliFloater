#include "fcn.h"

#define WAIT_TIME_NO_FIX_S 20
#define WAIT_TIME_FIX_S 72
//#define DBG

TinyGPSPlus gps;
GpsData gpsData, oldGpsData;

void setup() {

    Serial.begin(115200);
    Serial1.begin(9600, SERIAL_8N1, GPS_TX_PIN, GPS_RX_PIN);

    #ifdef DBG
    for (uint8_t i = 0; i < 3; i++) {

        vTaskDelay(1000 / portTICK_PERIOD_MS);
        if (Serial) break;
    }
    #endif
    #ifndef DBG
    vTaskDelay(200 / portTICK_PERIOD_MS);
    #endif

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, 1);
    Serial1.println("$PMTK886,3*2B");
    EEPROM.begin(sizeof(GpsData));
    Serial.println(getVoltage());

    loraInit();
}

void loop() {

    if (Serial1.available()) {

        char c = Serial1.read();
        Serial.print(c);
        if(gps.encode(c)) {

            digitalWrite(LED_PIN, 0);
            gpsData.lat = gps.location.lat();
            gpsData.lng = gps.location.lng();
            gpsData.alt = gps.altitude.feet();
            gpsData.speed = gps.speed.knots();
            digitalWrite(LED_PIN, 1);

            uint16_t gpsMaxWait_ms = 2500;
            #ifdef DBG
            gpsMaxWait_ms = 7000;
            #endif

            if (millis() > gpsMaxWait_ms) {
                // There is GPS fix:
                if (gpsData.lat > 1) {

                    vTaskDelay(100 / portTICK_PERIOD_MS);
                    EEPROM.get(0, oldGpsData);

                    if (gpsData.isPoland() && !oldGpsData.lastWasPolish) {

                        loraSetConditions(434855000, 9, 7);
                        gpsData.lastWasPolish = true;
                    }
                    else {

                        loraSetConditions(433775000, 12, 5);
                        gpsData.lastWasPolish = false;
                    }
                    vTaskDelay(100 / portTICK_PERIOD_MS);

                    String txStr = createFrame(gpsData, oldGpsData);
                    Serial.println(txStr);

                    if (!isDistanceEnough(gpsData, oldGpsData)) {
                        vTaskDelay(41357 / portTICK_PERIOD_MS);
                    }

                    EEPROM.put(0, gpsData);
                    EEPROM.commit();

                    digitalWrite(LED_PIN, 0);
                    loraSend(txStr);
                    digitalWrite(LED_PIN, 1);

                    vTaskDelay(100 / portTICK_PERIOD_MS);

                    // Sleep:
                    goToSleep(WAIT_TIME_FIX_S);
                }
                // There is no GPS fix:
                else {
                    // Sleep:
                    goToSleep(WAIT_TIME_NO_FIX_S);
                }
            }
        }
    }
    vTaskDelay(1 / portTICK_PERIOD_MS);
}
