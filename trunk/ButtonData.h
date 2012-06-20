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
	//! Name of the Distributor
	QString Distributor;
	//! Timestamp when button was installed
	QString Timestamp;
	//! PDOP
	double PDOP;
	//! HDOP
	double HDOP;
	//! Standard Deviation
	double StandardDeviation;
	//! HAE_DEM
	double HAE_DEM;
	//! East_DEM
	double East_DEM;
	//! North_DEM
	double North_DEM;
	//! Slope_DEM
	double Slope_DEM;
	//! SolDy_DEM
	double SolDy_DEM;
	//! SolYr_DEM
	double SolYr_DEM;
};

#endif /* BUTTONDATA_H_ */
