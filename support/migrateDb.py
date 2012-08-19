#!/usr/bin/env python

"""migrateDb.py: Migrate SQLite databases from older version of iAssist."""

__author__      = "Matthias Keller"
__copyright__   = "Copyright 2012, Computer Engineering and Networks Laboratory, ETH Zurich"
__license__     = "GPL"

import sqlite3
from datetime import datetime, timedelta
import argparse

parser = argparse.ArgumentParser(description='Convert iAssist DB to new format.')
parser.add_argument('-d', action='store', dest='deploymentName',
                    help='Deployment name', required=True, type=str)
parser.add_argument('-o', action='store', dest='oldDatabaseFile',
                    help='Old database file', required=True, type=str)

results = parser.parse_args()

connOld = sqlite3.connect(results.oldDatabaseFile)
connNew = sqlite3.connect('iassist.db')

cNew = connNew.cursor()
cOld = connOld.cursor()

# Create tables in new database
stmt = "CREATE TABLE IF NOT EXISTS Deployments (DeploymentID INTEGER PRIMARY KEY, Name char(100));"
cNew.execute(stmt)
stmt = "CREATE TABLE IF NOT EXISTS Buttons (ButtonID INTEGER PRIMARY KEY, DeploymentID INTEGER, \
	ButtonNr INTEGER, SerialNr char(25), CalibCoeffA REAL, CalibCoeffB REAL, CalibCoeffC REAL, \
	FOREIGN KEY (DeploymentID) REFERENCES Deployments(DeploymentID));"
cNew.execute(stmt)
stmt = "CREATE TABLE IF NOT EXISTS Footprints (DeploymentID INTEGER, FootprintPrefix INTEGER, FOREIGN \
		KEY(DeploymentID) REFERENCES Deployments(DeploymentID));"
cNew.execute(stmt)
stmt = "CREATE TABLE IF NOT EXISTS Measurements (MeasurementID INTEGER PRIMARY KEY, MeasurementProfileID \
		INTEGER, MeasurementNr INTEGER, Measurement REAL, FOREIGN KEY(MeasurementProfileID)  REFERENCES \
		MeasurementProfiles(MeasurementProfileID));"
cNew.execute(stmt)
stmt = "CREATE TABLE IF NOT EXISTS MeasurementProfiles (MeasurementProfileID INTEGER PRIMARY KEY, ButtonID \
		INTEGER, SessionNr int, ProgrammingTime char(20), CollectingTimeHost char(20), CollectingTimeButton \
		char(20), SamplingRate int, SamplingStartTime char(20), HighResolutionEn int, TempCalibUsed int, FOREIGN \
		KEY(ButtonID) REFERENCES Buttons(ButtonID));"
cNew.execute(stmt)

# Create new deployment
stmt = "INSERT INTO Deployments (Name) VALUES (?)"
cNew.execute(stmt, (results.deploymentName, ))
connNew.commit()
deploymentId = cNew.lastrowid
print "Created deployment ID " + str(deploymentId)

footprintPrefix = 0
for footprint in cOld.execute("SELECT FootprintID FROM Footprints ORDER BY FootprintID ASC"):
	footprintPrefix += 1
	cNew.execute("INSERT INTO Footprints (DeploymentID, FootprintPrefix) VALUES (?, ?)", (deploymentId, footprintPrefix))
	cOld2 = connOld.cursor()
	for button in cOld2.execute("SELECT ButtonNr, ButtonID FROM iButtons WHERE ButtonNr LIKE ?", (str(footprint[0])+'%',)):
		buttonNr = str(button[0])
		cNew.execute("INSERT INTO Buttons (DeploymentID, ButtonNr, SerialNr) VALUES (?, ?, ?)", (deploymentId, 
			footprintPrefix*1000+int(buttonNr[2:5]), str(button[1]).strip().replace(' ','')))
		buttonId = cNew.lastrowid
		cOld3 = connOld.cursor()
		for profile in cOld3.execute("SELECT MeasurementProfileID, ButtonNr, SessionNr, DistributingTime, CollectingTime, "
								"TimeShift, SamplingRate, SamplingStartTimeStamp, Resolution FROM MeasurementProfiles WHERE "
								"ButtonNr LIKE ?", (str(button[0]),)):
			if profile[4] != None and str(profile[4]) != "":
				collectingTimeHost = datetime.strptime(str(profile[4]), '%d.%m.%Y %H:%M:%S')
				if profile[5] != None and int(profile[5]) > -20000 and int(profile[5]) < 60000:
					collectingTimeButton = collectingTimeHost + timedelta(0, int(profile[5]))
				else:
					collectingTimeButton = collectingTimeHost
				collectingTimeButtonStr = collectingTimeButton.strftime('%d.%m.%Y %H:%M:%S')
			else:
				collectingTimeButtonStr = ''
			cNew.execute("INSERT INTO MeasurementProfiles (ButtonID, SessionNr, ProgrammingTime, CollectingTimeHost, "
								"CollectingTimeButton, SamplingStartTime, SamplingRate, TempCalibUsed) VALUES (?, ?, ?, "
								"?, ?, ?, ?, 0)", (buttonId, profile[2], str(profile[3]), str(profile[4]), collectingTimeButtonStr, 
								str(profile[7]), profile[6]))
			profileId = cNew.lastrowid
			cOld4 = connOld.cursor()
			# Check if Measurement table exists, is only created once buttons are collected
			cOld4.execute("SELECT name FROM sqlite_master WHERE type='table' AND name=?", ('Measurement',))
			n = cOld4.fetchone()
			if n == None:
				connNew.commit()
				continue
			for measurement in cOld4.execute("SELECT MeasurementNr, Measurement FROM Measurement WHERE MeasurementProfileID = ?", 
												(profile[0], )):
				cNew.execute("INSERT INTO Measurements (MeasurementProfileID, MeasurementNr, Measurement) VALUES (?, ?, ?)", 
								(profileId, measurement[0], measurement[1]))
			cOld4.close()
			connNew.commit()
		cOld3.close()
		print "Imported button "+str(button[0])
	cOld2.close()

cNew.close()
connNew.close()

# Show mapping between old and new footprints
print "# -------------------------------"
print "# DEPLOYMENT: " + results.deploymentName
footprintPrefix = 0
for footprint in cOld.execute("SELECT FootprintID FROM Footprints ORDER BY FootprintID ASC"):
	footprintPrefix += 1
	print "# " + footprint[0] + " -> " + '{:03d}'.format(footprintPrefix)

print "# -------------------------------"

cOld.close()
connOld.close()


