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
			+ MEASUREMENTPROFILETABLENAME + QString(" (MeasurementProfileID INTEGER PRIMARY KEY, ButtonID INTEGER, SessionNr int,")
			+ QString(" ProgrammingTime char(20), CollectingTimeHost char(20), CollectingTimeButton char(20), SamplingRate int,")
			+ QString(" SamplingStartTime char(20), HighResolutionEn int, TempCalibUsed int, FOREIGN KEY(ButtonID) REFERENCES Buttons(ButtonID));"));
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

	profile.SessionNr = this->getLatestSessionNrByButtonId(profile.ButtonId) + 1;

	// Convert the ButtonData to SQL Format
	this->convertDataToSQL(&profile);

	// Create query
	QString sqlText("INSERT INTO ");
	sqlText = sqlText + MEASUREMENTPROFILETABLENAME + " ( ButtonID, SessionNr, ProgrammingTime, CollectingTimeHost, CollectingTimeButton, "
			+ "SamplingRate, SamplingStartTime, HighResolutionEn, TempCalibUsed) VALUES ( " + ButtonId + ", " + SessionNr + ", "
			+ ProgrammingTime + ", " + CollectingTimeHost + ", " + CollectingTimeButton + ", " + SamplingRate + ", " + SamplingStartTime
			+ ", " + HighResolutionEn + ", " + TempCalibUsed + " );";

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
	SamplingRate ="NULL";
	HighResolutionEn = "NULL";
	TempCalibUsed = "NULL";

	this->ButtonId = QString::number(profile->ButtonId);
	if(profile->SessionNr!=-9999) this->SessionNr = QString::number(profile->SessionNr);
	if(profile->HighResolutionEn!=-9999) this->HighResolutionEn = QString::number(profile->HighResolutionEn);
	if(profile->SamplingRate!=-9999) this->SamplingRate = QString::number(profile->SamplingRate);
	if(profile->MeasurementProfileID!=-9999) this->MeasurementProfileID = QString::number(profile->MeasurementProfileID);
	this->CollectingTimeHost = "'" + profile->CollectingTimeHost + "'";
	this->CollectingTimeButton = "'" + profile->CollectingTimeButton + "'";
	this->ProgrammingTime = "'" + profile->ProgrammingTime + "'";
	this->SamplingStartTime = "'" + profile->SamplingStartTime + "'";
	if(profile->TempCalibUsed!=9999) this->TempCalibUsed = QString::number(profile->TempCalibUsed);

}

bool DBMeasurementProfileTable::updateProfile(MeasurementProfile profile)
{
	convertDataToSQL(&profile);
	bool success = true;

	if(profile.MeasurementProfileID==-9999)
	{
		Log::writeError("dbMeasurementProfileTable: Cannot update Profile, because MeasurementProfileID is missing.");
		return false;
	}

	if(SessionNr!="NULL" && success)
	    success = update(MEASUREMENTPROFILETABLENAME, "MeasurementProfileID", MeasurementProfileID, "SessionNr", SessionNr);
	if(HighResolutionEn!="NULL" && success)
	    success = update(MEASUREMENTPROFILETABLENAME, "MeasurementProfileID", MeasurementProfileID, "HighResolutionEn", HighResolutionEn);
	if(SamplingRate!="NULL" && success)
	    success = update(MEASUREMENTPROFILETABLENAME, "MeasurementProfileID", MeasurementProfileID, "SamplingRate", SamplingRate);
	if(CollectingTimeHost!="''" && success)
	    success = update(MEASUREMENTPROFILETABLENAME, "MeasurementProfileID", MeasurementProfileID, "CollectingTimeHost", CollectingTimeHost);
	if(CollectingTimeButton!="''" && success)
	    success = update(MEASUREMENTPROFILETABLENAME, "MeasurementProfileID", MeasurementProfileID, "CollectingTimeButton", CollectingTimeButton);
	if(ProgrammingTime!="''" && success)
	    success = update(MEASUREMENTPROFILETABLENAME, "MeasurementProfileID", MeasurementProfileID, "ProgrammingTime", ProgrammingTime);
	if(SamplingStartTime!="''" && success)
	    success = update(MEASUREMENTPROFILETABLENAME, "MeasurementProfileID", MeasurementProfileID, "SamplingStartTime", SamplingStartTime);
	if(TempCalibUsed!="NULL" && success)
	    success = update(MEASUREMENTPROFILETABLENAME, "MeasurementProfileID", MeasurementProfileID, "TempCalibUsed", TempCalibUsed);

	if(!success)
	{
		Log::writeError("dbMeasurementProfile: Cannot update Measurement Profile, unspecified error.");
	}
	return success;
}

MeasurementProfile DBMeasurementProfileTable::readProfile(QString measurementProfileID)
{
	MeasurementProfile data;
	QString temp;
	measurementProfileID = this->toSQLString(measurementProfileID);

	// Get all the data and save them to MeasurementProfile
	temp = this->read(MEASUREMENTPROFILETABLENAME, "MeasurementProfileID", measurementProfileID, "MeasurementProfileID");
	if(temp!="") data.MeasurementProfileID = temp.toInt();

	temp = this->read(MEASUREMENTPROFILETABLENAME, "MeasurementProfileID", measurementProfileID, "ButtonID");
	if(temp!="") data.ButtonId = temp.toInt();

	temp = this->read(MEASUREMENTPROFILETABLENAME, "MeasurementProfileID", measurementProfileID, "SessionNr");
	if(temp!="") data.SessionNr = temp.toInt();

	temp = this->read(MEASUREMENTPROFILETABLENAME, "MeasurementProfileID", measurementProfileID, "HighResolutionEn");
	if(temp!="") data.HighResolutionEn = temp.toInt();

	temp = this->read(MEASUREMENTPROFILETABLENAME, "MeasurementProfileID", measurementProfileID, "SamplingRate");
	if(temp!="") data.SamplingRate = temp.toInt();

	temp = this->read(MEASUREMENTPROFILETABLENAME, "MeasurementProfileID", measurementProfileID, "CollectingTimeHost");
	if(temp!="") data.CollectingTimeHost = temp;

	temp = this->read(MEASUREMENTPROFILETABLENAME, "MeasurementProfileID", measurementProfileID, "CollectingTimeButton");
	if(temp!="") data.CollectingTimeButton = temp;

	temp = this->read(MEASUREMENTPROFILETABLENAME, "MeasurementProfileID", measurementProfileID, "ProgrammingTime");
	if(temp!="") data.ProgrammingTime = temp;

	temp = this->read(MEASUREMENTPROFILETABLENAME, "MeasurementProfileID", measurementProfileID, "SamplingStartTime");
	if(temp!="") data.SamplingStartTime = temp;

    temp = this->read(MEASUREMENTPROFILETABLENAME, "MeasurementProfileID", measurementProfileID, "TempCalibUsed");
    if(temp!="") data.TempCalibUsed = temp.toInt();

	return data;
}

int DBMeasurementProfileTable::getLatestAddedProfileIDByButtonId(int buttonId)
{
	QSqlQuery query(this->getDB());
	QString text = QString("SELECT MeasurementProfileID FROM ") + MEASUREMENTPROFILETABLENAME + QString(" WHERE ButtonID = ") + QString::number(buttonId) + QString(";");

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

int DBMeasurementProfileTable::getLatestSessionNrByButtonId(int buttonId)
{
	QSqlQuery query(this->getDB());
	QString text = QString("SELECT SessionNr FROM ") + MEASUREMENTPROFILETABLENAME + QString(" WHERE ButtonID = ") + QString::number(buttonId) + QString(";");

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

bool DBMeasurementProfileTable::deleteProfileByButtonId(int buttonId)
{
	bool success;

	QSqlQuery query(this->getDB());
	QString text = QString("DELETE FROM ") + MEASUREMENTPROFILETABLENAME
			+ QString(" WHERE ButtonID=") + QString::number(buttonId) + QString(";");
	success = query.exec(text);

	if(!success)
	{
		Log::writeError("dbMeasurementProfileTable: Cannot delete Profiles to: " + QString::number(buttonId)
				+ " / Error: " + query.lastError().text());
		this->appendError("Cannot delete Profiles to: " + QString::number(buttonId));
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
            + QString(MEASUREMENTPROFILETABLENAME) + " WHERE CollectingTimeHost != '' "
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

QVector<MeasurementProfile> DBMeasurementProfileTable::getProfilesByButtonID(int buttonId)
{
    QVector<MeasurementProfile> profileVector;
    QSqlQuery query (this->getDB());

    QString text = "SELECT MeasurementProfileID FROM "
            + QString(MEASUREMENTPROFILETABLENAME) + " WHERE ButtonID = "
            + QString::number(buttonId) + " ORDER BY MeasurementProfileID ASC;";
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

QString DBMeasurementProfileTable::read(QString table, QString compCol, QString compVal, QString getCol)
{
    QSqlQuery query(this->getDB());
    QString text = QString("SELECT " + getCol + " FROM " + table
            + " WHERE " + compCol + "=" + compVal +";");

    bool success = query.exec(text);

    if(!success)
    {
        Log::writeError("dbConnection: Cannot read database for value: " + getCol
                + " / Error: " + query.lastError().text());
        appendError("Cannot read database. Error is: " + query.lastError().text());
        return "";
    }
    else
    {
        if(query.next())
        {
            return query.value(0).toString();
        }
        else
        {
            return "";
        }
    }
}

