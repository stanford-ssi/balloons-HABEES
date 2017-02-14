#include "dra818v.h"
static const char* TX_CTCSS = "0000";
static const char* RX_CTCSS = "0000";
static const float APRS_NA_FTX = 144.390; //transmission frequency in MHz (APRS 144.390 standard)
static const float APRS_NA_FRX = 144.390; //receiving frequency in MHz
static const int SOFT_SERIAL_BAUD = 9600;
static const int SERIAL_BAUD = 9600;
static const bool CHANNEL_SCAN_BW = true; //true: 25kHz, false: 12.5kHz

bool DRA818V::init() {
    pinMode(_pttPin,OUTPUT);
    pinMode(_radio_enable,OUTPUT);
    pinMode(_micPin,OUTPUT);
    digitalWrite(_radio_enable,LOW);
    pinMode(_sleepPin,OUTPUT);
    digitalWrite(_sleepPin,HIGH);
    radioSerial.begin(SOFT_SERIAL_BAUD);
    if(!Serial) {
        Serial.begin(SOFT_SERIAL_BAUD);
    }
    digitalWrite(_pttPin,LOW);
    delay(200);
    radioSerial.print("AT+DMOCONNECT\r\n");
    digitalWrite(_pttPin,HIGH);
    delay(200);
    digitalWrite(_pttPin,LOW);
    delay(200);
    configSettings();
    return true;
}

void DRA818V::configSettings() {
    digitalWrite(_pttPin,LOW);
    radioSerial.print("AT+DMOSETGROUP=");
    radioSerial.print(CHANNEL_SCAN_BW,1);
    radioSerial.print(",");
    radioSerial.print(APRS_NA_FTX,4);
    radioSerial.print(",");
    radioSerial.print(APRS_NA_FRX,4);
    radioSerial.print(",");
    radioSerial.print(TX_CTCSS);
    radioSerial.print(",");
    radioSerial.print(squelch);
    radioSerial.print(",");
    radioSerial.print(RX_CTCSS);
    radioSerial.print("\r\n");
    delay(500);
    digitalWrite(_pttPin,HIGH);
    delay(200);
}
