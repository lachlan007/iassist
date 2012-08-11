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
#ifndef DBAREATABLE_H_
#define DBAREATABLE_H_

#define AREATABLENAME	"Footprints"

#include "DBConnection.h"

/**
 * This class provides access to the table Footprints in the database.
 * It supplies all necessary functions to edit and read the database.
 */
class DBAreaTable : public DBConnection {
public:
	/**
	 * Default constructor
	 */
	DBAreaTable();
	/**
	 * Default deconstructor
	 */
	virtual ~DBAreaTable();

	/**
	 * Creates a new Footprint Table, if there's no one existing
	 */
	bool createTable();

	/**
	 * Adds a Footprint to the Database
	 * @param footprintID name of the footprint
	 * @return true or false for the success state
	 */
	bool addArea(QString footprintID);

	/**
	 * Deletes a certain Footprint out of the Footprint (%Area) Table of the Database.
	 * Attention: Buttons, Photos and Measurements have to be deleted separately.
	 * @param footprintID are which should be deleted
	 * @return true or false for the success state
	 */
	bool deleteArea(QString footprintID);

	/**
	 * Returns if the specified %Area is already entered in the Footprint Table of the iButton Database
	 * @param area %Area to look for
	 * @return true if found at least once, false if not found
	 */
	bool isAreaExisting(QString footprintID);

	/**
	 * Returns QStringList with all the Areas (Footprint ID).
	 * @return a list with all the FootprintIDs
	 */
	QStringList getAllArea();

	/**
	 * Opens the Database. If no Database is existing, it creates one first. And if the
	 * table is not existing it creates a table as well.
	 */
	bool open();

};

#endif /* DBAREATABLE_H_ */
