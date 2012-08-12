//---------------------------------------------------------------------------
//  iAssist - Rapid deployment of Maxim iButton(c) miniature logger devices
//  Copyright (C) 2012 ETH Zurich, Computer Engineering and Networks
//      Laboratory, Matthias Keller <kellmatt at tik.ee.ethz.ch>
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
#include "DBDeploymentTable.h"

DBDeploymentTable::DBDeploymentTable() {

}

DBDeploymentTable::~DBDeploymentTable() {

}

bool DBDeploymentTable::createTable()
{
	// check if a table exists
	if(this->isTableExisting(DEPLOYMENTTABLENAME))
	{
		return true;
	}
	else
	{
		QSqlQuery query(this->getDB());
		bool success = query.exec(QString("CREATE TABLE ")
				+ DEPLOYMENTTABLENAME + QString(" (DeploymentID INTEGER PRIMARY KEY, Name char(100));"));
		if(success)
		{
			Log::write("Created new Deployment Table in Database");
			return true;
		}
		else
		{
			QString error = query.lastError().text();
			this->appendError("Cannot create Deployment table: " + error);
			Log::writeError("dbDeploymentTable: Cannot create table: " + error);
			return false;
		}
	}
}

bool DBDeploymentTable::addDeployment(QString deploymentName, int &insertId)
{
	QSqlQuery query(this->getDB());

	QString text = "INSERT INTO " + QString(DEPLOYMENTTABLENAME) + " (Name) VALUES ( '" + deploymentName + "');";
	bool success = query.exec(text);

	if(!success)
	{
		QString error = query.lastError().text();
		Log::writeError("dbAreaTable: Cannot add deployment: " + deploymentName + " " + error);
		this->appendError("Cannot add Area: " + deploymentName);
		return false;
	}
	else
	{
	    insertId = query.lastInsertId().toInt();
	 	return true;
	}
}


bool DBDeploymentTable::deleteDeployment(int deploymentId)
{
	QSqlQuery query(this->getDB());
	QString text = QString("DELETE FROM ") + DEPLOYMENTTABLENAME
			+ QString(" WHERE DeploymentID = ") + QString::number(deploymentId) + QString(";");

	bool success = query.exec(text);

	if(!success)
	{
		QString error = query.lastError().text();
		Log::writeError("dbDeploymentTable: Cannot delete deployment: " + QString::number(deploymentId) + " " + error);
		this->appendError("Cannot delete deployment: " + QString::number(deploymentId));
		return false;
	}
	else
	{
		return true;
	}
}

QVector<QPair<int,QString> > DBDeploymentTable::getAllDeployments()
{
	QSqlQuery query (this->getDB());
	QVector<QPair<int,QString> > result;

	QString text = "SELECT DeploymentID, Name FROM " + QString(DEPLOYMENTTABLENAME) + ";";
	//qDebug(text.toStdString().c_str());
	bool success = query.exec(text);

	if(!success)
	{
		QString error = query.lastError().text();
		Log::writeError("dbDeploymentTable: Cannot get all deployments as a list: " + error);
		this->appendError("Cannot get deployment list.");
	}
	else
	{
		while(query.next())
		{
		    QPair<int,QString> p(query.value(0).toInt(), query.value(1).toString());
			result.append(p);
		}
	}
	return result;
}

QString DBDeploymentTable::getDeploymentName(int deploymentId)
{
    QSqlQuery query (this->getDB());

    QString text = "SELECT Name FROM " + QString(DEPLOYMENTTABLENAME) + " WHERE DeploymentID = " + QString::number(deploymentId) + ";";
    //qDebug(text.toStdString().c_str());
    bool success = query.exec(text);

    if(!success)
    {
        QString error = query.lastError().text();
        Log::writeError("dbDeploymentTable: Cannot find name: " + error);
        this->appendError("Cannot retrieve name.");
        return "";
    }
    if(query.next())
    {
        return query.value(0).toString();
    }
    else
    {
        return "";
    }
}

bool DBDeploymentTable::open()
{
    if(!this->openDatabase())
    {
        return false;
    }

    if(!this->isTableExisting(DEPLOYMENTTABLENAME))
    {
        return this->createTable();
    }

    return true;
}
