#!/usr/bin/env python

"""gsnCsvExport.py: Script for exporting measurements collected into multiple CSV files that are then uploaded to GSN."""

__author__      = "Matthias Keller"
__copyright__   = "Copyright 2012, Computer Engineering and Networks Laboratory, ETH Zurich"
__license__     = "GPL"

import sqlite3
from datetime import datetime, timedelta
import calendar
import argparse

# Set this variable to change the number space in which devices are mapped
deviceIdOffset = 10000

parser = argparse.ArgumentParser(description='Export iButton data to multiple files for GSN.')
parser.add_argument('-d', action='store', dest='deployment_id',
                    help='Deployment ID', required=True, type=int)

results = parser.parse_args()

# We need to generate three files
# 1) Output for ibutton_config virtual sensor
# 2) Output for ibutton_measurements virtual sensor
# 3) Output for GSN position mapping

fileNameBase = datetime.now().strftime('%Y%m%d%H%M%S')
fileNameConfig = fileNameBase+'_ibutton_config.csv'
fileNameMeasurements = fileNameBase+'_ibutton_measurements.csv'
fileNamePosMapping = fileNameBase+'_ibutton_posmapping.csv'

outputIButtonConfig = open(fileNameConfig, 'w')
outputIButtonMeasurements = open(fileNameMeasurements, 'w')
outputPosMapping = open(fileNamePosMapping, 'w')

#outputIButtonConfig.write('# DEVICE_ID, CONFIG_ID, SERIAL_NR (1001-999999), '
#    'PROGRAMMING_TIME (unixtime msec), SAMPLING_START_TIME (unixtime msec), SAMPLING_RATE (int), '
#    'HIGH_TEMP_RESOLUTION_EN (0 or 1), CALIB_COEFF_A (double), CALIB_COEFF_B (double), '
#    'CALIB_COEFF_C (double), COLLECT_TIME_HOST (unixtime msec), COLLECT_TIME_BUTTON (unixtime msec), '
#    'NUM_SAMPLES (0-4096), SW_TEMP_CALIB_USED (0 or 1)\n')

#outputIButtonMeasurements.write('# GENERATION_TIME (unixtime msec), DEVICE_ID, '
#    'CONFIG_ID, SAMPLE_NR (1-4096), TEMPERATURE (double)\n')

conn = sqlite3.connect('iassist.db')
c = conn.cursor()
for button in c.execute('SELECT ButtonID, ButtonNr, SerialNr, CalibCoeffA, CalibCoeffB, CalibCoeffC \
    FROM Buttons WHERE DeploymentID = ?', (results.deployment_id,)):
	c2 = conn.cursor()
        deviceId = deviceIdOffset + int(button[0])
        for profile in c2.execute('SELECT MeasurementProfileID, ProgrammingTime, SamplingStartTime, \
	    SamplingRate, HighResolutionEn, CollectingTimeHost, CollectingTimeButton, TempCalibUsed \
	    FROM MeasurementProfiles WHERE ButtonID = ?', (button[0],)):
            
            # We make one entry with TIMED = ProgrammingTime, and one entry with TIMED = CollectingTimeHost
       
            # Those fields are always set, also when the button has not been read out yet
            configId = int(profile[0]) # use MeasurementProfileID for the moment
            serialNr = str(button[2])
            
            if button[3] != None:
                calibCoeffA = str(float(button[3]))
                calibCoeffB = str(float(button[4]))
                calibCoeffC = str(float(button[5]))
            else:
                calibCoeffA = 'NULL'
                calibCoeffB = 'NULL'
                calibCoeffC = 'NULL'
            sampleStart = datetime.strptime(str(profile[2]), '%d.%m.%Y %H:%M:%S')
            samplingRate = int(profile[3])
            
            if profile[1] != None and profile[1] != "":
                programmingTime = datetime.strptime(str(profile[1]), '%d.%m.%Y %H:%M:%S')
                programmingTimeStr = str(calendar.timegm(programmingTime.utctimetuple())*1000)
            else:
                programmingTimeStr = 'NULL'
            
            if profile[4] != None and profile[4] != "":
                highTempResolutionEn = int(profile[4])
            else:
                highTempResolutionEn = 1
            
            # Write entry for programming event
            out = []
            # DEVICE_ID
            out.append(str(deviceId)), out.append(',')
            # CONFIG_ID
            out.append(str(configId)), out.append(',')
            # SERIAL_NR
            out.append(serialNr), out.append(',')
            # PROGRAMMING_TIME
            out.append(programmingTimeStr), out.append(',')
            # SAMPLING_START_TIME
            out.append(str(sampleStart)), out.append(',')
            # SAMPLING_RATE
            out.append(str(samplingRate)), out.append(',')
            # HIGH_TEMP_RESOLUTION_EN
            out.append(str(highTempResolutionEn)), out.append(',')
            # CALIB_COEFF_A
            out.append(calibCoeffA), out.append(',')
            # CALIB_COEFF_B
            out.append(calibCoeffB), out.append(',')
            # CALIB_COEFF_C
            out.append(calibCoeffC), out.append(',')
            # COLLECT_TIME_HOST
            out.append('NULL'), out.append(',')
            # COLLECT_TIME_BUTTON
            out.append('NULL'), out.append(',')
            # NUM_SAMPLES
            out.append('NULL'), out.append(',')
            # SW_TEMP_CALIB_USED
            out.append('NULL')
            outputIButtonConfig.write(''.join(out)+'\n')
            
            # Also write entry for collect event if already collected
            if profile[5] != None and profile[5] != "":
                collectTimeHost = datetime.strptime(str(profile[5]), '%d.%m.%Y %H:%M:%S')
                collectTimeButton = datetime.strptime(str(profile[6]), '%d.%m.%Y %H:%M:%S')
                tempCalibUsed = profile[7]
                c3 = conn.cursor()
                c3.execute('SELECT MAX(MeasurementNr) FROM Measurements WHERE MeasurementProfileID=?', (profile[0],))
                numSamples = c3.fetchone()[0]
        
                out = []
                # DEVICE_ID
                out.append(str(deviceId)), out.append(',')
                # CONFIG_ID
                out.append(str(configId)), out.append(',')
                # SERIAL_NR
                out.append(serialNr), out.append(',')
                # PROGRAMMING_TIME
                out.append(programmingTimeStr), out.append(',')
                # SAMPLING_START_TIME
                out.append(str(sampleStart)), out.append(',')
                # SAMPLING_RATE
                out.append(str(samplingRate)), out.append(',')
                # HIGH_TEMP_RESOLUTION_EN
                out.append(str(highTempResolutionEn)), out.append(',')
                # CALIB_COEFF_A
                out.append(str(calibCoeffA)), out.append(',')
                # CALIB_COEFF_B
                out.append(str(calibCoeffB)), out.append(',')
                # CALIB_COEFF_C
                out.append(str(calibCoeffC)), out.append(',')
                # COLLECT_TIME_HOST
                out.append(str(calendar.timegm(collectTimeHost.utctimetuple())*1000)), out.append(',')
                # COLLECT_TIME_BUTTON
                out.append(str(calendar.timegm(collectTimeButton.utctimetuple())*1000)), out.append(',')
                # NUM_SAMPLES
                out.append(str(numSamples)), out.append(',')
                # SW_TEMP_CALIB_USED
                out.append(str(tempCalibUsed))
                outputIButtonConfig.write(''.join(out)+'\n')
        
                if numSamples == None:
                    print 'No data for measurement profile ' + str(profile[0]) + '?'
                    continue
                
                samplingDurationNoDrift = samplingRate * (numSamples-1)
                timeShift = (collectTimeButton-collectTimeHost).total_seconds()
                samplingDurationWithDrift = samplingDurationNoDrift+timeShift
                samplingRateReal = float(samplingDurationWithDrift) / (numSamples-1)
                for meas in c3.execute('SELECT MeasurementNr, Measurement FROM Measurements WHERE MeasurementProfileID=?', (profile[0],)):
                    out = []
                    ts = sampleStart+timedelta(0, (int(meas[0])-1)*samplingRateReal)
                    # GENERATION_TIME
                    out.append(str(calendar.timegm(ts.utctimetuple())*1000)), out.append(',')
                    # DEVICE_ID
                    out.append(str(deviceId)), out.append(',')
                    # CONFIG_ID
                    out.append(str(configId)), out.append(',')
                    # SAMPLE_NR
                    out.append(str(meas[0])), out.append(',')
                    # TEMPERATURE
                    out.append(str(meas[1]))
                    outputIButtonMeasurements.write(''.join(out)+'\n')
                    

# Produce position mapping
outputPosMapping.write('"DEVICE_ID","BEGIN","END","POSITION","COMMENT"\n')

# Iterate over all known positions and read all related measurement profiles for finding 
# time boundaries needed
c = conn.cursor()
for position in c.execute('SELECT DISTINCT ButtonNr FROM Buttons WHERE DeploymentID = ? \
    ORDER BY ButtonNr ASC', (results.deployment_id,)):
    c2 = conn.cursor()
    for button in c2.execute('SELECT ButtonID FROM Buttons WHERE DeploymentID = ? AND ButtonNr = ?', (results.deployment_id, position[0])):
        c3 = conn.cursor()
        firstIterationDone = False
        missionStillRunning = False
        deviceId = deviceIdOffset + int(button[0])
        for profile in c3.execute('SELECT SamplingStartTime, CollectingTimeHost FROM MeasurementProfiles WHERE ButtonID = ?', (button[0], )):
            samplingStartTime = datetime.strptime(str(profile[0]), '%d.%m.%Y %H:%M:%S')
            if profile[1] != None and profile[1] != '':
                collectTimeHost = datetime.strptime(str(profile[1]), '%d.%m.%Y %H:%M:%S')
            else:
                missionStillRunning = True
                # Ok for the following comparison
                collectTimeHost = datetime.now()
            
            if firstIterationDone == False:
                startTime = samplingStartTime
                endTime = collectTimeHost         
                firstIterationDone = True
            else:
                if samplingStartTime < startTime:
                    startTime = samplingStartTime
                if collectTimeHost > endTime:
                    endTime = collectTimeHost
        out = []
        # DEVICE_ID
        out.append('"'+str(deviceId)+'"'), out.append(',')
        # BEGIN
        out.append('"'+startTime.strftime('%Y-%m-%d %H:%M:%S')+'"'), out.append(',')
        # END
        if missionStillRunning:
            out.append('"9999-01-01 00:00:00"'), out.append(',')
        else:
            out.append('"'+endTime.strftime('%Y-%m-%d %H:%M:%S')+'"'), out.append(',')
        # POSITION
        out.append('"'+str(position[0])+'"'), out.append(',')
        # COMMENT
        outputPosMapping.write(''.join(out)+'\n')

c.close()
c2.close()
c3.close()
conn.close()
	
outputIButtonMeasurements.close()
outputIButtonConfig.close()
outputPosMapping.close()

print 'iButton config:\t\t\t' + fileNameConfig
print 'iButton measurements:\t\t' + fileNameMeasurements
print 'iButton position mapping:\t' + fileNamePosMapping

