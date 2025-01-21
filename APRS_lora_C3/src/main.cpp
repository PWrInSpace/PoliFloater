#include "fcn.h"

#define WAIT_TIME_NO_FIX_S 399
#define WAIT_TIME_FIX_S 1999
#define DBG

TinyGPSPlus gps;
GpsData gpsData, oldGpsData;

void setup() {

    Serial.begin(115200);

    Wire.begin(SDA_PIN, SCL_PIN);
    bmp.begin(BMP085_ULTRALOWPOWER, &Wire);

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
    EEPROM.begin(sizeof(GpsData));
    Serial.println(getVoltage());

    loraInit();
}

void loop() {

    vTaskDelay(100 / portTICK_PERIOD_MS);
    EEPROM.get(0, oldGpsData);

    // Mockup:
    gpsData.lat = 51.1148;
    gpsData.lng = 17.1524;

    gpsData.alt = bmp.readAltitude();
    Serial.println(gpsData.alt);

    if (gpsData.alt < 3000 && !oldGpsData.lastWasPolish) {

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

    EEPROM.put(0, gpsData);
    EEPROM.commit();

    digitalWrite(LED_PIN, 0);
    loraSend(txStr);
    digitalWrite(LED_PIN, 1);

    vTaskDelay(100 / portTICK_PERIOD_MS);

    // Sleep:
    if (gpsData.alt < 3000) goToSleep(WAIT_TIME_NO_FIX_S);
    else goToSleep(WAIT_TIME_FIX_S);
}
