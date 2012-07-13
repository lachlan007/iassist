#ifndef DBBUTTONTABLE_H_
#define DBBUTTONTABLE_H_

#define BUTTONTABLENAME	"iButtons"

#include "DBConnection.h"
#include "../ButtonData.h"

/**
 * This class provides access to the table iButton in the database.
 * It supplies all necessary functions to edit and read the database.
 */

class DBButtonTable : public DBConnection {

public:
	DBButtonTable();
	virtual ~DBButtonTable();

	/**
	 * Creates a new Table to store information about iButtons
	 */
	bool createTable();

	/**
	 * Adds a Button to the Database
	 * @param button a ButtonData Object which contains all the information to store in the database
	 * @return true if successfully added to the database, false otherwise
	 */
	bool addButton(ButtonData button);

	/**
	 * Returns if the specified Button is existing in the iButton Table of the Database
	 * @param buttonNr the button Number of a specified Button
	 * @return true if found at least once, fals if not found
	 */
	bool isButtonExistingByButtonNr(QString buttonNr);

	/**
	 * Returns if the specified Button is existing in the iButton Table of the Database
	 * @param buttonNr the button's ID of a specified Button
	 * @return true if found at least once, fals if not found
	 */
	bool isButtonExistingByButtonID(QString buttonID);

	/**
	 * Reads a Button with the given ButtonNr to a ButtonData Object and returns it.
	 * @param _ButtonNr the Number of the Button
	 * @return all the information which are stored in the database
	 */
	ButtonData readButton(QString _ButtonNr);

	/**
	 * Returns a list of all the Buttons (ButtonNr) which are entered to the Database
	 * @return QStringList with all the ButtonNr in it
	 */
	QStringList getAllButtonNr();

	/**
	 * Returns a list of all the Buttons (ButtonNr) to a certain %Area
	 * @return QStringList with all the ButtonNr to a certain %Area
	 */
	QStringList getAllButtonNr(QString _area);

	/**
	 * Returns the highest Number of all the Buttons to a certain area (e.g. AA004 -> 4)
	 * @param _area the area, we want to have the highest ButtonNr
	 * @return The highest Number of all the Buttons which are in the Database
	 */
	int getLastAddedButtonNrInt(QString _area);

	/**
	 * Deletes a certain button. (Attention: Photos and Measurements have to be deleted as well)
	 * @param buttonNr Nr of the button which should be deleted
	 * @return true or false for the success state
	 */
	bool deleteButtonByButtonNr(QString buttonNr);

	/**
	 * Deletes all buttons which belong to a certain %Area.
	 * @param area The %Area ID (e.g. AA)
	 * @return true if successfully deleted, false otherwise
	 */
	bool deleteButtonByArea(QString area);

	/**
	 * Opens the Database. If no Database is existing, it creates one first. And if the
	 * table is not existing it creates a table as well.
	 */
	bool open();



private:
	void convertButtonDataToSQL(ButtonData *button);

	//iButton Table in iButtonDB
    QString ButtonNr;
    QString ButtonID;
 };

#endif /* DBBUTTONTABLE_H_ */
