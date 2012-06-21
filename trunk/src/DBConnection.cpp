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
#include "DBConnection.h"

static int connectionCount = 0;
static QString connectionName = CONNECTIONNAME;
static QSqlDatabase iButtonDB;

DBConnection::DBConnection() {

}

DBConnection::DBConnection(QString _connectionName)
{
	if(connectionName != CONNECTIONNAME)
	{
		connectionName = _connectionName;
	}

}

DBConnection::~DBConnection() {

}

bool DBConnection::isDatabaseExisting(QString dbName)
{
	QFile file(dbName);
	bool fileExists = file.exists(dbName);
	return fileExists;
}

bool DBConnection::createDatabase(QString dbName)
{
	if(this->isDatabaseExisting(dbName))
	{
		return false;
	}
	else
	{
		if(QSqlDatabase::contains(connectionName))
		{
			iButtonDB = QSqlDatabase::database(connectionName);
		}
		else
		{
			iButtonDB = QSqlDatabase::addDatabase("QSQLITE", connectionName);
		}
		iButtonDB.setDatabaseName(dbName);
		iButtonDB.open();
		iButtonDB.close();
		Log::write("Created new Database: " + dbName);
		return true;
	}
}

bool DBConnection::openDatabase(QString dbName)
{
    // Check if Database is already existing if not create it first
    if(!isDatabaseExisting(dbName))
    {
        createDatabase(dbName);
    }

    // if QT already manages the right database we have to close first
    if(QSqlDatabase::contains(connectionName))
    {
        iButtonDB = QSqlDatabase::database(connectionName);
        connectionCount++;
        //Log::write("Database already existing.");
    }
    // else we have to add this database to QT
    else
    {
        iButtonDB = QSqlDatabase::addDatabase("QSQLITE", connectionName);
        iButtonDB.setDatabaseName(dbName);
        if(iButtonDB.open())
        {
            //Log::write("Database opened.");
            connectionCount = 1;
        }
        else
        {
            Log::writeError("dbConnection: Cannot open the Database.");
            this->appendError("Cannot open the Database.");
            return false;
        }
        //Log::write("Database already existing.");
    }
    return true;

}

bool DBConnection::close()
{
	if(connectionCount>1 && iButtonDB.isOpen())
	{
		connectionCount--;
		//Log::write("Database is still open, but only: " + QString::number(connectionCount));
		return true;
	}
	else if(connectionCount==1 && iButtonDB.isOpen())
	{
		connectionCount=0;
		iButtonDB.close();
		//Log::write("Database is closed.");
		return true;
	}
	else if(connectionCount<1 && iButtonDB.isOpen())
	{
		connectionCount=0;
		iButtonDB.close();
		Log::writeError("Database was illegally open.");
		this->appendError("Database was NOT closed.");
		return false;
	}
	else
	{
		Log::writeError("Database was already closed.");
		this->appendError("Cannot close Database, it was already closed.");
		return false;
	}
}

bool DBConnection::backupDatabase(QString backupName, QString dbName)
{
	if(!isDatabaseExisting())
	{
		createDatabase();
	}

	if(backupName.contains(" "))
	{
		backupName = "'" + backupName + "'";
	}
	if(dbName.contains(" "))
	{
		dbName = "'" + dbName + "'";
	}

	Log::write("Backup the database. Backup Name is: " + backupName);

	// Make backup
	QString command ="cp " + dbName + " " + backupName;
	// Return true if the command was successfully done by the system
	QString retCmd = SystemCommand::getStringFromSystemCommand(command);
	if(retCmd!="")
	{
		Log::writeError("dbConnection: Cannot create Database Backup: " + retCmd);
		this->appendError("Cannot create Database Backup.");
		return false;
	}
	else
	{
		return true;
	}
}

bool DBConnection::deleteBackup(QString backupName, QString dbName)
{
	if(backupName.contains(" "))
	{
		backupName = "'" + backupName + "'";
	}

	if(backupName == dbName)
	{
		Log::writeError("dbConnection: not allowed to delete original Database Name");
		this->appendError("Tried to delete original Database, not allowed.");
		return false;
	}
	else
	{
		if(QFile::remove(backupName))
		{
			Log::write("Deleted the Backup Database.");
			return true;
		}
		else
		{
			Log::writeError("dbConnection: Cannot delete Backup Database. Check user permissions.");
			this->appendError("Cannot delete Backup Database.");
			return false;
		}
	}
}

bool DBConnection::loadBackup(QString backupName, QString dbName)
{
	if(iButtonDB.isOpen())
	{
		Log::writeError("dbConnection: Cannot load Backup. All Connections have to be closed first.");
		this->appendError("Cannot load Backup. All connections have to be closed first.");
		return false;
	}
	else
	{
		if(!QFile::remove(dbName))
		{
			Log::writeError("dbConnection: Cannot remove the Original Database to replace it with the Backup.");
			this->appendError("Cannot remove original database.");
			return false;
		}

		QString retCmd = SystemCommand::getStringFromSystemCommand("mv " + backupName + " " + dbName);
		if(retCmd!="")
		{
			Log::writeError("dbConnection: Cannot load Backup Database: " + retCmd);
			this->appendError("Cannot load backup database.");
			return false;
		}
		return true;
	}
}

void DBConnection::appendError(QString error)
{
	this->errorReport.append(error + "\n");
}

QSqlDatabase DBConnection::getDB()
{
	return iButtonDB;
}

bool DBConnection::isTableExisting(QString tablename)
{
	QSqlQuery query (iButtonDB);
	query.exec("SELECT COUNT(*) FROM sqlite_master WHERE NAME='" + tablename + "';");
	if(query.next())
	{
		if(query.value(0).toInt()==1)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

bool DBConnection::update(QString table, QString compCol, QString compVal, QString replCol, QString replVal)
{
	QSqlQuery query(iButtonDB);
	QString text = "UPDATE " + table + " SET " + replCol + "=" + replVal
		+ " WHERE " + compCol + "=" + compVal + ";";

	bool success = query.exec(text);

	if(success)
	{
		return true;
	}
	else
	{
		Log::writeError("DBConnection: Cannot update entry: " + query.lastError().text());
		Log::writeError("SQL statement was:" + text);
		return false;
	}
}

QString DBConnection::read(QString table, QString compCol, QString compVal, QString getCol)
{
	QSqlQuery query(iButtonDB);
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

QString DBConnection::toSQLString(QString str)
{
	str = "'" + str + "'";
	return str;
}

void DBConnection::clearError()
{
	errorReport = "";
}

QString DBConnection::getErrorReport()
{
	QString ret = errorReport;
	clearError();
	return errorReport;
}

bool DBConnection::isSQLiteDriverInstalled()
{
	QStringList drivers = QSqlDatabase::drivers();
	if(drivers.contains("QSQLITE"))
	{
		return true;
	}
	else
	{
		Log::writeError("dbConnection: SQLite Driver for QT is not installed.");
		return false;
	}
}


