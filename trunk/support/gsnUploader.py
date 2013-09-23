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
import httplib, urllib, mimetypes

# Location of the local SQLite3 database file
IASSIST_DB = '../iassist.db'
# The deployment ID within the SQLite database
DEPLOYMENT_ID = 1
# The virtual sensor to which data should be uploaded
GSN_VIRTUAL_SENSOR = 'etz_ibutton_upload'
# The address of the HTTP interface of the GSN server
GSN_HOST = 'whymper.ethz.ch:23001'
# The script that handles uploads
GSN_UPLOAD_URL = '/upload'

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
     
def uploadPayload(host, url, virtualSensor, payloadType, payload):
    values = {
              'vsname': virtualSensor,
              'cmd': 'upload',
              'upload;payload_type': str(payloadType),
              'upload;payload': str(payload)
    }
    post_multipart(host, url, values)


def post_multipart(host, url, fields):
    """
    Post fields and files to an http host as multipart/form-data.
    fields is a sequence of (name, value) elements for regular form fields.
    files is a sequence of (name, filename, value) elements for data to be uploaded as files
    Return the server's response page.
    """
    content_type, body = encode_multipart_formdata(fields)
    h = httplib.HTTP(host)
    h.putrequest('POST', url)
    h.putheader('Content-Type', content_type)
    h.putheader('Content-Length', str(len(body)))
    h.putheader('User-Agent', 'Python HTTP Uploader')
    h.endheaders()
    h.send(body)
    errcode, errmsg, headers = h.getreply()
    return h.file.read()

def encode_multipart_formdata(fields):
    """
    fields is a sequence of (name, value) elements for regular form fields.
    files is a sequence of (name, filename, value) elements for data to be uploaded as files
    Return (content_type, body) ready for httplib.HTTP instance
    """
    BOUNDARY = '------MyFormBoundaryVN1AnA0H4wkOqBA6'
    CRLF = '\r\n'
    L = []
    L.append('--' + BOUNDARY)
    L.append('Content-Disposition: form-data; name="vsname"')
    L.append('')
    L.append(fields['vsname'])
    
    L.append('--' + BOUNDARY)
    L.append('Content-Disposition: form-data; name="cmd"')
    L.append('')
    L.append(fields['cmd'])
    
    L.append('--' + BOUNDARY)
    L.append('Content-Disposition: form-data; name="upload;payload_type"')
    L.append('')
    L.append(fields['upload;payload_type'])

    L.append('--' + BOUNDARY)
    L.append('Content-Disposition: form-data; name="upload;payload"')
    L.append('')
    L.append(fields['upload;payload'])    
    
    L.append('--' + BOUNDARY + '--')
    L.append('')
    body = CRLF.join(L)
    content_type = 'multipart/form-data; boundary=%s' % BOUNDARY
    return content_type, body

def get_content_type(filename):
    return mimetypes.guess_type(filename)[0] or 'application/octet-stream'

conn = sqlite3.connect(IASSIST_DB)
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
        
        # This first part is only executed for buttons for which the data has already been collected
        if mission[2] != None and str(mission[2]) != "":
            programmingTime = timeStrToUnixtimeMillis(mission[1])
            collectionTimeHost = timeStrToUnixtimeMillis(mission[2])
            collectionTimeButton = timeStrToUnixtimeMillis(mission[3]) 
            samplingStartTime = timeStrToUnixtimeMillis(mission[5])
            samplingRate = int(mission[4])
            
            multiplier = collectionTimeHost - programmingTime
            divisor = collectionTimeButton - programmingTime
            correctedStarttime = ((samplingStartTime - programmingTime) * multiplier / divisor) + programmingTime
            
            # Put all measurements in one long line
            line = []
            # Mission ID
            line.append(uniqueMissionId)
            # ButtonNr
            line.append(str(int(button[1])))
            # Number of samples
            cursorMeas.execute("SELECT COUNT(*) FROM Measurements WHERE MeasurementProfileID = ?", (mission[8],))
            sampleCnt = int(cursorMeas.fetchone()[0])
            line.append(str(sampleCnt))
                
            for measurement in cursorMeas.execute("SELECT MeasurementNr, Measurement FROM " + 
                                                  "Measurements WHERE MeasurementProfileID = ?", (mission[8],)):
                # Unix timestamp (milli)
                sampleNr = int(measurement[0])-1;
                line.append(str((sampleNr * 1000 * samplingRate) * multiplier / divisor + correctedStarttime))
                # Sample number, begins at 0 (before at 1)
                line.append(str(sampleNr))
                # Measurement
                line.append(str(measurement[1]))
     
            # Make HTTP request, temperature measurements are payload type 1
            uploadPayload(GSN_HOST, GSN_UPLOAD_URL, GSN_VIRTUAL_SENSOR, 1, ",".join(line))
     
        line = []
        # Mission ID
        line.append(uniqueMissionId)
        # Button ID
        line.append(str(button[1]))
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
        # Number of rolled over samples
        line.append('0')
        # Sampling rate
        line.append(str(mission[4]))
        # Sampling start time
        if mission[5] != None and str(mission[5]) != "":
            line.append(str(timeStrToUnixtimeMillis(mission[5])))
        else:
            line.append('')
        # Temperature channel enabled? 0/1
        line.append('1')
        # High resolution enabled? 0/1
        line.append(str(mission[6]))
        # Temperature calibration used? 0/1
        if int(mission[7]) == -9999:
            line.append('0')
        else:
            line.append(str(mission[7]))
        # Temperature coefficient A
        line.append('')
        # Temperature coefficient B
        line.append('')
        # Temperature coefficient C
        line.append('')
        # Humidity channel enabled? 0/1
        line.append('0')
        # High resolution enabled? 0/1
        line.append('0')
        # Humidity calibration used? 0/1
        line.append('0')
        # Humidity coefficient A
        line.append('')
        # Humidity coefficient B
        line.append('')
        # Humidity coefficient C
        line.append('')
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
        
        # Make HTTP request, mission data is payload type 0
        uploadPayload(GSN_HOST, GSN_UPLOAD_URL, GSN_VIRTUAL_SENSOR, 0, ",".join(line))
        
        
cursor.close()
cursorMission.close()
cursorMeas.close()
conn.close()
