#include "fcn.h"

void loraInit() {

    SPI.begin();
    LoRa.setPins(LORA_CS_PIN, -1, -1);
    LoRa.setSignalBandwidth(125E3);

    if (LoRa.begin(433775000)) Serial.println("LoRa dziala");
    else Serial.println("LoRa nie dziala");

    LoRa.setTimeout(100);
    LoRa.setTxPower(20);
    LoRa.enableCrc();
}

void loraSetConditions(uint32_t freq, uint8_t sf, uint8_t cr) {

    LoRa.setFrequency(freq);
    LoRa.setSpreadingFactor(sf);
    LoRa.setCodingRate4(cr);
}

void loraSend(String txString) {

    LoRa.beginPacket();
    LoRa.write('<');
    LoRa.write(0xFF);
    LoRa.write(0x01);
    LoRa.write((const uint8_t *)txString.c_str(), txString.length());
    LoRa.endPacket();
}

String addLeadingZeros(int number, int desiredLength) {

    String str = String(number);
    int currentLength = str.length();
    if (currentLength < desiredLength) {
        // Jeśli liczba ma mniej cyfr niż żądany, dodaj zera wiodące
        for (int i = 0; i < desiredLength - currentLength; i++) {
            str = "0" + str;
        }
    }
    return str;
}

String create_lat_aprs(double lat) {

    char n_s = 'N';
    if (lat < 0) {
        n_s = 'S';
    }

    lat = fabs(lat);
    //sprintf(str, "%02d%05.2f%c", (int)lat, (lat - (double)((int)lat)) * 60.0, n_s);

    String str = addLeadingZeros(lat, 2);
    str += String((lat - (double)((int)lat)) * 60.0, 2);
    str += String(n_s);

    return str;
}

String create_long_aprs(double lng) {

    char e_w = 'E';
    if (lng < 0) {
        e_w = 'W';
    }

    lng = fabs(lng);
    //sprintf(str, "%03d%05.2f%c", (int)lng, (lng - (double)((int)lng)) * 60.0, e_w);

    String str = addLeadingZeros(lng, 2);
    str += String((lng - (double)((int)lng)) * 60.0, 2);
    str += String(e_w);

    return str;
}

uint16_t calculateAngle(GpsData gps1, GpsData gps2) {

    const double deltaLon = gps2.lng - gps1.lng;

    const double x = cos(gps2.lat * PI / 180.0) * sin(deltaLon * PI / 180.0);
    const double y = cos(gps1.lat * PI / 180.0) * sin(gps2.lat * PI / 180.0) -
                     sin(gps1.lat * PI / 180.0) * cos(gps2.lat * PI / 180.0) *
                     cos(deltaLon * PI / 180.0);

    int16_t bearing = atan2(x, y) * 180.0 / PI;

    // Konwersja z (-180, 180] do [0, 360):
    if (bearing < 0) bearing += 360;
    return bearing % 360;
}

String createFrame(GpsData gpsData, GpsData oldGpsData) {

    String latString = create_lat_aprs(gpsData.lat);
    String lngString = create_long_aprs(gpsData.lng);

    char altString[10], speedString[20];
    sprintf(altString, "%06d", gpsData.alt);
    sprintf(speedString, "%03d/%03d", calculateAngle(oldGpsData, gpsData), gpsData.speed);

    String frame = "SP3MIK-11>APLT00,WIDE1-";

    if (gpsData.alt > 3000) frame += String(1);
    else frame += String(2);

    frame += ":!";
    frame += latString + "/" + lngString; //5106.57N/01703.45E
    frame += "O";
    frame += speedString;
    frame += "/A=";
    frame += altString; //000390
    frame += "T" + String(millis()/1000);

    return frame;
}
