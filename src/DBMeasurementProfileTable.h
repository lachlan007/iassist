#ifndef DBMEASUREMENTPROFILETABLE_H_
#define DBMEASUREMENTPROFILETABLE_H_

#define MEASUREMENTPROFILETABLENAME "MeasurementProfiles"

#include "DBConnection.h"
#include "MeasurementProfile.h"

/**
 * This class provides access to the table MeasurementProfiles in the database.
 * It supplies all necessary functions to edit and read the database.
 */


class DBMeasurementProfileTable : public DBConnection{
public:
	/**
	 * Default constructor
	 */
	DBMeasurementProfileTable();
	/**
	 * Default deconstructor
	 */
	virtual ~DBMeasurementProfileTable();

	/**
	 * Creates a %MeasurementProfile table in the database
	 */
	bool createTable();

	/**
	 * Adds a %MeasurementProfile to the Database. If this record is already existing it returns false.
	 * @param profile The profile to add to the Database
	 * @return true if successfully done, false otherwise
	 */
	bool addProfile(MeasurementProfile profile);

	/**
	 * Update a %MeasurementProfile in the Database. It updates the record which correspond to the MeasurementID.
	 * Only the values are updated which are set by the user
	 * @param profile the Data which should be updated in the database.
	 * @return true if successfully done, false otherwise.
	 */
	bool updateProfile(MeasurementProfile profile);


	/**
	 * Reads a profile according to a ID from the database and returns it as a %MeasurementProfile object.
	 * @param _measurementProfileID the id of the desired profile in the database
	 * @return a MeasurementProfile object
	 */
	MeasurementProfile readProfile(QString _measurementProfileID);

	/**
	 * Returns the latest added MeasurementProfileID to a ButtonNr
	 * @param _buttonNr the number of the button whose last %MeasurementProfile ID we want to have
	 * @return the latest added MeasurementID
	 */
	int getLatestAddedProfileIDByButtonNr(QString _buttonNr);

	/**
	 * Returns the latest added SessionNr to a ButtonNr. The Measurement Nr is the amount
	 * of measurement series of this button.
	 * @param _buttonNr the number of the button (e.g. AA001)
	 * @return the amount of measurements sessions to a Button (SessionNr)
	 */
	int getLatestSessionNrByButtonNr(QString _buttonNr);

	/**
	 * Opens the database connection. Adds a table if no one is existing and creates
	 * a database if no one is existing.
	 * @return true if successful, false otherwise
	 */
	bool open();

	/**
	 * Deletes the Measurement Profiles to a certain area from the database.
	 * @param _area the area
	 * @return true if successful, false otherwise
	 */
	bool deleteProfileByArea(QString _area);

	/**
	 * Deletes the Measurement Profiles to a certain button from the database.
	 * @param _buttonNr the buttonNr
	 * @return true if successful, false otherwise
	 */
	bool deleteProfileByButtonNr(QString _buttonNr);

	/**
	 * Returns measurement profiles for which the related measurement data is
	 * already in the database.
	 * @return Vector
	 */
	QVector<MeasurementProfile> getFinishedMeasurementProfiles();

private:

	/**
	 * Converts all the values of a MeasurementProfile to a SQLite representation
	 * @param profile a pointer to the MeasurementProfile Object
	 */
	void convertDataToSQL(MeasurementProfile *profile);

	// all the variables of the DBMeasurementProfileTable
	QString ButtonNr;
	QString SessionNr;
	QString MeasurementProfileID;
	QString ProgrammingTime;
	QString CollectingTime;
	QString TimeShift;
	QString SamplingRate;
	QString SamplingStartTimeStamp;
	QString Resolution;

};

#endif /* DBMEASUREMENTPROFILETABLE_H_ */
