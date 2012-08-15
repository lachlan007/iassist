#!/usr/bin/python

import sqlite3
from datetime import datetime, timedelta
import argparse
import sys

parser = argparse.ArgumentParser(description='Export iButton data to a CSV file.')
parser.add_argument('-d', action='store', dest='deployment_id',
                    help='Deployment ID', required=True, type=int)

results = parser.parse_args()

fileName = datetime.now().strftime('%Y%m%d%H%M%S')+'-export.txt'
csvOutput = open(fileName, 'w')

csvOutput.write('# TIME,TEMP,BUTTONNR,SESSIONNR,RESOLUTION'+'\n')

conn = sqlite3.connect('iassist.db')
c = conn.cursor()
for button in c.execute('SELECT ButtonID, ButtonNr FROM Buttons WHERE DeploymentID = ?', (results.deployment_id, )):
	c2 = conn.cursor()
	for profile in c2.execute('SELECT MeasurementProfileID, SessionNr, SamplingStartTime, '
		'SamplingRate, TimeShift, Resolution, CollectingTime '
		'FROM MeasurementProfiles WHERE ButtonID = ?', (button[0],)):
		# Not yet collected
		if profile[6] == None or profile[6] == "":
			continue
		t = (profile[0],)
		c3 = conn.cursor()
		c3.execute('SELECT MAX(MeasurementNr) FROM Measurement WHERE MeasurementProfileID=?', t)
		# Clock drift compensation
		numMeas = c3.fetchone()[0]
		if numMeas == None:
			print 'No data for button ' + row[1] + '?'
			continue;
		sampleStart = datetime.strptime(str(profile[2]), '%d.%m.%Y %H:%M:%S')
		samplingRate = int(profile[3])
		samplingDurationNoDrift = samplingRate * (numMeas-1)
		timeShift = int(profile[4])
		samplingDurationWithDrift = samplingDurationNoDrift+timeShift
		samplingRateReal = float(samplingDurationWithDrift) / (numMeas-1)
		for meas in c3.execute('SELECT MeasurementNr,Measurement FROM Measurement WHERE MeasurementProfileID=?', t):
			out = []
			ts = sampleStart+timedelta(0, (int(meas[0])-1)*samplingRateReal)
			# Timestamp
			out.append(ts.strftime('%d.%m.%Y %H:%M:%S')), out.append(',')
			# Measured temperature
			out.append(str(meas[1])), out.append(',')
			# Button Nr
			out.append(str(button[1])), out.append(',')
			# Session Nr
			out.append(str(profile[2])), out.append(',')
			# Resolution
			out.append(str(profile[5]))
			csvOutput.write(''.join(out)+'\n')
		c3.close()
	c2.close()
c.close()
conn.close()
	
csvOutput.close()

print 'Export written to ' + fileName