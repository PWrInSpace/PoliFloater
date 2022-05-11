#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include <TinyGPSPlus.h>

HardwareSerial Serial1(PA10, PA9);
HardwareSerial Serial3(PB11, PB10);

TinyGPSPlus gps;
String gpsStrig;

// LSB
static const u1_t PROGMEM APPEUI[8]={ 0xEE, 0x9A, 0x7A, 0x2B, 0xE2, 0xF9, 0x81, 0x60 };
void os_getArtEui (u1_t* buf) { memcpy_P(buf, APPEUI, 8);}

// LSB
static const u1_t PROGMEM DEVEUI[8]={ 0x43, 0x1A, 0x03, 0x3A, 0x30, 0xF9, 0x81, 0x60 };
void os_getDevEui (u1_t* buf) { memcpy_P(buf, DEVEUI, 8);}

// MSB
static const u1_t PROGMEM APPKEY[16] = { 0x30, 0x39, 0xCB, 0xAD, 0xB2, 0x0E, 0x00, 0xBB, 0x38, 0xCD, 0xE1, 0xE9, 0x46, 0xE8, 0x0A, 0x03 };
void os_getDevKey (u1_t* buf) {  memcpy_P(buf, APPKEY, 16);}

static uint8_t mydata[14] = "PWr";
static osjob_t sendjob;

// Schedule TX every this many seconds
const unsigned TX_INTERVAL = 120;

// Pin mapping
const lmic_pinmap lmic_pins = {
    .nss = PB8,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = PB9,
    .dio = { PB7, PB6, LMIC_UNUSED_PIN},
};

void do_send(osjob_t* j){
    // Check if there is not a current TX/RX job running
    if (LMIC.opmode & OP_TXRXPEND) {
        Serial1.println(F("OP_TXRXPEND, not sending"));
    } else {
        // Prepare upstream data transmission at the next possible time.
        LMIC_setTxData2(1, mydata, sizeof(mydata)-1, 0);
        Serial1.println(F("Packet queued"));
    }
    // Next TX is scheduled after TX_COMPLETE event.
}

void onEvent (ev_t ev) {
    Serial1.print(os_getTime() / 100000);
    Serial1.print(": ");
    switch(ev) {
        case EV_SCAN_TIMEOUT:
            Serial1.println(F("EV_SCAN_TIMEOUT"));
            break;
        case EV_BEACON_FOUND:
            Serial1.println(F("EV_BEACON_FOUND"));
            break;
        case EV_BEACON_MISSED:
            Serial1.println(F("EV_BEACON_MISSED"));
            break;
        case EV_BEACON_TRACKED:
            Serial1.println(F("EV_BEACON_TRACKED"));
            break;
        case EV_JOINING:
            Serial1.println(F("EV_JOINING"));
            break;
        case EV_JOINED:
            Serial1.println(F("EV_JOINED"));

            // Disable link check validation (automatically enabled
            // during join, but not supported by TTN at this time).
            LMIC_setLinkCheckMode(0);
            break;
        case EV_RFU1:
            Serial1.println(F("EV_RFU1"));
            break;
        case EV_JOIN_FAILED:
            Serial1.println(F("EV_JOIN_FAILED"));
            break;
        case EV_REJOIN_FAILED:
            Serial1.println(F("EV_REJOIN_FAILED"));
            break;
            break;
        case EV_TXCOMPLETE:
            Serial1.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
            
            digitalWrite(PB15, 1);
            digitalWrite(PA8, 0);

            if (LMIC.txrxFlags & TXRX_ACK)
              Serial1.println(F("Received ack"));
            if (LMIC.dataLen) {
              Serial1.println(F("Received "));
              Serial1.println(LMIC.dataLen);
              Serial1.println(F(" bytes of payload"));
            }
            // Schedule next transmission
            os_setTimedCallback(&sendjob, os_getTime()+sec2osticks(TX_INTERVAL), do_send);
            break;
        case EV_LOST_TSYNC:
            Serial1.println(F("EV_LOST_TSYNC"));
            break;
        case EV_RESET:
            Serial1.println(F("EV_RESET"));
            break;
        case EV_RXCOMPLETE:
            // data received in ping slot
            Serial1.println(F("EV_RXCOMPLETE"));
            break;
        case EV_LINK_DEAD:
            Serial1.println(F("EV_LINK_DEAD"));
            break;
        case EV_LINK_ALIVE:
            Serial1.println(F("EV_LINK_ALIVE"));
            break;
         default:
            Serial1.println(F("Unknown event"));
            Serial1.println((int)ev);
            break;
    }
}

void setup() {
    Serial1.begin(115200);
    Serial3.begin(9600);
    Serial1.setTimeout(10);
    Serial3.setTimeout(10);

    pinMode(PB14, OUTPUT);
    pinMode(PB13, OUTPUT);
    pinMode(PB15, OUTPUT);
    pinMode(PA8, OUTPUT);

    digitalWrite(PB14, 1);
    digitalWrite(PB13, 1);
    digitalWrite(PB15, 1);
    digitalWrite(PA8, 1);

    delay(500);
    digitalWrite(PB13, 0);
    delay(500);
    digitalWrite(PB13, 1);
    digitalWrite(PB15, 0);
    delay(500);
    digitalWrite(PB15, 1);
    digitalWrite(PA8, 0);
    delay(500);
    digitalWrite(PA8, 1);

    Serial1.println("LECIMYY");

    bool encoded = false;

    digitalWrite(PB13, 0);

    do {

        while (Serial3.available()) {
            char tmpChar = Serial3.read();
            gps.encode(tmpChar);
            Serial1.print(tmpChar);
            encoded = true;
        }

        delay(100);

        /*if (encoded) {
            encoded = false;

            Serial1.println("\nPOZ:");
            gpsStrig =  String(gps.location.lat(), 3) + ";";
            gpsStrig += String(gps.location.lng(), 3) + ";";
            gpsStrig += String(gps.altitude.meters(), 0) + ";";
            gpsStrig += String(gps.time.second()) + ";";
            gpsStrig += String(gps.satellites.value());
            Serial1.println(gpsStrig);
        }*/
    
    } while (abs(gps.location.lat()) < 0.5);
    
    uint8_t idx = 0;
    int32_t dataLat = (gps.location.lat() * 1E7);
    mydata[idx++] = dataLat >> 24;
    mydata[idx++] = dataLat >> 16;
    mydata[idx++] = dataLat >> 8;
    mydata[idx++] = dataLat;
    int32_t dataLng = (gps.location.lng() * 1E7);
    mydata[idx++] = dataLng >> 24;
    mydata[idx++] = dataLng >> 16;
    mydata[idx++] = dataLng >> 8;
    mydata[idx++] = dataLng;
    int16_t dataAlt = (gps.altitude.meters());
    mydata[idx++] = dataAlt >> 8;
    mydata[idx++] = dataAlt;
    int16_t dataSpeed = (gps.speed.mph());
    mydata[idx++] = dataSpeed >> 8;
    mydata[idx++] = dataSpeed;
    uint16_t dataBatt = 3300;
    mydata[idx++] = dataBatt >> 8;
    mydata[idx++] = dataBatt;

    delay(3000);
    Serial1.println("Zaczynamy");

    digitalWrite(PB13, 1);
    digitalWrite(PB15, 0);

    SPI.setMISO(PB4);
    SPI.setMOSI(PB5);
    SPI.setSCLK(PB3);

    //pinMode(PC13, OUTPUT);
    //strcpy((char*) mydata, "PWr in Space");

    // LMIC init
    os_init();
    Serial1.println("inic");
    // Reset the MAC state. Session and pending data transfers will be discarded.
    LMIC_reset();
    Serial1.println("res");

    // Start job (sending automatically starts OTAA too)
    //digitalWrite(PC13, 0);
    do_send(&sendjob);
    Serial1.println("Poszlo do kolejki");
}

uint32_t gpsTimer;

void loop() {

    while (Serial3.available()) {
        gps.encode(Serial3.read());
    }

    if (millis() - gpsTimer >= 10000) {

        gpsTimer = millis();
        gpsStrig =  String(gps.location.lat(), 3) + ";";
        gpsStrig += String(gps.location.lng(), 3) + ";";
        gpsStrig += String(gps.altitude.meters(), 0) + ";";
        gpsStrig += String(gps.time.second()) + ";";
        gpsStrig += String(gps.satellites.value());

        Serial1.println(gpsStrig);

        uint8_t idx = 0;
        int32_t dataLat = (gps.location.lat() * 1E7);
        mydata[idx++] = dataLat >> 24;
        mydata[idx++] = dataLat >> 16;
        mydata[idx++] = dataLat >> 8;
        mydata[idx++] = dataLat;
        int32_t dataLng = (gps.location.lng() * 1E7);
        mydata[idx++] = dataLng >> 24;
        mydata[idx++] = dataLng >> 16;
        mydata[idx++] = dataLng >> 8;
        mydata[idx++] = dataLng;
        int16_t dataAlt = (gps.altitude.meters());
        mydata[idx++] = dataAlt >> 8;
        mydata[idx++] = dataAlt;
        int16_t dataSpeed = (gps.speed.mph());
        mydata[idx++] = dataSpeed >> 8;
        mydata[idx++] = dataSpeed;
        uint16_t dataBatt = 3300;
        mydata[idx++] = dataBatt >> 8;
        mydata[idx++] = dataBatt;
    }

    os_runloop_once();
}