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
binary = "000000000000010100111000000100000100000011000101001001111111110011110100100111100101101010011101011010001010010001111011101010000000000001100001100010000110001100000000110011101111110111111111111000000000000000000100"

#Regex is what is pasted into "advanced parser"
regex = """  lengthBits += compressVariable(data.LOOP_RATE,      0,    1000000, 19, lengthBits);
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
