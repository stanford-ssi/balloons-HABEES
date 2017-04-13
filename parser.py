#!/usr/bin/env python
# Stanford Student Space Initiative
# Balloons | HABEES | April 2017
# Davy Ragland | dragland@stanford.edu

# File: parser.py
# --------------------------
# Server side script to parse incoming messages
# from RockBLOCK

#*******************************  SETUP  ***************************************
import math

#******************************  GLOBALS  **************************************
#binary is the string we get from RockBLOCK
binary = "101101010011101011010001010010001111011101000001100010011000000110001011010100000000100001100111100010111001111010110000010111001110100000000100000000000011000011000101010000000101000000000010000111110010000011000100110011111111111111111111111111111111111111111111111111111111100000000001000010111001100100000000";
#Regex is what is pasted into "advanced parser"
regex = """lengthBits += compressVariable(data.LAT_GPS,                         -90,   90,      21, lengthBits);
lengthBits += compressVariable(data.LONG_GPS,                        -180,  180,     22, lengthBits);
lengthBits += compressVariable(data.ALTITUDE,                        -2000, 40000,   16, lengthBits);
lengthBits += compressVariable(data.ALTITUDE_GPS,                    -2000, 40000,   16, lengthBits);
lengthBits += compressVariable(data.ASCENT_RATE,                     -10,   10,      11, lengthBits);
lengthBits += compressVariable(data.CUTDOWN_STATE,                    0,    1,       1,  lengthBits);
lengthBits += compressVariable(data.PRESS,                            0,    500000,  19, lengthBits);
lengthBits += compressVariable(data.TEMP_IN,                         -50,   100,     9,  lengthBits);
lengthBits += compressVariable(data.TEMP_EXT,                        -50,   100,     9,  lengthBits);
lengthBits += compressVariable(data.VOLTAGE,                          0,    5,       9,  lengthBits);
lengthBits += compressVariable(data.CURRENT,                          0,    5000,    8,  lengthBits);
lengthBits += compressVariable(data.SPEED_GPS,                       -100,  100,     9,  lengthBits);
lengthBits += compressVariable(data.HEADING_GPS,                     -2000, 40000,   16, lengthBits);
lengthBits += compressVariable(data.NUM_SATS_GPS,                     0,    25,      4,  lengthBits);
lengthBits += compressVariable(data.LOOP_TIME,                        0,    10000,   10, lengthBits);
lengthBits += compressVariable(data.RB_SENT_COMMS,                    0,    8191,    13, lengthBits);
lengthBits += compressVariable(data.SHOULD_CUTDOWN,                   0,    1,       1,  lengthBits);
lengthBits += compressVariable(data.SETUP_STATE,                      0,    1,       1,  lengthBits);
lengthBits += compressVariable(data.DEBUG_STATE,                      0,    1,       1,  lengthBits);
lengthBits += compressVariable(data.BAT_GOOD_STATE,                   0,    1,       1,  lengthBits);
lengthBits += compressVariable(data.CURR_GOOD_STATE,                  0,    1,       1,  lengthBits);
lengthBits += compressVariable(data.PRES_GOOD_STATE,                  0,    1,       1,  lengthBits);
lengthBits += compressVariable(data.TEMP_GOOD_STATE,                  0,    1,       1,  lengthBits);
lengthBits += compressVariable(data.CAN_GOOD_STATE,                   0,    1,       1,  lengthBits);
lengthBits += compressVariable(data.RB_GOOD_STATE,                    0,    1,       1,  lengthBits);
lengthBits += compressVariable(data.GPS_GOOD_STATE,                   0,    1,       1,  lengthBits);
lengthBits += compressVariable(data.LOOP_GOOD_STATE,                  0,    1,       1,  lengthBits);
lengthBits += compressVariable(data.ALTITUDE_LAST,                   -2000, 40000,   16, lengthBits);
lengthBits += compressVariable(data.GPS_LAST,                         0,    500000,  19, lengthBits);
lengthBits += compressVariable(data.COMMS_LAST,                       0,    500000,  19, lengthBits);
lengthBits += compressVariable(data.WATCHDOG_LAST,                    0,    500000,  19, lengthBits);
lengthBits += compressVariable(data.LOOP_START / 1000,                0,    3000000, 20, lengthBits);
lengthBits += compressVariable(data.COMMS_LENGTH,                     0,    200,     8,  lengthBits);"""

names = []
mins  = []
maxs  = []
bits  = []

#******************************  HELPERS  *************************************
def setupREGEX():
    for line in regex.split('\n'):
        csv = line.split(",")
        names.append(csv[0].split("data.")[1])
        mins.append(int(csv[1].replace(" ", "")))
        maxs.append(int(csv[2].replace(" ", "")))
        bits.append(int(csv[3].replace(" ", "")))

def parseMessage(message):
    curr = 0
    for i in range(len(names)):
        num = message[curr:(curr + bits[i])]
        curr = curr + bits[i]
        adc = int(num, 2)
        value = mins[i] + adc * ((maxs[i] - mins[i]) / (math.pow(2, bits[i]) - 1))
        print(names[i] + ":" + str(value))

#********************************  MAIN  ***************************************
setupREGEX()
parseMessage(binary)
