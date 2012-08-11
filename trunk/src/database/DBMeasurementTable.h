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
#ifndef DBMEASUREMENTTABLE_H_
#define DBMEASUREMENTTABLE_H_

#define MEASUREMENTTABLENAME "Measurement"

#include "DBConnection.h"
#include "../Measurement.h"
#include <QtCore>

/**
 * This class provides access to the table MeasurementTable in the database.
 * It supplies all necessary functions to edit and read the database.
 */

class DBMeasurementTable : public DBConnection {
public:
	/**
	 * Default constructor
	 */
	DBMeasurementTable();
	/**
	 * Default deconstructor
	 */
	virtual ~DBMeasurementTable();

	/**
	 * Creates a new %Measurement Table in the database, if no %Measurement Table is existing
	 * @return true if successful, false otherwise
	 */
	bool createTable();

	/**
	 * Deletes a %Measurement to a certain ButtonNr
	 * @param buttonNr Nr of the button which measurements should be deleted
	 * @return true or false for the success state
	 */
	bool deleteMeasurementByButtonNr(QString buttonNr);

	/**
	 * Deletes a %Measurement to a certain %Area (Footprint)
	 * @param area FootprintID which measurements should be deleted
	 * @return true or false for the success state
	 */
	bool deleteMeasurementByArea(QString area);

	/**
	 * Adds a %Measurement to a of a certain Button to the %Measurement Table of the Database
	 * @param meas Measurement object which contains the measurements
	 * @return true if successful, false otherwise
	 */
	bool addMeasurement(Measurement meas);

	/**
	 * Opens the Database. If no Database is existing, it creates one first. And if the
	 * table is not existing it creates a table as well.
	 */
	bool open();

	/**
	 * Returns the latest used MeasurementProfileID to a certain ButtonNr. MeasurementProfileID
	 * is the link to the detail information of a certain measurement session.
	 * @param buttonNr buttonNr of the button we want to have the latest MeasurementProfileID
	 * @return the MeasurementProfileID of a certain Button's latest measurement Session. Returns -1 for Error.
	 */
	int getLatestMeasurementProfileID(QString buttonNr);

	/**
	 * Returns measured values for a measurement with a specific profile ID
	 * @param measurementProfileID measurement profile ID of requested measurement
	 * @param values Pointer for returning the measurements
	 * @return Number of returned measurements, or -1 on error
	 */
	int getMeasurementsByProfileID(QString measurementProfileID, double **values);

};

#endif /* DBMEASUREMENTTABLE_H_ */
