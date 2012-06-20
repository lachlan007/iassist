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
			+ BUTTONTABLENAME + QString(" (ButtonNr char(10), ButtonID char(25), ")
			+ QString(" Timestamp char(30), Distributor char(50), PDOP int, HDOP int,")
			+ QString(" StandardDeviation int, HAE_DEM int, East_DEM int, North_DEM int, Slope_DEM int,")
			+ QString(" SolDy_DEM int, SolYr_DEM int);"));
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
	sqlText = sqlText + BUTTONTABLENAME + " VALUES ( " + ButtonNr + ", " + ButtonID + ", " + Timestamp
		+ ", " + Distributor + ", " + PDOP + ", " + HDOP + ", " + StandardDeviation
		+ ", " + HAE_DEM + ", " + East_DEM + ", " + North_DEM + ", " + Slope_DEM + ", " + SolDy_DEM
		+ ", " + SolYr_DEM + " );";

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
	PDOP = "NULL";
	HDOP = "NULL";
	StandardDeviation = "NULL";
	HAE_DEM = "NULL";
	East_DEM = "NULL";
	North_DEM = "NULL";
	Slope_DEM = "NULL";
	SolDy_DEM = "NULL";
	SolYr_DEM = "NULL";

	ButtonID=QString("'") + button->ButtonID + QString("'");
	ButtonNr=QString("'") + button->ButtonNr + QString("'");
	Distributor=QString("'") + button->Distributor + QString("'");
	Timestamp=QString("'") + QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss") + QString("'");
	if(button->PDOP!=-9999) PDOP = QString::number(button->PDOP, 'f', 2);
	if(button->HDOP!=-9999) HDOP = QString::number(button->HDOP, 'f', 2);
	if(button->StandardDeviation!=-9999) StandardDeviation = QString::number(button->StandardDeviation, 'f', 6);
	if(button->HAE_DEM!=-9999) HAE_DEM = QString::number(button->HAE_DEM, 'f', 1);
	if(button->East_DEM!=-9999) East_DEM = QString::number(button->East_DEM, 'f', 4);
	if(button->North_DEM!=-9999) North_DEM = QString::number(button->North_DEM, 'f', 4);
	if(button->Slope_DEM!=-9999) Slope_DEM = QString::number(button->Slope_DEM, 'f', 8);
	if(button->SolDy_DEM!=-9999) SolDy_DEM= QString::number(button->SolDy_DEM, 'f', 8);
	if(button->SolYr_DEM!=-9999) SolYr_DEM = QString::number(button->SolYr_DEM, 'f', 8);
}

bool DBButtonTable::updateButton(ButtonData button)
{
	convertButtonDataToSQL(&button);
	bool success = true;

	if(Timestamp!="''" && success)
			success = update(BUTTONTABLENAME, "ButtonNr", ButtonNr, "Timestamp", Timestamp);
	if(Distributor!="''" && success)
			success = update(BUTTONTABLENAME, "ButtonNr", ButtonNr, "Distributor", Distributor);
	if(PDOP!="NULL" && success)
			success = update(BUTTONTABLENAME, "ButtonNr", ButtonNr, "PDOP", PDOP);
	if(HDOP!="NULL" && success)
			success = update(BUTTONTABLENAME, "ButtonNr", ButtonNr, "HDOP", HDOP);
	if(StandardDeviation!="NULL" && success)
			success = update(BUTTONTABLENAME, "ButtonNr", ButtonNr, "StandardDeviation", StandardDeviation);
	if(HAE_DEM!="NULL" && success)
			success = update(BUTTONTABLENAME, "ButtonNr", ButtonNr, "HAE_DEM", HAE_DEM);
	if(East_DEM!="NULL" && success)
			success = update(BUTTONTABLENAME, "ButtonNr", ButtonNr, "East_DEM", East_DEM);
	if(North_DEM!="NULL" && success)
			success = update(BUTTONTABLENAME, "ButtonNr", ButtonNr, "North_DEM", North_DEM);
	if(Slope_DEM!="NULL" && success)
			success = update(BUTTONTABLENAME, "ButtonNr", ButtonNr, "Slope_DEM", Slope_DEM);
	if(SolDy_DEM!="NULL" && success)
			success = update(BUTTONTABLENAME, "ButtonNr", ButtonNr, "SolDy_DEM", SolDy_DEM);
	if(SolYr_DEM!="NULL" && success)
			success = update(BUTTONTABLENAME, "ButtonNr", ButtonNr, "SolYr_DEM", SolYr_DEM);

	return success;
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

	temp = this->read(BUTTONTABLENAME, "ButtonNr", this->toSQLString(_ButtonNr), "Distributor");
	if(temp!="") data.Distributor = temp;

	temp = this->read(BUTTONTABLENAME, "ButtonNr", this->toSQLString(_ButtonNr), "East_DEM");
	if(temp!="") data.East_DEM = temp.toDouble();

	temp = this->read(BUTTONTABLENAME, "ButtonNr", this->toSQLString(_ButtonNr), "Timestamp");
	if(temp!="") data.Timestamp = temp;

	temp = this->read(BUTTONTABLENAME, "ButtonNr", this->toSQLString(_ButtonNr), "HAE_DEM");
	if(temp!="") data.HAE_DEM = temp.toDouble();

	temp = this->read(BUTTONTABLENAME, "ButtonNr", this->toSQLString(_ButtonNr), "HDOP");
	if(temp!="") data.HDOP = temp.toDouble();

	temp = this->read(BUTTONTABLENAME, "ButtonNr", this->toSQLString(_ButtonNr), "North_DEM");
	if(temp!="") data.North_DEM = temp.toDouble();

	temp = this->read(BUTTONTABLENAME, "ButtonNr", this->toSQLString(_ButtonNr), "PDOP");
	if(temp!="") data.PDOP = temp.toDouble();

	temp = this->read(BUTTONTABLENAME, "ButtonNr", this->toSQLString(_ButtonNr), "Slope_DEM");
	if(temp!="") data.Slope_DEM = temp.toDouble();

	temp = this->read(BUTTONTABLENAME, "ButtonNr", this->toSQLString(_ButtonNr), "SolDy_DEM");
	if(temp!="") data.SolDy_DEM = temp.toDouble();

	temp = this->read(BUTTONTABLENAME, "ButtonNr", this->toSQLString(_ButtonNr), "SolYr_DEM");
	if(temp!="") data.SolYr_DEM = temp.toDouble();

	temp = this->read(BUTTONTABLENAME, "ButtonNr", this->toSQLString(_ButtonNr), "StandardDeviation");
	if(temp!="") data.StandardDeviation = temp.toDouble();

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


