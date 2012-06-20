#ifndef DBAREATABLE_H_
#define DBAREATABLE_H_

#define AREATABLENAME	"Footprints"

#include "DBConnection.h"
#include "Area.h"

/**
 * This class provides access to the table Footprints in the database.
 * It supplies all necessary functions to edit and read the database.
 */
class DBAreaTable : public DBConnection {
public:
	/**
	 * Default constructor
	 */
	DBAreaTable();
	/**
	 * Default deconstructor
	 */
	virtual ~DBAreaTable();

	/**
	 * Creates a new Footprint Table, if there's no one existing
	 */
	bool createTable();

	/**
	 * Adds a Footprint to the Database
	 * @param area a Area Object
	 * @return true or false for the success state
	 */
	bool addArea(Area areaData);

	/**
	 * Deletes a certain Footprint out of the Footprint (%Area) Table of the Database.
	 * Attention: Buttons, Photos and Measurements have to be deleted separately.
	 * @param area are which should be deleted
	 * @return true or false for the success state
	 */
	bool deleteArea(QString area);

	/**
	 * Returns if the specified %Area is already entered in the Footprint Table of the iButton Database
	 * @param area %Area to look for
	 * @return true if found at least once, false if not found
	 */
	bool isAreaExisting(QString area);

	/**
	 * Updates the %Area which has the FootprintID of this %Area with the values stored in the parameter %Area.
	 * If you wish to update just a few values of a %Area, just let the corresponding variable in the Area Object
	 * empty. or equal to -9999.
	 * @param area the %Area which has to be updated
	 * @return true for success, false for failed
	 */
	bool updateArea(Area area);

	/**
	 * Returns QStringList with all the Areas (Footprint ID).
	 * @return a list with all the FootprintIDs
	 */
	QStringList getAllArea();

	/**
	 * Returns an Area Object which contains all the Data of a specified %Area which are stored in the database
	 * @param footprintID the ID of the Footprint (%Area) we want to read
	 * @return the Area Object, filled with data found in database
	 */
	Area readArea(QString footprintID);

	/**
	 * Opens the Database. If no Database is existing, it creates one first. And if the
	 * table is not existing it creates a table as well.
	 */
	bool open();



private:
	/**
	 * Converts Area object to proper SQL Language which is needed for SQLite
	 * this means if a number doesn't exist there has to be NULL submitted to the database
	 * and if a String doesn't exist there has to be submitted '' to the SQLite database.
	 * @param area a pointer to the Area Object
	 */
	void convertAreaDataToSQL(Area *area);

	//Footprints Table in iButtonDB
	QString AreaID;
	QString AAltitude;
	QString ALongitude;
	QString ALatitude;
	QString AHAE_DEM;
	QString AEast_DEM;
	QString ANorth_DEM;
	QString ASlope_DEM;
	QString AAspect_DEM;
	QString ASolDy_DEM;
	QString ASolYr_DEM;
	QString AStartLongitude;
	QString AEndLongitude1;
	QString AEndLongitude2;
	QString AStartLatitude;
	QString AEndLatitude1;
	QString AEndLatitude2;
};

#endif /* DBAREATABLE_H_ */
