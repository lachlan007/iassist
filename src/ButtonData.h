#ifndef BUTTONDATA_H_
#define BUTTONDATA_H_

#include <QString>

/**
 * This class is used to store all the available information about the GPS Position.
 */
class ButtonData {
public:
	/**
	 * Default constructor
	 */
	ButtonData();
	/**
	 * Default deconstructor
	 */
	virtual ~ButtonData();
	/**
	 * Deletes all the stored information. This is used to reuse this object after information were stored.
	 */
	void clearData();

	//! ButtonNr of the Button e.g. AA0001
	QString ButtonNr;
	//! Button ID of the iButton e.g. 00:A2:00:00:B2:04:72:1B
	QString ButtonID;
};

#endif /* BUTTONDATA_H_ */
