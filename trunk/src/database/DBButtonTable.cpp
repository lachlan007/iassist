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

DBButtonTable::DBButtonTable(int deploymentId) : DBConnection() {
    this->deploymentId = deploymentId;
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
		bool success = query.exec(QString("CREATE TABLE ") +
		        BUTTONTABLENAME + QString(" (ButtonID INTEGER PRIMARY KEY, DeploymentID INTEGER, ") +
		        QString("ButtonNr char(10), SerialNr char(25), FOREIGN KEY ") +
		        QString("(DeploymentID) REFERENCES Deployments(DeploymentID));"));
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

bool DBButtonTable::addButton(ButtonData button, int &insertId)
{
	// Create the Query to add a complete iButton Database entry to the database
	QSqlQuery query(this->getDB());

	// Check if Button is already entered
	if(this->isButtonExistingBySerialNr(button.SerialNr))
	{
		Log::writeError("dbButtonTable: Cannot add a button twice.");
		this->appendError("Cannot add a button twice. " + button.SerialNr + " already in DB");
		return false;
	}

	// Convert the ButtonData to SQL Format
	convertButtonDataToSQL(&button);

	QString sqlText("INSERT INTO ");
	sqlText = sqlText + BUTTONTABLENAME + " (DeploymentID, ButtonNr, SerialNr) VALUES ( " + QString::number(deploymentId) +
	        ", " +  ButtonNr + ", " + SerialNr + ");";

	// Send the query
	bool success = query.exec(sqlText);

	if(!success)
	{
		QString error = query.lastError().text();
		Log::writeError("dbButtonTable: Cannot add Button to Database: " + error);
		this->appendError("Cannot add Button to Database: " + error);
	}

	insertId = query.lastInsertId().toInt();

	query.clear();
	return success;
}

void DBButtonTable::convertButtonDataToSQL(ButtonData *button)
{
	// Give all the digits "NULL" as value.
	ButtonNr=QString("'") + button->ButtonNr + QString("'");
	SerialNr=QString("'") + button->SerialNr + QString("'");

}

ButtonData DBButtonTable::getButtonByButtonNr(QString buttonNr)
{
	ButtonData data;
	QString temp;

	// Get all the data and save them to ButtonData

	temp = this->read(BUTTONTABLENAME, "ButtonNr", this->toSQLString(buttonNr), "ButtonNr");
	if(temp!="") data.ButtonNr = temp;

	temp = this->read(BUTTONTABLENAME, "ButtonNr", this->toSQLString(buttonNr), "SerialNr");
	if(temp!="") data.SerialNr = temp;

	return data;
}

ButtonData DBButtonTable::getButtonBySerialNr(QString serialNr)
{
    ButtonData data;
    QString temp;

    // Get all the data and save them to ButtonData

    temp = this->read(BUTTONTABLENAME, "SerialNr", this->toSQLString(serialNr), "ButtonNr");
    if(temp!="") data.ButtonNr = temp;

    temp = this->read(BUTTONTABLENAME, "SerialNr", this->toSQLString(serialNr), "SerialNr");
    if(temp!="") data.SerialNr = temp;

    return data;
}

ButtonData DBButtonTable::getButtonByButtonId(int buttonId)
{
    ButtonData data;
    QString temp;

    // Get all the data and save them to ButtonData

    temp = this->read(BUTTONTABLENAME, "ButtonID", QString::number(buttonId), "ButtonNr");
    if(temp!="") data.ButtonNr = temp;

    temp = this->read(BUTTONTABLENAME, "ButtonID", QString::number(buttonId), "SerialNr");
    if(temp!="") data.SerialNr = temp;

    return data;
}

int DBButtonTable::getButtonIdBySerialNr(QString serialNr)
{
    QString temp;
    int buttonId;

    // Get all the data and save them to ButtonData
    temp = this->read(BUTTONTABLENAME, "SerialNr", this->toSQLString(serialNr), "ButtonID");
    if(temp!="") buttonId = temp.toInt();

    return buttonId;
}

int DBButtonTable::getButtonIdByButtonNr(QString buttonNr)
{
    QString temp;
    int buttonId;

    // Get all the data and save them to ButtonData
    temp = this->read(BUTTONTABLENAME, "ButtonNr", this->toSQLString(buttonNr), "ButtonID");
    if(temp!="") buttonId = temp.toInt();

    return buttonId;
}

QStringList DBButtonTable::getAllButtonNr()
{
	QStringList list;
	QSqlQuery query(this->getDB());

	QString text = QString("SELECT ButtonNr FROM ") + QString(BUTTONTABLENAME) +
	        QString(" WHERE DeploymentID = ") + QString::number(deploymentId) + QString(";");
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

	QString text = QString("SELECT ButtonNr FROM ") + QString(BUTTONTABLENAME) + " WHERE DeploymentID = " +
	        QString::number(deploymentId) + " AND ButtonNr LIKE '" + _area + "%';";
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
	QString text = QString("SELECT ButtonNr FROM ") + BUTTONTABLENAME + QString(" WHERE DeploymentID = " +
            QString::number(deploymentId) + " AND ButtonNr LIKE '") + _area + QString("%';");

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

bool DBButtonTable::deleteButtonByButtonId(int buttonId)
{
	bool success;

	QSqlQuery query(this->getDB());
	QString text = QString("DELETE FROM ") + BUTTONTABLENAME
			+ QString(" WHERE ButtonID = ") + QString::number(buttonId) + QString(";");
	success = query.exec(text);

	if(!success)
	{
		Log::writeError("dbButtonTable: Cannot delete Button: " + QString::number(buttonId)
				+ " / Error indicated: " + query.lastError().text());
		this->appendError("Cannot delete Button: " + QString::number(buttonId));
		return false;
	}
	else
	{
		return true;
	}
}

QVector<int> DBButtonTable::getButtonIdsByArea(QString area)
{
	bool success;
	QVector<int> result;


	QSqlQuery query(this->getDB());
	QString text =  QString("SELECT ButtonID FROM ") + QString(BUTTONTABLENAME)
			+  QString(" WHERE DeploymentID = ") + QString::number(deploymentId) +  QString(" AND ButtonNr LIKE '") + area + QString("%';");
	success = query.exec(text);

	if(!success)
	{
		Log::writeError("dbButtonTable: Cannot retrieve buttons of Area: " + area
				+ " / Error indicated: " + query.lastError().text());
		this->appendError("Cannot retrieve Buttons of Area: " + area);
	}
	else
	{
	    while(query.next())
	    {
	        result.append(query.value(0).toInt());
	    }
	}
	return result;
}

bool DBButtonTable::isButtonExistingBySerialNr(QString serialNr)
{
	// Ask DB for this ButtonNr
	QSqlQuery query(this->getDB());
	QString text = QString("SELECT ButtonNr FROM ") + QString(BUTTONTABLENAME)
		+ " WHERE DeploymentID = " + QString::number(deploymentId) + " AND SerialNr LIKE '" + QString(serialNr + "'");
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

QString DBButtonTable::read(QString table, QString compCol, QString compVal, QString getCol)
{
    QSqlQuery query(this->getDB());
    QString text = QString("SELECT " + getCol + " FROM " + table
            + " WHERE DeploymentID = " + QString::number(deploymentId) + " AND " + compCol + "=" + compVal +";");

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

