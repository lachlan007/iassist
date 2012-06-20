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
#include "DBMeasurementProfileTable.h"

DBMeasurementProfileTable::DBMeasurementProfileTable() {


}

DBMeasurementProfileTable::~DBMeasurementProfileTable() {

}

bool DBMeasurementProfileTable::createTable()
{
	// check if a table exists
	if(this->isTableExisting(MEASUREMENTPROFILETABLENAME))
	{
		return true;
	}
	else
	{
		QSqlQuery query(this->getDB());
		bool success = query.exec(QString("CREATE TABLE ")
			+ MEASUREMENTPROFILETABLENAME + QString(" (MeasurementProfileID INTEGER PRIMARY KEY, ButtonNr char(10), SessionNr int,")
			+ QString(" DistributingTime char(30), CollectingTime char(30), TimeShift int, SamplingRate int,")
			+ QString(" SamplingStartTimeStamp char(30), Resolution int);"));
		if(success)
		{
			Log::write("Created new MeasurementProfiles Table in Database");
			return true;
		}
		else
		{
			QString error = query.lastError().text();
			this->appendError("Cannot create table: " + error);
			Log::writeError("dbMeasurementProfileTable: Cannot create table: " + error);
			return false;
		}
	}
}

bool DBMeasurementProfileTable::addProfile(MeasurementProfile profile)
{
	// Create the Query to add a complete Profile Measurement record to the database
	QSqlQuery query(this->getDB());

	// TODO check if profile not already entered
	//if(this->isButtonExistingByButtonNr(button.ButtonNr))
	//{
	//	Log::writeError("DBButtonTable: Cannot add a button twice.");
	//	this->appendError("Cannot add a button twice. " + button.ButtonNr + " already in DB");
	//	return false;
	//}

	profile.SessionNr = this->getLatestSessionNrByButtonNr(profile.ButtonNr) + 1;

	// Convert the ButtonData to SQL Format
	this->convertDataToSQL(&profile);

	// Create query
	QString sqlText("INSERT INTO ");
	sqlText = sqlText + MEASUREMENTPROFILETABLENAME + " ( ButtonNr, SessionNr, DistributingTime, CollectingTime, "
			+ "TimeShift, SamplingRate, SamplingStartTimeStamp, Resolution) VALUES ( " + ButtonNr + ", " + SessionNr + ", "
			+ DistributingTime + ", " + CollectingTime + ", " + TimeShift + ", " + SamplingRate + ", " + SamplingStartTimeStamp
			+ ", " + Resolution + " );";

	// Send the query
	bool success = query.exec(sqlText);

	if(!success)
	{
		QString error = query.lastError().text();
		Log::writeError("dbMeasurementProfileTable: Cannot add MeasurementProfile to Database: " + error);
		this->appendError("Cannot add MeasurementProfile to Database: " + error);
	}

	query.clear();
	return success;
}

void DBMeasurementProfileTable::convertDataToSQL(MeasurementProfile *profile)
{
	// Give all the digits "NULL" as value.
	SessionNr = "NULL";
	MeasurementProfileID = "NULL";
	TimeShift ="NULL";
	SamplingRate ="NULL";
	Resolution = "NULL";

	ButtonNr ="'" + profile->ButtonNr + "'";
	if(profile->SessionNr!=-9999) this->SessionNr= QString::number(profile->SessionNr);
	if(profile->Resolution!=-9999) this->Resolution= QString::number(profile->Resolution);
	if(profile->SamplingRate!=-9999) this->SamplingRate = QString::number(profile->SamplingRate);
	if(profile->TimeShift!=-9999) this->TimeShift = QString::number(profile->TimeShift);
	if(profile->MeasurementProfileID!=-9999) this->MeasurementProfileID = QString::number(profile->MeasurementProfileID);
	this->CollectingTime = "'" + profile->CollectingTime + "'";
	this->DistributingTime = "'" + profile->DistributingTime + "'";
	this->SamplingStartTimeStamp = "'" + profile->SamplingStartTimeStamp + "'";

}

bool DBMeasurementProfileTable::updateProfile(MeasurementProfile profile)
{
	convertDataToSQL(&profile);
	bool success = true;

	if(profile.MeasurementProfileID==-9999)
	{
		Log::writeError("dbMeasurementProfileTable: Cannot update Profile, 'cause MeasurementProfileID is missing.");
		return false;
	}

	if(ButtonNr!="''" && success)
		success = update(MEASUREMENTPROFILETABLENAME, "MeasurementProfileID", MeasurementProfileID, "ButtonNr", ButtonNr);
	if(SessionNr!="NULL" && success)
		success = update(MEASUREMENTPROFILETABLENAME, "MeasurementProfileID", MeasurementProfileID, "SessionNr", SessionNr);
	if(Resolution!="NULL" && success)
		success = update(MEASUREMENTPROFILETABLENAME, "MeasurementProfileID", MeasurementProfileID, "Resolution", Resolution);
	if(SamplingRate!="NULL" && success)
		success = update(MEASUREMENTPROFILETABLENAME, "MeasurementProfileID", MeasurementProfileID, "SamplingRate", SamplingRate);
	if(TimeShift!="NULL" && success)
		success = update(MEASUREMENTPROFILETABLENAME, "MeasurementProfileID", MeasurementProfileID, "TimeShift", TimeShift);
	if(CollectingTime!="''" && success)
		success = update(MEASUREMENTPROFILETABLENAME, "MeasurementProfileID", MeasurementProfileID, "CollectingTime", CollectingTime);
	if(DistributingTime!="''" && success)
		success = update(MEASUREMENTPROFILETABLENAME, "MeasurementProfileID", MeasurementProfileID, "DistributingTime", DistributingTime);
	if(SamplingStartTimeStamp!="''" && success)
		success = update(MEASUREMENTPROFILETABLENAME, "MeasurementProfileID", MeasurementProfileID, "SamplingStartTimeStamp", SamplingStartTimeStamp);

	if(!success)
	{
		Log::writeError("dbMeasurementProfile: Cannot update Measurement Profile, unspecified error.");
	}
	return success;
}

MeasurementProfile DBMeasurementProfileTable::readProfile(QString _measurementProfileID)
{
	MeasurementProfile data;
	QString temp;
	QString measurementID = this->toSQLString(_measurementProfileID);

	// Get all the data and save them to MeasurementProfile
	temp = this->read(MEASUREMENTPROFILETABLENAME, "MeasurementProfileID", measurementID, "MeasurementProfileID");
	if(temp!="") data.MeasurementProfileID = temp.toInt();

	temp = this->read(MEASUREMENTPROFILETABLENAME, "MeasurementProfileID", measurementID, "ButtonNr");
	if(temp!="") data.ButtonNr = temp;

	temp = this->read(MEASUREMENTPROFILETABLENAME, "MeasurementProfileID", measurementID, "SessionNr");
	if(temp!="") data.SessionNr = temp.toInt();

	temp = this->read(MEASUREMENTPROFILETABLENAME, "MeasurementProfileID", measurementID, "Resolution");
	if(temp!="") data.Resolution = temp.toInt();

	temp = this->read(MEASUREMENTPROFILETABLENAME, "MeasurementProfileID", measurementID, "SamplingRate");
	if(temp!="") data.SamplingRate = temp.toInt();

	temp = this->read(MEASUREMENTPROFILETABLENAME, "MeasurementProfileID", measurementID, "TimeShift");
	if(temp!="") data.TimeShift = temp.toInt();

	temp = this->read(MEASUREMENTPROFILETABLENAME, "MeasurementProfileID", measurementID, "CollectingTime");
	if(temp!="") data.CollectingTime = temp;

	temp = this->read(MEASUREMENTPROFILETABLENAME, "MeasurementProfileID", measurementID, "DistributingTime");
	if(temp!="") data.DistributingTime = temp;

	temp = this->read(MEASUREMENTPROFILETABLENAME, "MeasurementProfileID", measurementID, "SamplingStartTimeStamp");
	if(temp!="") data.SamplingStartTimeStamp = temp;

	return data;
}

int DBMeasurementProfileTable::getLatestAddedProfileIDByButtonNr(QString _buttonNr)
{
	QSqlQuery query(this->getDB());
	QString text = QString("SELECT MeasurementProfileID FROM ") + MEASUREMENTPROFILETABLENAME + QString(" WHERE ButtonNr LIKE '") + _buttonNr + QString("';");

	int nr = 0;
	int temp = 0;
	if(!query.exec(text))
	{
		Log::writeError("DBMeasurementProfileTable: Cannot get latest added ProfileID: " + query.lastError().text());
		return 0;
	}

	while(query.next())
	{
		temp = query.value(0).toInt();
		if(temp > nr)
		{
			nr = temp;
		}
	}
	return nr;
}

int DBMeasurementProfileTable::getLatestSessionNrByButtonNr(QString _buttonNr)
{
	QSqlQuery query(this->getDB());
	QString text = QString("SELECT SessionNr FROM ") + MEASUREMENTPROFILETABLENAME + QString(" WHERE ButtonNr LIKE '") + _buttonNr + QString("';");

	int nr = 0;
	int temp = 0;

	if(!query.exec(text))
	{
		Log::writeError("DBMeasurementProfileTable: Cannot get latest added SessionNr: " + query.lastError().text());
		return 0;
	}

	while(query.next())
	{
		temp = query.value(0).toInt();
		if(temp > nr)
		{
			nr = temp;
		}
	}
	return nr;
}

bool DBMeasurementProfileTable::open()
{
    if(!this->openDatabase())
    {
        return false;
    }

    if(!this->isTableExisting(MEASUREMENTPROFILETABLENAME))
    {
        return this->createTable();
    }

    return true;
}

bool DBMeasurementProfileTable::deleteProfileByArea(QString _area)
{
	bool success;

	QSqlQuery query(this->getDB());
	QString text = QString("DELETE FROM ") + MEASUREMENTPROFILETABLENAME
			+ QString(" WHERE ButtonNr LIKE '") + _area + QString("%';");
	success = query.exec(text);

	if(!success)
	{
		Log::writeError("dbMeasurementProfileTable: Cannot delete Profiles of Area: " + _area);
		Log::writeError("Error: " + query.lastError().text());
		this->appendError("Cannot delete Profiles of Area: " + _area);
		return false;
	}
	else
	{
		return true;
	}
}

bool DBMeasurementProfileTable::deleteProfileByButtonNr(QString _buttonNr)
{
	bool success;

	QSqlQuery query(this->getDB());
	QString text = QString("DELETE FROM ") + MEASUREMENTPROFILETABLENAME
			+ QString(" WHERE ButtonNr='") + _buttonNr + QString("';");
	success = query.exec(text);

	if(!success)
	{
		Log::writeError("dbMeasurementProfileTable: Cannot delete Profiles to: " + _buttonNr
				+ " / Error: " + query.lastError().text());
		this->appendError("Cannot delete Profiles to: " + _buttonNr);
		return false;
	}
	else
	{
		return true;
	}
}

QVector<MeasurementProfile> DBMeasurementProfileTable::getFinishedMeasurementProfiles()
{
    QVector<MeasurementProfile> profileVector;
    QSqlQuery query (this->getDB());

    QString text = "SELECT MeasurementProfileID FROM "
            + QString(MEASUREMENTPROFILETABLENAME) + " WHERE CollectingTime != '' "
            + "ORDER BY MeasurementProfileID ASC;";
    //qDebug(text.toStdString().c_str());
    bool success = query.exec(text);

    if(!success)
    {
        QString error = query.lastError().text();
        Log::writeError("dbMeasurementProfileTable: Cannot get all profiles as a list: " + error);
        this->appendError("Cannot get measurement profile list.");
    }
    else
    {
        while(query.next())
        {
            profileVector.append(readProfile(query.value(0).toString()));
        }
    }
    return profileVector;
}
