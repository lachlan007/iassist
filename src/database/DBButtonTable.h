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
#ifndef DBBUTTONTABLE_H_
#define DBBUTTONTABLE_H_

#define BUTTONTABLENAME	"Buttons"

#include "DBConnection.h"
#include "../ButtonData.h"

/**
 * This class provides access to the table iButton in the database.
 * It supplies all necessary functions to edit and read the database.
 */

class DBButtonTable : public DBConnection {

public:
	DBButtonTable(int deploymentId);
	virtual ~DBButtonTable();

	/**
	 * Creates a new Table to store information about iButtons
	 */
	bool createTable();

	/**
	 * Adds a Button to the Database
	 * @param button a ButtonData Object which contains all the information to store in the database
	 * @return true if successfully added to the database, false otherwise
	 */
	bool addButton(ButtonData button, int &insertId);

	/**
	 * Returns if the specified Button is existing in the iButton Table of the Database
	 * @param buttonNr the button Number of a specified Button
	 * @return true if found at least once, fals if not found
	 */
	//bool isButtonExistingByButtonId(QString buttonNr);

	/**
	 * Returns if the specified Button is existing in the iButton Table of the Database
	 * @param buttonNr the button's ID of a specified Button
	 * @return true if found at least once, false if not found
	 */
	bool isButtonExistingBySerialNr(QString serialNr);

	ButtonData getButtonBySerialNr(QString serialNr);

	ButtonData getButtonByButtonNr(QString buttonNr);

	ButtonData getButtonByButtonId(int buttonId);

	/**
	 * Returns a list of all the Buttons (ButtonNr) which are entered to the Database
	 * @return QStringList with all the ButtonNr in it
	 */
	QStringList getAllButtonNr();

	/**
	 * Returns a list of all the Buttons (ButtonNr) to a certain %Area
	 * @return QStringList with all the ButtonNr to a certain %Area
	 */
	QStringList getAllButtonNr(QString _area);

	/**
	 * Returns the highest Number of all the Buttons to a certain area (e.g. AA004 -> 4)
	 * @param _area the area, we want to have the highest ButtonNr
	 * @return The highest Number of all the Buttons which are in the Database
	 */
	int getLastAddedButtonNrInt(QString _area);

	bool deleteButtonByButtonId(int buttonId);

	/**
	 * Deletes all buttons which belong to a certain %Area.
	 * @param area The %Area ID (e.g. AA)
	 * @return true if successfully deleted, false otherwise
	 */
	bool deleteButtonByArea(QString area);

	/**
	 * Opens the Database. If no Database is existing, it creates one first. And if the
	 * table is not existing it creates a table as well.
	 */
	bool open();

	int getButtonIdBySerialNr(QString serialNr);

    int getButtonIdByButtonNr(QString buttonNr);

    /**
     * Reads one specific field in the database. If more than one hit is found, only the first
     * will be returned. It represents the following SQL statement:
     * SELECT getCOL FROM table WHERE compCol=compVal
     * @param table The table name of the DB
     * @param compCol the column which will be compared to compVal
     */
    QString read(QString table, QString compCol, QString compVal, QString getCol);


private:
	int deploymentId;

	void convertButtonDataToSQL(ButtonData *button);

	//iButton Table in iButtonDB
    QString ButtonNr;
    QString SerialNr;
 };

#endif /* DBBUTTONTABLE_H_ */
