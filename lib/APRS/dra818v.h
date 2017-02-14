#ifndef DRA818V_H
#define DRA818V_H

#include "Arduino.h"
#include "SoftwareSerial.h"

class DRA818V
{
public:
    DRA818V(uint8_t tx, uint8_t rx, uint8_t pttPin, uint8_t micPin, uint8_t  radio_enable, uint8_t radio_power,uint8_t sleepPin):
      radioSerial(_tx, _rx){
        _tx = tx;
        _rx = rx;
        _sleepPin = sleepPin;
        _pttPin = pttPin;
        _radio_enable = radio_enable;
        _radio_power = radio_power;
        _micPin = micPin;
    }
    void configSettings();
    bool init();
    SoftwareSerial radioSerial;
// private:
    uint8_t _tx = 0;
    uint8_t _rx = 0;
    uint8_t _pttPin = 0;
    uint8_t _micPin = 0;
    uint8_t audioOutPin = 0;
    uint8_t pwmPin = 0;
    uint8_t squelch;
    uint8_t _sleepPin = 0;
    uint8_t _radio_enable = 0;
    uint8_t _radio_power = 0;
};

#endif // DRA818V_H
