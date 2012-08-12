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
#include "DBFootprintTable.h"

DBFootprintTable::DBFootprintTable(int deploymentId) : DBConnection() {
    this->deploymentId = deploymentId;
}

DBFootprintTable::~DBFootprintTable() {

}

bool DBFootprintTable::createTable()
{
	// check if a table exists
	if(this->isTableExisting(FOOTPRINTTABLENAME))
	{
		return true;
	}
	else
	{
		QSqlQuery query(this->getDB());
		bool success = query.exec(QString("CREATE TABLE ") +
		        FOOTPRINTTABLENAME + QString(" (FootprintID char(10), DeploymentID INTEGER, FOREIGN ") +
		        QString("KEY(DeploymentID) REFERENCES Deployments(DeploymentID));"));
		if(success)
		{
			Log::write("Created new Footprint Table in Database");
			return true;
		}
		else
		{
			QString error = query.lastError().text();
			this->appendError("Cannot create Footprint table: " + error);
			Log::writeError("dbFootprintTable: Cannot create table: " + error);
			return false;
		}
	}
}

bool DBFootprintTable::addFootprint(QString footprintID)
{
	if(this->isFootprintExisting(footprintID))
	{
		Log::writeError("dbFootprintTable: Cannot add Footprint twice: " + footprintID);
		this->appendError("Cannot add Footprint twice: " + footprintID);
		return false;
	}

	QSqlQuery query(this->getDB());

	QString text = "INSERT INTO " + QString(FOOTPRINTTABLENAME) + " VALUES ('" + footprintID + "', " + QString::number(deploymentId) + ");";
	bool success = query.exec(text);

	if(!success)
	{
		QString error = query.lastError().text();
		Log::writeError("dbFootprintTable: Cannot add Footprint: " + footprintID + " " + error);
		this->appendError("Cannot add Footprint: " + footprintID);
		return false;
	}
	else
	{
		return true;
	}
}


bool DBFootprintTable::deleteFootprint(QString footprintID)
{
	QSqlQuery query(this->getDB());
	QString text =  QString("DELETE FROM ") + FOOTPRINTTABLENAME
			+  QString(" WHERE DeploymentID = ") + QString::number(deploymentId) +
			QString(" AND FootprintID LIKE '") + footprintID +  QString("';");

	bool success = query.exec(text);

	if(!success)
	{
		QString error = query.lastError().text();
		Log::writeError("dbFootprintTable: Cannot delete Footprint: " + footprintID + " " + error);
		this->appendError("Cannot delete Footprint: " + footprintID);
		return false;
	}
	else
	{
		return true;
	}
}

bool DBFootprintTable::isFootprintExisting(QString footprintID)
{
	// Ask DB for this area
	QSqlQuery query(this->getDB());
	QString text = "SELECT FootprintID FROM " + QString(FOOTPRINTTABLENAME)
		+ " WHERE DeploymentID = " + QString::number(deploymentId) + " AND FootprintID LIKE '" + footprintID + "'";

	bool success = query.exec(text);

	if(!success)
	{
		QString error = query.lastError().text();
		Log::writeError("dbFootprintTable: Cannot detect if Footprint " + footprintID + " already entered: " + error);
		this->appendError("Cannot detect if Footprint existing: " + footprintID);
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

QStringList DBFootprintTable::getAllFootprints()
{
	QSqlQuery query (this->getDB());
	QStringList list;

	QString text = "SELECT FootprintID FROM " + QString(FOOTPRINTTABLENAME) + " WHERE DeploymentID = " +
	        QString::number(deploymentId) + ";";
	//qDebug(text.toStdString().c_str());
	bool success = query.exec(text);

	if(!success)
	{
		QString error = query.lastError().text();
		Log::writeError("dbFootprintTable: Cannot get all Footprint as a list: " + error);
		this->appendError("Cannot get Footprint list.");
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

bool DBFootprintTable::open()
{
    if(!this->openDatabase())
    {
        return false;
    }

    if(!this->isTableExisting(FOOTPRINTTABLENAME))
    {
        return this->createTable();
    }

    return true;
}
