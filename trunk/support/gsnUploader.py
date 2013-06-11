#!/usr/bin/env python

"""gsnUploader.py: Script for uploading the contents of a SQLite database to a GSN server"""

__author__      = "Matthias Keller"
__copyright__   = "Copyright 2013, Computer Engineering and Networks Laboratory, ETH Zurich"
__license__     = "GPL"

import sqlite3
from datetime import datetime, timedelta
from math import floor
import time
import calendar
import socket
import random

def generateUniqueId(timestampMilli):
    id = (timestampMilli & 0x7FFFFFFFFFF) << 20;
    rLong = long(random.uniform(0, pow(2, 32)-1))
    rLong = rLong << 32 + long(random.uniform(0, pow(2, 32)-1))
    id |= rLong & 0xFFFFF;
    return id

def currentUtcTimeMillis(): 
    return int(time.time()*1000)  

def timeStrToUnixtimeMillis(timeStr):
     ts = datetime.strptime(str(timeStr), '%Y-%m-%d %H:%M:%S')
     return int(calendar.timegm(ts.timetuple())*1000)
     

DEPLOYMENT_ID = 1
GSN_HOST = 'pc-10022.ethz.ch'
UPLOAD_PORT = 22002

print 'Connecting to: ', GSN_HOST, socket.gethostbyname(GSN_HOST)
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((socket.gethostbyname(GSN_HOST), UPLOAD_PORT))
txLineCnt = 0

conn = sqlite3.connect('../iassist.db')
cursor = conn.cursor()
cursorMission = conn.cursor()
cursorMeas = conn.cursor()
   
for button in cursor.execute("SELECT ButtonID, ButtonNr, SerialNr FROM Buttons WHERE DeploymentID = ?", (DEPLOYMENT_ID, )):
    for mission in cursorMission.execute("SELECT SessionNr, ProgrammingTime, CollectingTimeHost, " +
                                         "CollectingTimeButton, SamplingRate, SamplingStartTime, " + 
                                         "HighResolutionEn, TempCalibUsed, MeasurementProfileID FROM MeasurementProfiles " +
                                         " WHERE ButtonID = ?", (button[0],)):
    
        syncTime = currentUtcTimeMillis()
        uniqueMissionId = str(generateUniqueId(syncTime))
        sampleCnt = 0
        
        if mission[2] != None and str(mission[2]) != "":
            programmingTime = timeStrToUnixtimeMillis(mission[1])
            collectionTimeHost = timeStrToUnixtimeMillis(mission[2])
            collectionTimeButton = timeStrToUnixtimeMillis(mission[3]) 
            samplingStartTime = timeStrToUnixtimeMillis(mission[5])
            samplingRate = int(mission[4])
            
            multiplier = collectionTimeHost - programmingTime
            divisor = collectionTimeButton - programmingTime
            correctedStarttime = ((samplingStartTime - programmingTime) * multiplier / divisor) + programmingTime
            
            for measurement in cursorMeas.execute("SELECT MeasurementNr, Measurement FROM " + 
                                                  "Measurements WHERE MeasurementProfileID = ?", (mission[8],)):
                line = []
                # Message type
                line.append('2')
                # Mission ID
                line.append(uniqueMissionId)
                # ButtonNr
                line.append(str(int(button[1])))
                # Unix timestamp (milli)
                sampleNr = int(measurement[0])-1;
                line.append(str((sampleNr * 1000 * samplingRate) * multiplier / divisor + correctedStarttime))
                # Sample number, begins at 0 (before at 1)
                line.append(str(sampleNr))
                # Measurement
                line.append(str(measurement[1]))
               
                numSent = s.send(",".join(line)+"\n");
                txLineCnt = txLineCnt + 1
                print "MEASUREMENT: Sent ", numSent, " bytes" 
                
                sampleCnt = sampleCnt + 1
    
        line = []
        # Message type
        line.append('1')
        # Mission ID
        line.append(uniqueMissionId)
        # Session number
        line.append(str(mission[0]))
        # Programming time
        line.append(str(timeStrToUnixtimeMillis(mission[1])))
        # Collection time host
        if mission[2] != None and str(mission[2]) != "":
            line.append(str(timeStrToUnixtimeMillis(mission[2])))
        else:
            line.append('')
        # Collection time button
        if mission[3] != None and str(mission[3]) != "":
            line.append(str(timeStrToUnixtimeMillis(mission[3])))
        else:
            line.append('')
        # Number of samples
        line.append(str(sampleCnt))
        # Sampling rate
        line.append(str(mission[4]))
        # Sampling start time
        if mission[5] != None and str(mission[5]) != "":
            line.append(str(timeStrToUnixtimeMillis(mission[5])))
        else:
            line.append('')
        # High resolution enabled? 0/1
        line.append(str(mission[6]))
        # Temperature calibration used? 0/1
        if int(mission[7]) == -9999:
            line.append('')
        else:
            line.append(str(mission[7]))
        # Location Lon
        line.append('')
        # Location Lat
        line.append('')
        # Location Alt
        line.append('')
        # Location Accuracy
        line.append('')
        # Button prefix
        line.append(str(int(floor(int(button[1])/1000))))
        # Button number
        line.append(str(int(button[1])-1000*int(floor(int(button[1])/1000))))
        # Serial number
        buttonStr = str(button[2]).upper()
        line.append(' '.join(buttonStr[i:i+2] for i in xrange(0,len(buttonStr),2)))
        # Sync time
        line.append(str(syncTime))
        
        numSent = s.send(",".join(line)+"\n");
        txLineCnt = txLineCnt + 1
        print "MISSION: Sent ", numSent, " bytes" 
        
cursor.close()
cursorMission.close()
cursorMeas.close()
conn.close()
s.close()

print 'Sent ', txLineCnt, ' lines to GSN'
