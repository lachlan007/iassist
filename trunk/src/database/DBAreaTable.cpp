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

bool DBAreaTable::addArea(QString footprintID)
{
	if(this->isAreaExisting(footprintID))
	{
		Log::writeError("dbAreaTable: Cannot add Area twice: " + footprintID);
		this->appendError("Cannot add Area twice: " + footprintID);
		return false;
	}

	QSqlQuery query(this->getDB());

	QString text = "INSERT INTO " + QString(AREATABLENAME) + " VALUES ( '" + footprintID + "');";
	bool success = query.exec(text);

	if(!success)
	{
		QString error = query.lastError().text();
		Log::writeError("dbAreaTable: Cannot add Area: " + footprintID + " " + error);
		this->appendError("Cannot add Area: " + footprintID);
		return false;
	}
	else
	{
		return true;
	}
}


bool DBAreaTable::deleteArea(QString footprintID)
{
	QSqlQuery query(this->getDB());
	QString text = QString("DELETE FROM ") + AREATABLENAME
			+ QString(" WHERE FootprintID LIKE '") + footprintID + QString("';");

	bool success = query.exec(text);

	if(!success)
	{
		QString error = query.lastError().text();
		Log::writeError("dbAreaTable: Cannot delete Area: " + footprintID + " " + error);
		this->appendError("Cannot delete Area: " + footprintID);
		return false;
	}
	else
	{
		return true;
	}
}

bool DBAreaTable::isAreaExisting(QString footprintID)
{
	// Ask DB for this area
	QSqlQuery query(this->getDB());
	QString text = "SELECT FootprintID FROM " + QString(AREATABLENAME)
		+ " WHERE FootprintID LIKE '" + footprintID + "'";

	bool success = query.exec(text);

	if(!success)
	{
		QString error = query.lastError().text();
		Log::writeError("dbAreaTable: Cannot detect if Area " + footprintID + " already entered: " + error);
		this->appendError("Cannot detect if Area existing: " + footprintID);
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
