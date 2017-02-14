/*
  Stanford Student Space Initiative
  Balloons | HABEES | February 2017
  Davy Ragland | dragland@stanford.edu
  Vinh Nguyen | vnguyen5@stanford.edu

  File: APRS.h
  --------------------------
  APRS API.
*/

#ifndef APRS_H
#define APRS_H
#include <Arduino.h>
#include <stdint.h>
#include <string>
#include <SoftwareSerial.h>
#include "dra818v.h"
#include "afsk.h"

static const uint8_t MAX_SSIDS = 4;
static const int MAX_PACKET_SIZE = 512; //bytes

using namespace std;

struct SSID {
    const char* address;
    uint8_t ssid_designator;
};

class APRS
{
public:
/**********************************  SETUP  ***********************************/
    APRS(uint8_t tx, uint8_t rx, uint8_t pttPin, uint8_t micPin, \
          uint8_t  radio_enable, uint8_t radio_power,uint8_t sleepPin, \
          const char* targ_callsign, uint8_t targ_desig, const char* own_callsign, \
          const char* comment):
       radio(tx,rx,pttPin,micPin,radio_enable,radio_power,sleepPin){
         _targ_callsign = targ_callsign;
         _targ_desig = targ_desig;
         _own_callsign = own_callsign;
         _comment = comment;
    }
    bool    init();
/********************************  FUNCTIONS  *********************************/
    int16_t sendAdditionalData(char* extData, uint16_t len);
    int16_t sendPacket(char* time, float lat, float lon, float altitude, uint16_t heading, float speed, bool debug);
    void    sendPacketNoGPS(string data);
    void    sendPacketNoGPS(char* data);
    int     getPacketSize();
    void    clearPacket();
    void    setSSIDs();
private:
/*********************************  HELPERS  **********************************/
    void    loadHeader();
    void    loadData(uint8_t *data_buffer, uint8_t length);
    void    loadFooter();
    void    loadTrailingBits(uint8_t bitIndex);
    void    loadByte(uint8_t byte);
    void    loadBit(uint8_t bit, bool bitStuff);
    void    loadString(const char* str);
    void    loadString(const char* str, uint16_t len);
    void    loadString(string str);
    void    loadHDLCFlag();
    void    update_crc(uint8_t bit);
/*********************************  OBJECTS  **********************************/
    DRA818V radio;
    SSID ssids[MAX_SSIDS];
    uint8_t num_ssids;
    const char* _targ_callsign;
    uint8_t _targ_desig;
    const char* _own_callsign;
    const char* _comment;
    volatile uint8_t packet_buffer[MAX_PACKET_SIZE];
    int packet_size;
};
#endif // APRS_H
