//---------------------------------------------------------------------------
//  iAssist - Rapid deployment of Maxim iButton(c) miniature logger devices
//  Copyright (C) 2012 ETH Zurich, Computer Engineering and Networks
//      Laboratory, Matthias Keller <kellmatt at tik.ee.ethz.ch>
//  Copyright (C) 2009 Guido Hungerbuehler
//  Copyright (C) 2009 Oliver Knecht
//  Copyright (C) 2009 Suhel Sheikh
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//---------------------------------------------------------------------------
#ifndef DBMEASUREMENTPROFILETABLE_H_
#define DBMEASUREMENTPROFILETABLE_H_

#define MEASUREMENTPROFILETABLENAME "MeasurementProfiles"

#include "DBConnection.h"
#include "../MeasurementProfile.h"
#include <QVector>

/**
 * This class provides access to the table MeasurementProfiles in the database.
 * It supplies all necessary functions to edit and read the database.
 */


class DBMeasurementProfileTable : public DBConnection{
public:
	/**
	 * Default constructor
	 */
	DBMeasurementProfileTable();
	/**
	 * Default deconstructor
	 */
	virtual ~DBMeasurementProfileTable();

	/**
	 * Creates a %MeasurementProfile table in the database
	 */
	bool createTable();

	/**
	 * Adds a %MeasurementProfile to the Database. If this record is already existing it returns false.
	 * @param profile The profile to add to the Database
	 * @return true if successfully done, false otherwise
	 */
	bool addProfile(MeasurementProfile profile);

	/**
	 * Update a %MeasurementProfile in the Database. It updates the record which correspond to the MeasurementID.
	 * Only the values are updated which are set by the user
	 * @param profile the Data which should be updated in the database.
	 * @return true if successfully done, false otherwise.
	 */
	bool updateProfile(MeasurementProfile profile);


	/**
	 * Reads a profile according to a ID from the database and returns it as a %MeasurementProfile object.
	 * @param _measurementProfileID the id of the desired profile in the database
	 * @return a MeasurementProfile object
	 */
	MeasurementProfile readProfile(QString _measurementProfileID);

	int getLatestAddedProfileIDByButtonId(int buttonId);

	int getLatestSessionNrByButtonId(int buttonId);

	/**
	 * Opens the database connection. Adds a table if no one is existing and creates
	 * a database if no one is existing.
	 * @return true if successful, false otherwise
	 */
	bool open();

	/**
	 * Deletes the Measurement Profiles to a certain button from the database.
	 * @param _buttonNr the buttonNr
	 * @return true if successful, false otherwise
	 */
	bool deleteProfileByButtonId(int buttonId);

	/**
	 * Returns measurement profiles for which the related measurement data is
	 * already in the database.
	 * @return Vector
	 */
	QVector<MeasurementProfile> getFinishedMeasurementProfiles();

	QVector<MeasurementProfile> getProfilesByButtonID(int buttonId);

private:
	/**
	 * Converts all the values of a MeasurementProfile to a SQLite representation
	 * @param profile a pointer to the MeasurementProfile Object
	 */
	void convertDataToSQL(MeasurementProfile *profile);

    /**
     * Reads one specific field in the database. If more than one hit is found, only the first
     * will be returned. It represents the following SQL statement:
     * SELECT getCOL FROM table WHERE compCol=compVal
     * @param table The table name of the DB
     * @param compCol the column which will be compared to compVal
     */
	QString read(QString table, QString compCol, QString compVal, QString getCol);

	// all the variables of the DBMeasurementProfileTable
	QString ButtonId;
	QString SessionNr;
	QString MeasurementProfileID;
	QString ProgrammingTime;
	QString CollectingTimeHost;
	QString CollectingTimeButton;
	QString SamplingRate;
	QString SamplingStartTime;
	QString Resolution;
	QString TempCalibUsed;

};

#endif /* DBMEASUREMENTPROFILETABLE_H_ */
