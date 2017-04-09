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
binary = "00011001010000001000000000000001011110000110111111100001111100010011010110000000000000000000010000000000000000000001000000000000110000110001000011000011000100110100010011100100000000000000000000000000"

#Regex is what is pasted into "advanced parser"
regex = """lengthBits += compressVariable(data.LOOP_RATE,      0,    10000,   10, lengthBits);
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
  lengthBits += compressVariable(data.PRESS_BMP,      0,    500000,  19, lengthBits);
  lengthBits += compressVariable(data.NUM_SATS_GPS,   0,    25,      4,  lengthBits);
  lengthBits += compressVariable(data.RB_SENT_COMMS,  0,    8191,    13, lengthBits);
  lengthBits += compressVariable(data.CUTDOWN_STATE,  0,    1,       1,  lengthBits);"""

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
