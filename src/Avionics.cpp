/*
  Stanford Student Space Initiative
  Balloons | HABEES | March 2017
  Davy Ragland | dragland@stanford.edu
  Joan Creus-Costa | jcreus@stanford.edu

  File: Avionics.cpp
  --------------------------
  Implimentation of Avionics.h
*/

#include "Avionics.h"

/**********************************  SETUP  ***********************************/
/*
 * Function: init
 * -------------------
 * This function initializes the avionics flight controller.
 */
void Avionics::init() {
  PCB.init();
  Serial.begin(CONSOLE_BAUD);
  watchdog();
  printHeader();
  if(!SD.begin(SD_CS)) PCB.faultLED();
  setupLog();
  logHeader();
  if(!sensors.init())     logAlert("unable to initialize Sensors", true);
  if(!gpsModule.init())   logAlert("unable to initialize GPS", true);
  if(!RBModule.init())    logAlert("unable to initialize RockBlock", true);
  if(!radioModule.init()) logAlert("unable to initialize radio", true);
  if(!canModule.init())   logAlert("unable to initialize CAN BUS", true);
  watchdog();
  data.SETUP_STATE = false;
}

/********************************  FUNCTIONS  *********************************/
/*
 * Function: updateData
 * -------------------
 * This function handles basic flight data collection.
 */
void Avionics::updateState() {
  if(!readData()) logAlert("unable to read Data", true);
  watchdog();
}

/*
 * Function: evaluateState
 * -------------------
 * This function intelligently calculates the current state.
 */
void Avionics::evaluateState() {
  if(!calcVitals())  logAlert("unable to calculate vitals", true);
  if(!calcDebug())   logAlert("unable to calculate debug", true);
  if(!calcCutdown()) logAlert("unable to calculate cutdown", true);
  watchdog();
}

/*
 * Function: actuateState
 * -------------------
 * This function intelligently reacts to the current data frame.
 */
void Avionics::actuateState() {
  if(!runHeaters()) logAlert("unable to run heaters", true);
  if(!runCutdown()) logAlert("unable to run cutdown", true);
  watchdog();
}

/*
 * Function: logState
 * -------------------
 * This function logs the current data frame.
 */
void Avionics::logState() {
  if(!logData())    logAlert("unable to log Data", true);
  if(!debugState()) logAlert("unable to debug state", true);
  // if(!sendCAN()) logAlert("unable to send Data", true);
  watchdog();
}

/*
 * Function: sendComms
 * -------------------
 * This function sends the current data frame down.
 */
void Avionics::sendComms() {
  if((millis() - data.COMMS_LAST) < COMMS_RATE) return;
  if(compressData() < 0) logAlert("unable to compress Data", true);
  if(!sendSATCOMS()) logAlert("unable to communicate over RB", true);
  // if(!sendAPRS()) logAlert("unable to communicate over APRS", true);
  data.COMMS_LAST = millis();
  watchdog();
}

/*
 * Function: sleep
 * -------------------
 * This function sleeps at the end of the loop.
 */
void Avionics::sleep() {
  gpsModule.smartDelay(LOOP_RATE);
  watchdog();
}

/*
 * Function: finishedSetup
 * -------------------
 * This function returns true if the avionics has completed setup.
 */
bool Avionics::finishedSetup() {
  return !data.SETUP_STATE;
}

/*********************************  HELPERS  **********************************/
/*
 * Function: watchdog
 * -------------------
 * This function pulses the watchdog IC in order to ensure that avionics recovers from a fatal crash.
 */
void Avionics::watchdog() {
  if((millis() - data.WATCHDOG_LAST) < WATCHDOG_RATE) return;
  PCB.watchdog();
  data.WATCHDOG_LAST = millis();
}

/*
 * Function: readData
 * -------------------
 * This function updates the current data frame.
 */
bool Avionics::readData() {
  sprintf(data.TIME, "%s", sensors.getTime());
  data.LOOP_GOOD_STATE = !data.LOOP_GOOD_STATE;
  data.LOOP_RATE       = millis() - data.LOOP_START;
  data.LOOP_START      = millis();
  data.ALTITUDE_LAST   = data.ALTITUDE_BMP;
  data.VOLTAGE         = sensors.getVoltage();
  data.CURRENT         = sensors.getCurrent();
  data.TEMP_EXT        = sensors.getTempOut();
  data.TEMP_IN         = sensors.getTempIn();
  data.PRESS_BMP       = sensors.getPressure();
  data.ALTITUDE_BMP    = sensors.getAltitude();
  data.ASCENT_RATE     = sensors.getAscentRate();
  data.LAT_GPS         = gpsModule.getLatitude();
  data.LONG_GPS        = gpsModule.getLongitude();
  data.ALTITUDE_GPS    = gpsModule.getAltitude();
  data.HEADING_GPS     = gpsModule.getCourse();
  data.SPEED_GPS       = gpsModule.getSpeed();
  data.NUM_SATS_GPS    = gpsModule.getSats();
  return true;
}

/*
 * Function: runHeaters
 * -------------------
 * This function thermaly regulates the avionics.
 */
bool Avionics::runHeaters() {
  PCB.heater(data.TEMP_IN);
  return true;
}

/*
 * Function: runCutdown
 * -------------------
 * This function cuts down the payload if nessisary.
 */
bool Avionics::runCutdown() {
  if(data.CUTDOWN_STATE) return true;
  if(data.SHOULD_CUTDOWN) {
    PCB.cutDown(true);
    gpsModule.smartDelay(CUTDOWN_TIME);
    PCB.cutDown(false);
    data.CUTDOWN_STATE = true;
    logAlert("completed cutdown", false);
  }
  return true;
}

/*
 * Function: sendCAN
 * -------------------
 * This function sends the current data frame over the CAN BUS IO.
 */
bool Avionics::sendCAN() {
  logAlert("sending CAN message", false);
  int16_t ret = canModule.write(COMMS_BUFFER, data.COMMS_LENGTH);
  if(ret < 0) {
    data.CAN_GOOD_STATE  = false;
    return false;
  }
  data.CAN_GOOD_STATE  = true;
  return true;
}

/*
 * Function: sendSATCOMS
 * -------------------
 * This function sends the current data frame over the ROCKBLOCK IO.
 */
bool Avionics::sendSATCOMS() {
  logAlert("sending Rockblock message", false);
  data.RB_SENT_COMMS++;
  int16_t ret = RBModule.writeRead(COMMS_BUFFER, data.COMMS_LENGTH);
  if(ret < 0) {
    data.RB_GOOD_STATE  = false;
    return false;
  }
  data.RB_GOOD_STATE  = true;
  if(ret > 0) parseCommand(ret);
  return true;
}

/*
 * Function: sendAPRS
 * -------------------
 * This function sends the current data frame over the APRS RF IO.
 */
bool Avionics::sendAPRS() {
  logAlert("sending APRS message", false);
  if (radioModule.sendAdditionalData(COMMS_BUFFER, data.COMMS_LENGTH) < 0) return false;
  if (radioModule.sendPacket(data.TIME, data.LAT_GPS, data.LONG_GPS, data.ALTITUDE_LAST, data.HEADING_GPS, data.SPEED_GPS, data.DEBUG_STATE) < 0) return false;
  return true;
}

/*
 * Function: parseCommand
 * -------------------
 * This function parses the command received from the RockBLOCK.
 */
void Avionics::parseCommand(int16_t len) {
  if(strncmp(COMMS_BUFFER, CUTDOWN_COMAND, len)) {
    data.SHOULD_CUTDOWN = true;
  }
}

/*
 * Function: calcVitals
 * -------------------
 * This function calculates if the current state is within bounds.
 */
bool Avionics::calcVitals() {
  data.BAT_GOOD_STATE  = (data.VOLTAGE >= 3.63);
  data.CURR_GOOD_STATE = (data.CURRENT > -5.0 && data.CURRENT <= 500.0);
  data.PRES_GOOD_STATE = (data.ALTITUDE_BMP > -50 && data.ALTITUDE_BMP < 200);
  data.TEMP_GOOD_STATE = (data.TEMP_IN > 15 && data.TEMP_IN < 50);
  data.GPS_GOOD_STATE  = (data.LAT_GPS != 1000.0 && data.LAT_GPS != 0.0 && data.LONG_GPS != 1000.0 && data.LONG_GPS != 0.0);
  return true;
}

/*
 * Function: calcDebug
 * -------------------
 * This function calculates if the avionics is in debug mode.
 */
bool Avionics::calcDebug() {
  if(data.DEBUG_STATE   && (data.ALTITUDE_LAST >= DEBUG_ALT) && (data.ALTITUDE_BMP >= DEBUG_ALT)) {
    data.DEBUG_STATE = false;
  }
  return true;
}

/*
 * Function: calcCutdown
 * -------------------
 * This function calculates if the avionics should cutdown.
 */
bool Avionics::calcCutdown() {
  if(CUTDOWN_GPS_ENABLE && data.GPS_GOOD_STATE &&
    (((data.LAT_GPS < GPS_FENCE_LAT_MIN) || (data.LAT_GPS > GPS_FENCE_LAT_MAX)) ||
    ((data.LONG_GPS < GPS_FENCE_LON_MIN) || (data.LONG_GPS > GPS_FENCE_LON_MAX)))
  ) data.SHOULD_CUTDOWN  = true;

  if(CUTDOWN_ALT_ENABLE && !data.CUTDOWN_STATE &&
    (data.ALTITUDE_LAST >= CUTDOWN_ALT) &&
    (data.ALTITUDE_BMP  >= CUTDOWN_ALT)
  ) data.SHOULD_CUTDOWN  = true;
  return true;
}

/*
 * Function: debugState
 * -------------------
 * This function provides debuging information.
 */
bool Avionics::debugState() {
  if(data.DEBUG_STATE) {
    displayState();
    printState();
  }
  return true;
}

/*
 * Function: displayState
 * -------------------
 * This function displays the current avionics state.
 */
void Avionics::displayState() {
    PCB.writeLED(BAT_GOOD_LED,  data.BAT_GOOD_STATE);
    PCB.writeLED(CURR_GOOD_LED, data.CURR_GOOD_STATE);
    PCB.writeLED(PRES_GOOD_LED, data.PRES_GOOD_STATE);
    PCB.writeLED(TEMP_GOOD_LED, data.TEMP_GOOD_STATE);
    PCB.writeLED(CAN_GOOD_LED,  data.CAN_GOOD_STATE);
    PCB.writeLED(RB_GOOD_LED,   data.RB_GOOD_STATE);
    PCB.writeLED(GPS_GOOD_LED,  data.GPS_GOOD_STATE);
    PCB.writeLED(LOOP_GOOD_LED, data.LOOP_GOOD_STATE);
}

/*
 * Function: setupLog
 * -------------------
 * This function initializes the SD card file.
 */
void Avionics::setupLog() {
  Serial.println("Card Initialitzed");
  char filename[] = "LOGGER00.txt";
  for (uint8_t i = 0; i < 100; i++) {
    filename[6] = i / 10 + '0';
    filename[7] = i % 10 + '0';
    if (! SD.exists(filename)) {
      dataFile = SD.open(filename, FILE_WRITE);
      break;
    }
  }
  logFile = SD.open("EVENTS.txt", FILE_WRITE);
  if (!dataFile || !logFile) {
    PCB.faultLED();
    Serial.println ("ERROR: COULD NOT CREATE FILE");
  }
  else {
    Serial.print("Logging to: ");
    Serial.println(filename);
  }
}

/*
 * Function: printHeader
 * -------------------
 * This function prints the CSV header.
 */
void Avionics::printHeader() {
  if(!Serial) PCB.faultLED();
  Serial.print("Stanford Student Space Initiative Balloons Launch ");
  Serial.print(MISSION_NUMBER);
  Serial.print('\n');
  Serial.print(CSV_DATA_HEADER);
  Serial.print('\n');
}

/*
 * Function: logHeader
 * -------------------
 * This function logs the CSV header.
 */
void Avionics::logHeader() {
  dataFile.print("Stanford Student Space Initiative Balloons Launch ");
  dataFile.print(MISSION_NUMBER);
  dataFile.print('\n');
  dataFile.print(CSV_DATA_HEADER);
  dataFile.print('\n');
  dataFile.flush();
}

/*
 * Function: logAlert
 * -------------------
 * This function logs important information whenever a specific event occurs.
 */
void Avionics::logAlert(const char* debug, bool fatal) {
  if(fatal) PCB.faultLED();
  if(logFile) {
    logFile.print(data.TIME);
    logFile.print(',');
    if(fatal) logFile.print("FATAL ERROR!!!!!!!!!!: ");
    else logFile.print("Alert: ");
    logFile.print(debug);
    logFile.print("...\n");
    logFile.flush();
  }
  if(data.DEBUG_STATE) {
    Serial.print(data.TIME);
    Serial.print(',');
    if(fatal) Serial.print("FATAL ERROR!!!!!!!!!!: ");
    else Serial.print("Alert: ");
    Serial.print(debug);
    Serial.print("...\n");
  }
}

/*
 * Function: printState
 * -------------------
 * This function prints the current avionics state.
 */
void Avionics::printState() {
  Serial.print(data.TIME);
  Serial.print(',');
  Serial.print(data.LOOP_RATE);
  Serial.print(',');
  Serial.print(data.VOLTAGE);
  Serial.print(',');
  Serial.print(data.CURRENT);
  Serial.print(',');
  Serial.print(data.ALTITUDE_BMP);
  Serial.print(',');
  Serial.print(data.ASCENT_RATE);
  Serial.print(',');
  Serial.print(data.TEMP_IN);
  Serial.print(',');
  Serial.print(data.TEMP_EXT);
  Serial.print(',');
  Serial.print(data.LAT_GPS, 4);
  Serial.print(',');
  Serial.print(data.LONG_GPS, 4);
  Serial.print(',');
  Serial.print(data.SPEED_GPS);
  Serial.print(',');
  Serial.print(data.HEADING_GPS);
  Serial.print(',');
  Serial.print(data.ALTITUDE_GPS);
  Serial.print(',');
  Serial.print(data.PRESS_BMP);
  Serial.print(',');
  Serial.print(data.NUM_SATS_GPS);
  Serial.print(',');
  Serial.print(data.RB_SENT_COMMS);
  Serial.print(',');
  Serial.print(data.CUTDOWN_STATE);
  Serial.print('\n');
}

/*
 * Function: logData
 * -------------------
 * This function logs the current data frame.
 */
bool Avionics::logData() {
  bool sucess = true;
  dataFile.print(data.TIME);
  dataFile.print(',');
  dataFile.print(data.LOOP_RATE);
  dataFile.print(',');
  dataFile.print(data.VOLTAGE);
  dataFile.print(',');
  dataFile.print(data.CURRENT);
  dataFile.print(',');
  dataFile.print(data.ALTITUDE_BMP);
  dataFile.print(',');
  dataFile.print(data.ASCENT_RATE);
  dataFile.print(',');
  dataFile.print(data.TEMP_IN);
  dataFile.print(',');
  dataFile.print(data.TEMP_EXT);
  dataFile.print(',');
  dataFile.print(data.LAT_GPS, 4);
  dataFile.print(',');
  dataFile.print(data.LONG_GPS, 4);
  dataFile.print(',');
  dataFile.print(data.SPEED_GPS);
  dataFile.print(',');
  dataFile.print(data.HEADING_GPS);
  dataFile.print(',');
  dataFile.print(data.ALTITUDE_GPS);
  dataFile.print(',');
  dataFile.print(data.PRESS_BMP);
  dataFile.print(',');
  dataFile.print(data.NUM_SATS_GPS);
  dataFile.print(',');
  dataFile.print(data.RB_SENT_COMMS);
  if(dataFile.print(',') != 1) sucess = false;
  dataFile.print(data.CUTDOWN_STATE);
  dataFile.print('\n');
  dataFile.flush();
  return sucess;
}

/*
 * Function: compressData
 * -------------------
 * This function compresses the data frame into a bit stream.
 */
int16_t Avionics::compressData() {
  int16_t lengthBits  = 0;
  int16_t lengthBytes = 0;
  for(uint16_t i = 0; i < BUFFER_SIZE; i++) COMMS_BUFFER[i] = 0;
  for(uint16_t i = 0; i < sizeof(data.TIME); i++) {
    COMMS_BUFFER[i] = data.TIME[i];
    lengthBits += 8;
  }
  lengthBits += compressVariable(data.LOOP_RATE,      0,    1000000, 19, lengthBits);
  lengthBits += compressVariable(data.VOLTAGE,        0,    5,       9,  lengthBits);
  lengthBits += compressVariable(data.CURRENT,        0,    5000,    8,  lengthBits);
  lengthBits += compressVariable(data.ALTITUDE_BMP,  -2000, 40000,   16, lengthBits);
  lengthBits += compressVariable(data.ASCENT_RATE,   -10,   10,      11, lengthBits);
  lengthBits += compressVariable(data.TEMP_IN,       -50,   100,     9,  lengthBits);
  lengthBits += compressVariable(data.TEMP_EXT,      -100,  100,     9,  lengthBits);
  lengthBits += compressVariable(data.LAT_GPS,       -90,   90,      21, lengthBits);
  lengthBits += compressVariable(data.LONG_GPS,      -180,  180,     22, lengthBits);
  lengthBits += compressVariable(data.SPEED_GPS,     -100,  100,     9,  lengthBits);
  lengthBits += compressVariable(data.HEADING_GPS,   -2000, 40000,   16, lengthBits);
  lengthBits += compressVariable(data.ALTITUDE_GPS,  -2000, 40000,   16, lengthBits);
  lengthBits += compressVariable(data.PRESS_BMP,      0,    1000000, 19, lengthBits);
  lengthBits += compressVariable(data.NUM_SATS_GPS,   0,    10,      11, lengthBits);
  lengthBits += compressVariable(data.RB_SENT_COMMS,  0,    1000000, 19, lengthBits);
  lengthBits += compressVariable(data.CUTDOWN_STATE,  0,    1,       1,  lengthBits);
  lengthBits += 8 - (lengthBits % 8);
  lengthBytes = lengthBits / 8;
  data.COMMS_LENGTH = lengthBytes;
  return lengthBytes;
}

/*
 * Function: compressVariable
 * -------------------
 * This function compresses a single variable into a scaled digital bitmask.
 */
int16_t Avionics::compressVariable(float var, float minimum, float maximum, int16_t resolution, int16_t length) {
  if (resolution <= 0) return -1;
  if (var < minimum) var = minimum;
  if (var > maximum) var = maximum;
  int32_t adc = round( (pow(2, resolution) - 1) * (var - minimum) / (maximum - minimum));
  int16_t byteIndex = length / 8;
  int16_t bitIndex = 7 - (length % 8);
  for (uint16_t i = resolution - 1; i >= 0; i--) {
    bool bit = adc & (1 << i);
    if (bit) COMMS_BUFFER[byteIndex] |= (1 << bitIndex);
    bitIndex -= 1;
    if (bitIndex == 0) {
      bitIndex = 7;
      byteIndex++;
    }
  }
  return resolution;
}
