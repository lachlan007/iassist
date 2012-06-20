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
				+ AREATABLENAME + QString(" (FootprintID char(10), Altitude int, Longitude int, Latitude int,")
				+ QString(" HAE_DEM int, East_DEM int, North_DEM int, Slope_DEM int, Aspect_DEM int,")
				+ QString(" SolDy_DEM int, SolYr_DEM int, StartLongitude int, EndLongitude1 int,")
				+ QString(" EndLongitude2 int, StartLatitude int, EndLatitude1 int, EndLatitude2 int);"));
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

	QString text = "INSERT INTO " + QString(AREATABLENAME) + " VALUES ( " + AreaID + ", " + AAltitude + ", " + ALongitude
		+ ", " + ALatitude + ", " + AHAE_DEM + ", " + AEast_DEM + ", " + ANorth_DEM + ", " + ASlope_DEM
		+ ", " + AAspect_DEM + ", " + ASolDy_DEM	+ ", " + ASolYr_DEM + ", " + AStartLongitude
		+ ", " + AEndLongitude1 + ", " + AEndLongitude2 + ", " + AStartLatitude
		+ ", " + AEndLatitude1 + ", " + AEndLatitude2 + " );";
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
	AAltitude = "NULL";
	ALongitude = "NULL";
	ALatitude = "NULL";
	AHAE_DEM = "NULL";
	AEast_DEM = "NULL";
	ANorth_DEM = "NULL";
	ASlope_DEM = "NULL";
	AAspect_DEM = "NULL";
	ASolDy_DEM = "NULL";
	ASolYr_DEM = "NULL";
	AStartLongitude = "NULL";
	AEndLongitude1 = "NULL";
	AEndLongitude2 = "NULL";
	AStartLatitude = "NULL";
	AEndLatitude1 = "NULL";
	AEndLatitude2 = "NULL";

	AreaID="'" + area->AreaID + "'";
	if(area->Altitude!=-9999) AAltitude=QString::number(area->Altitude, 'f', 1);
	if(area->Longitude!=-9999) ALongitude=QString::number(area->Longitude, 'f', 10);
	if(area->Latitude!=-9999)ALatitude=QString::number(area->Latitude, 'f', 10);
	if(area->HAE_DEM!=-9999)AHAE_DEM=QString::number(area->HAE_DEM, 'f', 1);
	if(area->East_DEM!=-9999)AEast_DEM=QString::number(area->East_DEM, 'f', 4);
	if(area->North_DEM!=-9999)ANorth_DEM=QString::number(area->North_DEM, 'f', 4);
	if(area->Slope_DEM!=-9999)ASlope_DEM=QString::number(area->Slope_DEM, 'f', 8);
	if(area->Aspect_DEM!=-9999)AAspect_DEM=QString::number(area->Aspect_DEM, 'f', 8);
	if(area->SolDy_DEM!=-9999)ASolDy_DEM=QString::number(area->SolDy_DEM, 'f', 8);
	if(area->SolYr_DEM!=-9999)ASolYr_DEM=QString::number(area->SolYr_DEM, 'f', 8);
	if(area->StartLongitude!=-9999)AStartLongitude=QString::number(area->StartLongitude, 'f', 10);
	if(area->EndLongitude1!=-9999)AEndLongitude1=QString::number(area->EndLongitude1, 'f', 10);
	if(area->EndLongitude2!=-9999)AEndLongitude2=QString::number(area->EndLongitude2, 'f', 10);
	if(area->StartLatitude!=-9999)AStartLatitude=QString::number(area->StartLatitude, 'f', 10);
	if(area->EndLatitude1!=-9999)AEndLatitude1=QString::number(area->EndLatitude1, 'f', 10);
	if(area->EndLatitude2!=-9999)AEndLatitude2=QString::number(area->EndLatitude2, 'f', 10);
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

bool DBAreaTable::updateArea(Area areaData)
{
	convertAreaDataToSQL(&areaData);
	bool success = true;
	// All the values that where entered to area will replace the old one in the DB
	if(AAltitude!="NULL") success = update(AREATABLENAME, "FootprintID", AreaID, "Altitude", AAltitude);
	if(ALongitude!="NULL" && success)
		success = update(AREATABLENAME, "FootprintID", AreaID, "Longitude", ALongitude);
	if(ALatitude!="NULL" && success)
		success = update(AREATABLENAME, "FootprintID", AreaID, "Latitude", ALatitude);
	if(AHAE_DEM!="NULL" && success)
		success = update(AREATABLENAME, "FootprintID", AreaID, "HAE_DEM", AHAE_DEM);
	if(AEast_DEM!="NULL" && success)
		success = update(AREATABLENAME, "FootprintID", AreaID, "East_DEM", AEast_DEM);
	if(ANorth_DEM!="NULL" && success)
		success = update(AREATABLENAME, "FootprintID", AreaID, "North_DEM", ANorth_DEM);
	if(ASlope_DEM!="NULL" && success)
		success = update(AREATABLENAME, "FootprintID", AreaID, "Slope_DEM", ASlope_DEM);
	if(AAspect_DEM!="NULL" && success)
		success = update(AREATABLENAME, "FootprintID", AreaID, "Aspect_DEM", AAspect_DEM);
	if(ASolDy_DEM!="NULL" && success)
		success = update(AREATABLENAME, "FootprintID", AreaID, "SolDy_DEM", ASolDy_DEM);
	if(ASolYr_DEM!="NULL" && success)
		success = update(AREATABLENAME, "FootprintID", AreaID, "SolYr_DEM", ASolYr_DEM);
	if(AStartLongitude!="NULL" && success)
		success = update(AREATABLENAME, "FootprintID", AreaID, "StartLongitude", AStartLongitude);
	if(AEndLongitude1!="NULL" && success)
		success = update(AREATABLENAME, "FootprintID", AreaID, "EndLongitude1", AEndLongitude1);
	if(AEndLongitude2!="NULL" && success)
		success = update(AREATABLENAME, "FootprintID", AreaID, "EndLongitude2", AEndLongitude2);
	if(AStartLatitude!="NULL" && success)
		success = update(AREATABLENAME, "FootprintID", AreaID, "StartLatitude", AStartLatitude);
	if(AEndLatitude1!="NULL" && success)
		success = update(AREATABLENAME, "FootprintID", AreaID, "EndLatitude1", AEndLatitude1);
	if(AEndLatitude2!="NULL" && success)
		success = update(AREATABLENAME, "FootprintID", AreaID, "EndLatitude2", AEndLatitude2);

	return success;
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

	temp = this->read(AREATABLENAME, "FootprintID", sqlFootprintID, "Altitude");
	if(temp!="") data.Altitude = temp.toDouble();

	temp = this->read(AREATABLENAME, "FootprintID", sqlFootprintID, "Aspect_DEM");
	if(temp!="") data.Aspect_DEM = temp.toDouble();

	temp = this->read(AREATABLENAME, "FootprintID", sqlFootprintID, "East_DEM");
	if(temp!="") data.East_DEM = temp.toDouble();

	temp = this->read(AREATABLENAME, "FootprintID", sqlFootprintID, "EndLatitude1");
	if(temp!="") data.EndLatitude1 = temp.toDouble();

	temp = this->read(AREATABLENAME, "FootprintID", sqlFootprintID, "EndLatitude2");
	if(temp!="") data.EndLatitude2 = temp.toDouble();

	temp = this->read(AREATABLENAME, "FootprintID", sqlFootprintID, "EndLongitude1");
	if(temp!="") data.EndLongitude1 = temp.toDouble();

	temp = this->read(AREATABLENAME, "FootprintID", sqlFootprintID, "EndLongitude2");
	if(temp!="") data.EndLongitude2 = temp.toDouble();

	temp = this->read(AREATABLENAME, "FootprintID", sqlFootprintID, "HAE_DEM");
	if(temp!="") data.HAE_DEM = temp.toDouble();

	temp = this->read(AREATABLENAME, "FootprintID", sqlFootprintID, "Latitude");
	if(temp!="") data.Latitude = temp.toDouble();

	temp = this->read(AREATABLENAME, "FootprintID", sqlFootprintID, "Longitude");
	if(temp!="") data.Longitude = temp.toDouble();

	temp = this->read(AREATABLENAME, "FootprintID", sqlFootprintID, "North_DEM");
	if(temp!="") data.North_DEM = temp.toDouble();

	temp = this->read(AREATABLENAME, "FootprintID", sqlFootprintID, "Slope_DEM");
	if(temp!="") data.Slope_DEM = temp.toDouble();

	temp = this->read(AREATABLENAME, "FootprintID", sqlFootprintID, "SolDy_DEM");
	if(temp!="") data.SolDy_DEM = temp.toDouble();

	temp = this->read(AREATABLENAME, "FootprintID", sqlFootprintID, "SolYr_DEM");
	if(temp!="") data.SolYr_DEM = temp.toDouble();

	temp = this->read(AREATABLENAME, "FootprintID", sqlFootprintID, "StartLatitude");
	if(temp!="") data.StartLatitude = temp.toDouble();

	temp = this->read(AREATABLENAME, "FootprintID", sqlFootprintID, "StartLongitude");
	if(temp!="") data.StartLongitude = temp.toDouble();

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
