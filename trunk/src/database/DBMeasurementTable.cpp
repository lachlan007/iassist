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
#include "DBMeasurementTable.h"

DBMeasurementTable::DBMeasurementTable() {

}

DBMeasurementTable::~DBMeasurementTable() {

}

bool DBMeasurementTable::createTable()
{
	// check if a table exists
	if(this->isTableExisting(MEASUREMENTTABLENAME))
	{
		return true;
	}
	else
	{
	    QSqlQuery query(this->getDB());
	    bool success = query.exec(QString("CREATE TABLE ") +
	            MEASUREMENTTABLENAME + QString(" (MeasurementID INTEGER PRIMARY KEY,") +
	            QString(" MeasurementProfileID INTEGER, MeasurementNr INTEGER, Measurement REAL, FOREIGN KEY(MeasurementProfileID) ") +
	            QString(" REFERENCES MeasurementProfiles(MeasurementProfileID));"));
		if(success)
		{
			Log::write("Created new Measurement Table in Database");
			return true;
		}
		else
		{
			QString error = query.lastError().text();
			this->appendError("Cannot create Measurement table: " + error);
			Log::writeError("dbButtonTable: Cannot create table: " + error);
			return false;
		}
	}
}

bool DBMeasurementTable::deleteMeasurementsByMeasurementProfileID(int measurementProfileId)
{
	QSqlQuery query(this->getDB());
	QString text = QString("DELETE FROM ") + MEASUREMENTTABLENAME
			+ QString(" WHERE MeasurementProfileID=") + QString::number(measurementProfileId) + QString(";");
	bool success = query.exec(text);

	if(!success)
	{
		QString error = query.lastError().text();
		Log::writeError("dbMeasurementTable: Cannot delete Measurements of profile: " + QString::number(measurementProfileId)
				+ " / Error: " + error);
		this->appendError("Cannot delete Measurements of profile: " + QString::number(measurementProfileId));
		return false;
	}
	else
	{
		return true;
	}
}

bool DBMeasurementTable::addMeasurement(Measurement meas)
{
	QSqlDatabase db = this->getDB();
    QSqlQuery query(this->getDB());
	bool success = true;

	db.transaction();

	QString textBeg = "INSERT INTO " + QString(MEASUREMENTTABLENAME)
			+ " (MeasurementProfileID , MeasurementNr, Measurement) VALUES( "
			+ QString::number(meas.MeasurementProfileID) + ", ";

	// insert all measurements
	for(int i=0; i<meas.size; i++)
	{
		if(!query.exec(textBeg + QString::number(i+1) + QString(", ") + QString::number(meas.missionData[i]) + QString(" );")))
		{
			this->appendError("Cannot add Measurement of Button: " + meas.ButtonNr);
			this->appendError(query.lastError().text());
			success = false;
			break;
		}
	}

	db.commit();

	return success;
}

bool DBMeasurementTable::open()
{
    if(!this->openDatabase())
    {
        return false;
    }

    if(!this->isTableExisting(MEASUREMENTTABLENAME))
    {
        return this->createTable();
    }

    return true;
}

int DBMeasurementTable::getMeasurementsByProfileID(QString measurementProfileID, double **values) {
    QSqlQuery query(this->getDB());

    // Get size of result set - query.size() not supported with SQLite
    QString text = "SELECT COUNT(*) FROM " + QString(MEASUREMENTTABLENAME)
                + " WHERE MeasurementProfileID='" + measurementProfileID + "';";
    if(!query.exec(text))
    {
        Log::writeError("dbMeasurementTable: Cannot get number of measured values: " + measurementProfileID
                    + " / Error: " + query.lastError().text());
        this->appendError("Cannot get number of measured values: " + measurementProfileID);
        return -1;
    }

    query.next();
    int resultSize = query.value(0).toInt();
    *values = new double[resultSize];

    text = "SELECT Measurement FROM " + QString(MEASUREMENTTABLENAME)
            + " WHERE MeasurementProfileID='" + measurementProfileID + "' ORDER BY MeasurementNr ASC;";
    if(!query.exec(text))
    {
        Log::writeError("dbMeasurementTable: Cannot get measured values: " + measurementProfileID
                + " / Error: " + query.lastError().text());
        this->appendError("Cannot get measured values: " + measurementProfileID);
        return -1;
    }
    else
    {
        int i=0;
        while(query.next())
        {
            (*values)[i] = query.value(0).toDouble();
            i++;
        }
    }

    return resultSize;
}

//bool DBMeasurementTable::getMe
