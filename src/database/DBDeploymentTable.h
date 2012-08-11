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
#ifndef DBDEPLOYMENTTABLE_H_
#define DBDEPLOYMENTTABLE_H_

#define DEPLOYMENTTABLENAME	"Deployments"

#include "DBConnection.h"
#include <QVector>
#include <QPair>

/**
 * This class provides access to the table Footprints in the database.
 * It supplies all necessary functions to edit and read the database.
 */
class DBDeploymentTable : public DBConnection {
public:
	/**
	 * Default constructor
	 */
	DBDeploymentTable();
	/**
	 * Default deconstructor
	 */
	virtual ~DBDeploymentTable();

	/**
	 * Creates a new Deployment Table, if there's no one existing
	 */
	bool createTable();

	bool addDeployment(QString deploymentName, int &insertId);

	bool deleteDeployment(int deploymentID);

	QVector<QPair<int,QString> > getAllDeployments();

	/**
	 * Opens the Database. If no Database is existing, it creates one first. And if the
	 * table is not existing it creates a table as well.
	 */
	bool open();

};

#endif /* DBDEPLOYMENTTABLE_H_ */
