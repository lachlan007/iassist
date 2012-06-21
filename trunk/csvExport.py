#!/usr/bin/python

import sqlite3
from datetime import datetime, timedelta

conn = sqlite3.connect('iButtonDB')
c = conn.cursor()

fileName = datetime.now().strftime('%Y%m%d%H%M%S')+'-export.txt'
csvOutput = open(fileName, 'w')

csvOutput.write('# TIME,TEMP,BUTTONNR,SESSIONNR,RESOLUTION'+'\n')

sqlStmt = 'SELECT MeasurementProfileID, ButtonNr, SessionNr, SamplingStartTime, SamplingRate, TimeShift, Resolution FROM MeasurementProfiles'
for row in c.execute(sqlStmt):
	t = (row[0],)
	c.execute('SELECT MAX(MeasurementNr) FROM Measurement WHERE MeasurementProfileID=?', t)
	# Clock drift compensation
	numMeas = c.fetchone()[0]
	sampleStart = datetime.strptime(str(row[3]), '%d.%m.%Y %H:%M:%S')
	samplingRate = int(row[4])
	samplingDurationNoDrift = samplingRate * (numMeas-1)
	timeShift = int(row[5])
	samplingDurationWithDrift = samplingDurationNoDrift+timeShift
	samplingRateReal = float(samplingDurationWithDrift) / (numMeas-1)
	for row2 in c.execute('SELECT MeasurementNr,Measurement FROM Measurement WHERE MeasurementProfileID=?', t):
		out = []
		ts = sampleStart+timedelta(0, (int(row2[0])-1)*samplingRateReal)
		# Timestamp
		out.append(ts.strftime('%d.%m.%Y %H:%M:%S')), out.append(',')
		# Measured temperature
		out.append(str(row2[1])), out.append(',')
		# Button Nr
		out.append(str(row[1])), out.append(',')
		# Session Nr
		out.append(str(row[2])), out.append(',')
		# Resolution
		out.append(str(row[6]))
		csvOutput.write(''.join(out)+'\n')

csvOutput.close()

print 'Export written to ' + fileName