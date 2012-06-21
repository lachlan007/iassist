#ifndef DBCONNECTION_H_
#define DBCONNECTION_H_

#define DBNAME 			"iButtonDB"
#define CONNECTIONNAME 	"dbConnection"

#include <QFile>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <cstdlib>
#include "Log.h"
#include "SystemCommand.h"

/**
 * DBConnection supplies support to manage one single Database with one single DatabaseConnection.
 * All Errors will be reported in the %Log File and as well in the internal Error Variable.
 */

class DBConnection {
public:
	/**
	 * Default constructor
	 */
	DBConnection();
	/**
	 * Constructs a Database object with a specified connection Name.
	 * @param _connectionName the desired connection Name.
	 */
	DBConnection(QString _connectionName);
	/**
	 * Default deconstructor
	 */
	virtual ~DBConnection();

	/**
	 * Creates a Database with the specified dbName. It only creates a new one, if there's no
	 * database existing.
	 * @param dbName the name of the Database to create
	 * @return true if a database was created, false otherwise
	 */
	bool createDatabase(QString dbName = DBNAME);
	/**
	 * Closes the database if close is called as many times as open was called. Otherwise simply decreases
	 * the amount of connections. Errors will be written to the Log file.
	 * @return true if successfully closed, false otherwise
	 */
	bool close();
	/**
	 * Makes a backup of a Database.
	 * @param backupName the Name the backup Database should have
	 * @param dbName the Name of the Database which should stored as backup
	 * @return true if successfully done a backup, false otherwise
	 */
	bool backupDatabase(QString backupName, QString dbName = DBNAME);
	/**
	 * Loads a backup of a Database.
	 * @param backupName the Name of the Backup Database
	 * @param dbName Name of the Database which should be replaced by the Backup Database
	 */
	bool loadBackup(QString backupName, QString dbName = DBNAME);
	/**
	 * Deletes the backup of a Database.
	 * @param backupName the Name of the backup Database to remove
	 * @param dbName the Name of the original Database, if specified
	 * @return true if deleted, false if not deleted
	 */
	bool deleteBackup(QString backupName, QString dbName = DBNAME);
	/**
	 * Returns if a Database is existing
	 * @param dbName the Name of the Database which has to be existing
	 * @return true if dbName is existing, false otherwise
	 */
	bool isDatabaseExisting(QString dbName = DBNAME);
	/**
	 * Appends an Error to errorReport String.
	 * @param error The error description to save.
	 */
	void appendError(QString error);

	/**
	 * Clears the errorReport string.
	 */
	void clearError();

	/**
	 * Returns the error and clears the ErrorReport Variable
	 * @return the Error Report of DBConnection
	 */
	QString getErrorReport();

	/**
	 * Returns the Database Name
	 * @return the database Name
	 */
	QSqlDatabase getDB();

	/**
	 * Returns if a certain table exists.
	 * @return true if already exists, false otherwise
	 */
	bool isTableExisting(QString tablename);

	/**
	 * Makes an update on a certain table. It does the following SQL statement:
	 * UPDATE table SET replCol = replVal WHERE compCol = compVal;
	 * @param table The table name of the DB
	 * @param compCol the column which will be compared to compVal
	 * @param compVal the value compCol should contain
	 * @param replCol the column which the replacement takes place
	 * @param replVal the new Value for replCol
	 * @returns true or false for success or failed
	 */
	bool update(QString table, QString compCol, QString compVal, QString replCol, QString replVal);

	/**
	 * Reads one specific field in the database. If more than one hit is found, only the first
	 * will be returned. It represents the following SQL statement:
	 * SELECT getCOL FROM table WHERE compCol=compVal
	 * @param table The table name of the DB
	 * @param compCol the column which will be compared to compVal
	 */
	QString read(QString table, QString compCol, QString compVal, QString getCol);

	/**
	 * Checks if the SQLite Driver for QT is installed.
	 * @return true if installed, false otherwise
	 */
	static bool isSQLiteDriverInstalled();


protected:
	QString errorReport;
	QString toSQLString(QString str);
	/**
	 * Opens the database with the specified Database Name. If an other name than the default
	 * DBNAME is desired, you have to specify it. Errors will be written to the Log file
	 * @param dbName the name of the Database to open
	 * @return true if successfully opened, false otherwise
	 */
	bool openDatabase(QString dbName = DBNAME);
};

#endif /* DBCONNECTION_H_ */
