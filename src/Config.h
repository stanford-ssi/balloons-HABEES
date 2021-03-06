/*
  Stanford Student Space Initiative
  Balloons | HABEES | APRIL 2017
  Davy Ragland | dragland@stanford.edu

  File: Config.h
  --------------------------
  Global constants specific to each launch.
*/

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <stdint.h>
#include <string.h>

/*****************************  TEENSY PIN OUTS  ******************************/
static const uint8_t   SD_CS                         =                       21;
static const uint8_t   BMP_CS1                       =                       20;
static const uint8_t   BMP_CS2                       =                       15;
static const uint8_t   THERMOCPL_CS                  =                       22;
static const uint8_t   CUTDOWN_PIN                   =                       25;
static const uint8_t   HEATER_PIN                    =                       23;
static const uint8_t   WATCHDOG_PIN                  =                       33;
static const uint8_t   FAULT_PIN                     =                       24;
static const uint8_t   VBAT_PIN                      =                      A13;

static const uint8_t   GPS_ENABLE                    =                       26;
static const uint8_t   RB_SLEEP                      =                        9;
static const uint8_t   DRA_ENABLE                    =                       A0;
static const uint8_t   DRA_SLEEP                     =                      A18;
static const uint8_t   DRA_PWR                       =                       A3;
static const uint8_t   DRA_TX                        =                      A16;
static const uint8_t   DRA_RX                        =                      A17;
static const uint8_t   DRA_MIC                       =                      A14;
static const uint8_t   DRA_PTT                       =                       A2;
static const uint8_t   CAN_ENABLE                    =                        5;

static const uint8_t   CAN_GOOD_LED                  =                        0;
static const uint8_t   RB_GOOD_LED                   =                        1;
static const uint8_t   GPS_GOOD_LED                  =                        2;
static const uint8_t   LOOP_GOOD_LED                 =                        3;
static const uint8_t   BAT_GOOD_LED                  =                        4;
static const uint8_t   CURR_GOOD_LED                 =                        5;
static const uint8_t   PRES_GOOD_LED                 =                        6;
static const uint8_t   TEMP_GOOD_LED                 =                        7;

/****************************  EDITABLE CONSTANTS  ****************************/
static const char      MISSION_NUMBER[]              =                 "SSI-51";
static const char      CSV_DATA_HEADER[]             =                 "TIME,LAT_GPS,LONG_GPS,ALTITUDE,ALTITUDE_GPS,ASCENT_RATE,CUTDOWN_STATE,PRESS,TEMP_IN,TEMP_EXT,VOLTAGE,CURRENT,SPEED_GPS,HEADING_GPS,NUM_SATS_GPS,LOOP_TIME,RB_SENT_COMMS,SHOULD_CUTDOWN,SETUP_STATE,DEBUG_STATE,BAT_GOOD_STATE,CURR_GOOD_STATE,PRES_GOOD_STATE,TEMP_GOOD_STATE,CAN_GOOD_STATE ,RB_GOOD_STATE,GPS_GOOD_STATE,LOOP_GOOD_STATE,ALTITUDE_LAST,GPS_LAST,COMMS_LAST,WATCHDOG_LAST,LOOP_START,COMMS_LENGTH";
static const bool      CUTDOWN_ALT_ENABLE            =                     true;
static const bool      CUTDOWN_GPS_ENABLE            =                     true;
static const uint16_t  CUTDOWN_ALT                   =                    20000;
static const uint16_t  CUTDOWN_TIME                  =                    10000;

static const uint16_t  GPS_LOCK_TIME                 =                      500;
static const float     GPS_FENCE_LAT_MIN             =                    -9999;
static const float     GPS_FENCE_LAT_MAX             =                     9999;
static const float     GPS_FENCE_LON_MIN             =                    -9999;
static const float     GPS_FENCE_LON_MAX             =                     9999;

static const uint32_t  GPS_INTERVAL                  =                    12000;
static const uint32_t  COMMS_INTERVAL                =                   300000;
static const uint32_t  COMMS_DEBUG_INTERVAL          =                    60000;
static const uint16_t  WATCHDOG_INTERVAL             =                      200;
static const uint16_t  LOOP_INTERVAL                 =                       50;

static const uint16_t  DEBUG_ALT                     =                      300;
static const uint16_t  ANALOG_RES                    =                       12;
static const uint16_t  ANALOG_MAX                    =                     4095;
static const uint16_t  BUFFER_SIZE                   =                      200;
static const uint32_t  CONSOLE_BAUD                  =                   115200;
static const uint16_t  GPS_BAUD                      =                     9600;
static const uint16_t  RB_BAUD                       =                    19200;
static const uint32_t  CAN_BAUD                      =                   500000;
static const double    PID_SETPOINT                  =                        0;

static const char      TARGET_CALLSIGN[]             =                   "APRS";
static const char      TX_CALLSIGN[]                 =                 "KM6HBK";
static const uint8_t   TARGET_DESIG                  =                        0;

#endif
