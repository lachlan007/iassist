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
#include "DBButtonTable.h"

DBButtonTable::DBButtonTable() {

}

DBButtonTable::~DBButtonTable() {

}

bool DBButtonTable::createTable()
{
	// check if a table exists
	if(this->isTableExisting(BUTTONTABLENAME))
	{
		return true;
	}
	else
	{
		QSqlQuery query(this->getDB());
		bool success = query.exec(QString("CREATE TABLE ")
			+ BUTTONTABLENAME + QString(" (ButtonNr char(10), ButtonID char(25));"));
		if(success)
		{
			Log::write("Created new iButton Table in Database");
			return true;
		}
		else
		{
			QString error = query.lastError().text();
			this->appendError("Cannot create table: " + error);
			Log::writeError("dbButtonTable: Cannot create table: " + error);
			return false;
		}
	}
}

bool DBButtonTable::addButton(ButtonData button)
{
	// Create the Query to add a complete iButton Database entry to the database
	QSqlQuery query(this->getDB());

	// Check if Button is already entered
	if(this->isButtonExistingByButtonNr(button.ButtonNr))
	{
		Log::writeError("dbButtonTable: Cannot add a button twice.");
		this->appendError("Cannot add a button twice. " + button.ButtonNr + " already in DB");
		return false;
	}

	// Convert the ButtonData to SQL Format
	convertButtonDataToSQL(&button);

	QString sqlText("INSERT INTO ");
	sqlText = sqlText + BUTTONTABLENAME + " VALUES ( " + ButtonNr + ", " + ButtonID + ");";

	// Send the query
	bool success = query.exec(sqlText);

	if(!success)
	{
		QString error = query.lastError().text();
		Log::writeError("dbButtonTable: Cannot add Button to Database: " + error);
		this->appendError("Cannot add Button to Database: " + error);
	}

	query.clear();
	return success;
}

void DBButtonTable::convertButtonDataToSQL(ButtonData *button)
{
	// Give all the digits "NULL" as value.
	ButtonID=QString("'") + button->ButtonID + QString("'");
	ButtonNr=QString("'") + button->ButtonNr + QString("'");
}

ButtonData DBButtonTable::readButton(QString _ButtonNr)
{
	ButtonData data;
	QString temp;

	// Get all the data and save them to ButtonData

	temp = this->read(BUTTONTABLENAME, "ButtonNr", this->toSQLString(_ButtonNr), "ButtonNr");
	if(temp!="") data.ButtonNr = temp;

	temp = this->read(BUTTONTABLENAME, "ButtonNr", this->toSQLString(_ButtonNr), "ButtonID");
	if(temp!="") data.ButtonID = temp;

	return data;
}

QStringList DBButtonTable::getAllButtonNr()
{
	QStringList list;
	QSqlQuery query(this->getDB());

	QString text = QString("SELECT ButtonNr FROM ") + QString(BUTTONTABLENAME) + QString(";");
	bool success = query.exec(text);

	if(success)
	{
		while(query.next())
		{
			list.append(query.value(0).toString());
		}
	}
	else
	{
		Log::writeError("dbButtonTable: Cannot read all Buttons: " + query.lastError().text());
		this->appendError("Cannot read all Buttons to a list.");
	}
	return list;
}

QStringList DBButtonTable::getAllButtonNr(QString _area)
{
	QStringList list;
	QSqlQuery query(this->getDB());

	QString text = QString("SELECT ButtonNr FROM ") + QString(BUTTONTABLENAME) + " WHERE ButtonNr LIKE '" + _area + "%';";
	bool success = query.exec(text);

	if(success)
	{
		while(query.next())
		{
			list.append(query.value(0).toString());
		}
	}
	else
	{
		Log::writeError("dbButtonTable: Cannot read all Buttons: " + query.lastError().text());
		this->appendError("Cannot read all Buttons to a list.");
	}
	return list;
}

int DBButtonTable::getLastAddedButtonNrInt(QString _area)
{
	QSqlQuery query(this->getDB());
	QString text = QString("SELECT ButtonNr FROM ") + BUTTONTABLENAME + QString(" WHERE ButtonNr LIKE '") + _area + QString("%';");

	int nr = 0;
	QString tempStr = "";
	int temp = 0;
	query.exec(text);

	while(query.next())
	{
		tempStr = query.value(0).toString();
		temp = tempStr.right(3).toInt();
		if(temp > nr)
		{
			nr = temp;
		}
	}
	return nr;
}

bool DBButtonTable::deleteButtonByButtonNr(QString buttonNr)
{
	bool success;

	QSqlQuery query(this->getDB());
	QString text = QString("DELETE FROM ") + BUTTONTABLENAME
			+ QString(" WHERE ButtonNr='") + buttonNr + QString("';");
	success = query.exec(text);

	if(!success)
	{
		Log::writeError("dbButtonTable: Cannot delete Button: " + buttonNr
				+ " / Error indicated: " + query.lastError().text());
		this->appendError("Cannot delete Button: " + buttonNr);
		return false;
	}
	else
	{
		return true;
	}
}

bool DBButtonTable::deleteButtonByArea(QString area)
{
	bool success;

	QSqlQuery query(this->getDB());
	QString text = QString("DELETE FROM ") + BUTTONTABLENAME
			+ QString(" WHERE ButtonNr LIKE '") + area + QString("%';");
	success = query.exec(text);

	if(!success)
	{
		Log::writeError("dbButtonTable: Cannot delete Buttons of Area: " + area
				+ " / Error indicated: " + query.lastError().text());
		this->appendError("Cannot delete Buttons of Area: " + area);
		return false;
	}
	else
	{
		return true;
	}
}

bool DBButtonTable::isButtonExistingByButtonNr(QString buttonNr)
{
	// Ask DB for this ButtonNr
	QSqlQuery query(this->getDB());
	QString text = QString("SELECT ButtonNr FROM ") + QString(BUTTONTABLENAME)
		+ QString(" WHERE ButtonNr LIKE '") + QString(buttonNr + "'");
	bool success = query.exec(text);

	if(!success)
	{
		Log::writeError("dbButtonTable: Cannot check if Button is existing: " + query.lastError().text());
		this->appendError("Cannot check if Button is existing in DB");
		return false;
	}
	// Workaround because SQLite does not support size of query
	int size = 0;
	while(query.next())
	{
		size++;
	}

	// Return true if it was found at least once
	if(size>=1)
		return true;
	else
		return false;
}

bool DBButtonTable::isButtonExistingByButtonID(QString buttonID)
{
	// Ask DB for this ButtonNr
	QSqlQuery query(this->getDB());
	QString text = QString("SELECT ButtonNr FROM ") + QString(BUTTONTABLENAME)
		+ QString(" WHERE ButtonNr LIKE '") + QString(buttonID + "'");
	bool success = query.exec(text);

	if(!success)
	{
		Log::writeError("dbButtonTable: Cannot check if Button is existing: " + query.lastError().text());
		this->appendError("Cannot check if Button is existing in DB");
		return false;
	}
	// Workaround because SQLite does not support size of query
	int size = 0;
	while(query.next())
	{
		size++;
	}

	// Return true if it was found at least once
	if(size>=1)
		return true;
	else
		return false;
}

bool DBButtonTable::open()
{
	if(!this->openDatabase())
	{
	    return false;
	}

	if(!this->isTableExisting(BUTTONTABLENAME))
	{
	    return this->createTable();
	}

	return true;
}


