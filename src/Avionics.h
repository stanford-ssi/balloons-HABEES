/*
  Stanford Student Space Initiative
  Balloons | HABEES | March 2017
  Davy Ragland | dragland@stanford.edu

  File: avionics.h
  --------------------------
  Primary avionics code.
*/

#ifndef AVIONICS_H
#define AVIONICS_H

#include "Config.h"
#include "Cutdown.h"
#include "Data.h"
#include "Sensors.h"
#include "Hardware.h"
#include <SD.h>
#include <GPS.h>
#include <RockBLOCK.h>
#include <APRS.h>
#include <CAN.h>

class Avionics {
public:
/**********************************  SETUP  ***********************************/
  Avionics() :
    PCB(),
    sensors(),
    gpsModule(GPS_ENABLE, GPS_BAUD, GPS_LOCK_TIME),
    canModule(CAN_ENABLE, CAN_BAUD),
    RBModule(RB_SLEEP, RB_BAUD),
    radioModule(DRA_TX, DRA_RX, DRA_PTT, DRA_MIC, DRA_ENABLE, DRA_PWR, DRA_SLEEP
      , TARGET_CALLSIGN, TARGET_DESIG, TX_CALLSIGN, MISSION_NUMBER) {
  }
  void    init();
/********************************  FUNCTIONS  *********************************/
  void    updateState();
  void    evaluateState();
  void    actuateState();
  void    logState();
  void    sendComms();
  void    sleep();
  bool    finishedSetup();

private:
/*********************************  HELPERS  **********************************/
  void    watchdog();
  bool    readData();
  bool    runHeaters();
  bool    runCutdown();
  bool    sendCAN();
  bool    sendSATCOMS();
  bool    sendAPRS();
  void    parseCommand(int16_t len);
  bool    calcVitals();
  bool    calcDebug();
  bool    calcCutdown();
  bool    debugState();
  void    displayState();
  void    setupLog();
  void    printHeader();
  void    logHeader();
  void    logAlert(const char*, bool fatal);
  void    printState();
  bool    logData();
  int16_t compressData();
  int16_t compressVariable(float var, float minimum, float maximum, int16_t resolution, int32_t length);
/*********************************  OBJECTS  **********************************/
  char COMMS_BUFFER[BUFFER_SIZE];
  DataFrame data;
  File dataFile;
  File logFile;
  Hardware PCB;
  Sensors sensors;
  GPS gpsModule;
  CAN canModule;
  RockBLOCK RBModule;
  APRS radioModule;
};

#endif
