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
#ifndef DBFOOTPRINTTABLE_H_
#define DBFOOTPRINTTABLE_H_

#define FOOTPRINTTABLENAME	"Footprints"

#include "DBConnection.h"

/**
 * This class provides access to the table Footprints in the database.
 * It supplies all necessary functions to edit and read the database.
 */
class DBFootprintTable : public DBConnection {
public:
	/**
	 * Default constructor
	 */
	DBFootprintTable(int deploymentId);
	/**
	 * Default deconstructor
	 */
	virtual ~DBFootprintTable();

	/**
	 * Creates a new Footprint Table, if there's no one existing
	 */
	bool createTable();

	/**
	 * Adds a Footprint to the Database
	 * @param footprintPrefix name of the footprint
	 * @return true or false for the success state
	 */
	bool addFootprint(int footprintPrefix);

	/**
	 * Deletes a certain Footprint out of the Footprint (%Footprint) Table of the Database.
	 * Attention: Buttons, Photos and Measurements have to be deleted separately.
	 * @param footprintPrefix are which should be deleted
	 * @return true or false for the success state
	 */
	bool deleteFootprint(int footprintPrefix);

	/**
	 * Returns if the specified %Footprint is already entered in the Footprint Table of the iButton Database
	 * @return true if found at least once, false if not found
	 */
	bool isFootprintExisting(int footprintPrefix);

	/**
	 * Returns QStringList with all the Footprints (FootprintPrefix).
	 * @return a list with all the FootprintPrefix
	 */
	QVector<int> getAllFootprints();

	/**
	 * Opens the Database. If no Database is existing, it creates one first. And if the
	 * table is not existing it creates a table as well.
	 */
	bool open();

private:

	int deploymentId;

};

#endif /* DBFOOTPRINTTABLE_H_ */
