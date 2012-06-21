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
#include "DBAreaTable.h"

DBAreaTable::DBAreaTable() {

}

DBAreaTable::~DBAreaTable() {

}

bool DBAreaTable::createTable()
{
	// check if a table exists
	if(this->isTableExisting(AREATABLENAME))
	{
		return true;
	}
	else
	{
		QSqlQuery query(this->getDB());
		bool success = query.exec(QString("CREATE TABLE ")
				+ AREATABLENAME + QString(" (FootprintID char(10));"));
		if(success)
		{
			Log::write("Created new Footprint Table in Database");
			return true;
		}
		else
		{
			QString error = query.lastError().text();
			this->appendError("Cannot create Footprint table: " + error);
			Log::writeError("dbAreaTable: Cannot create table: " + error);
			return false;
		}
	}
}

bool DBAreaTable::addArea(Area areaData)
{
	if(this->isAreaExisting(areaData.AreaID))
	{
		Log::writeError("dbAreaTable: Cannot add Area twice: " + areaData.AreaID);
		this->appendError("Cannot add Area twice: " + areaData.AreaID);
		return false;
	}

	QSqlQuery query(this->getDB());

	convertAreaDataToSQL(&areaData);

	QString text = "INSERT INTO " + QString(AREATABLENAME) + " VALUES ( " + AreaID + " );";
	bool success = query.exec(text);

	if(!success)
	{
		QString error = query.lastError().text();
		Log::writeError("dbAreaTable: Cannot add Area: " + areaData.AreaID + " " + error);
		this->appendError("Cannot add Area: " + areaData.AreaID);
		return false;
	}
	else
	{
		return true;
	}
}

void DBAreaTable::convertAreaDataToSQL(Area *area)
{
	// Doubles and ints have to be "NULL" for the SQL Statement
	AreaID="'" + area->AreaID + "'";
}

bool DBAreaTable::deleteArea(QString area)
{
	QSqlQuery query(this->getDB());
	QString text = QString("DELETE FROM ") + AREATABLENAME
			+ QString(" WHERE FootprintID LIKE '") + area + QString("';");

	bool success = query.exec(text);

	if(!success)
	{
		QString error = query.lastError().text();
		Log::writeError("dbAreaTable: Cannot delete Area: " + area + " " + error);
		this->appendError("Cannot delete Area: " + area);
		return false;
	}
	else
	{
		return true;
	}
}

bool DBAreaTable::isAreaExisting(QString area)
{
	// Ask DB for this area
	QSqlQuery query(this->getDB());
	QString text = "SELECT FootprintID FROM " + QString(AREATABLENAME)
		+ " WHERE FootprintID LIKE '" + area + "'";

	bool success = query.exec(text);

	if(!success)
	{
		QString error = query.lastError().text();
		Log::writeError("dbAreaTable: Cannot detect if Area " + area + " already entered: " + error);
		this->appendError("Cannot detect if Area existing: " + area);
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

QStringList DBAreaTable::getAllArea()
{
	QSqlQuery query (this->getDB());
	QStringList list;

	QString text = "SELECT FootprintID FROM " + QString(AREATABLENAME) + ";";
	//qDebug(text.toStdString().c_str());
	bool success = query.exec(text);

	if(!success)
	{
		QString error = query.lastError().text();
		Log::writeError("dbAreaTable: Cannot get all Area as a list: " + error);
		this->appendError("Cannot get Area list.");
	}
	else
	{
		while(query.next())
		{
			list.append(query.value(0).toString());
		}
	}
	return list;
}

Area DBAreaTable::readArea(QString footprintID)
{
	Area data;
	QString temp;
	QString sqlFootprintID = this->toSQLString(footprintID);

	// Get all the data and save them to ButtonData
	temp = this->read(AREATABLENAME, "FootprintID", sqlFootprintID, "FootprintID");
	if(temp!="") data.AreaID = temp;

	return data;
}

bool DBAreaTable::open()
{
    if(!this->openDatabase())
    {
        return false;
    }

    if(!this->isTableExisting(AREATABLENAME))
    {
        return this->createTable();
    }

    return true;
}
