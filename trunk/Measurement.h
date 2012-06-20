/**
 * Measurement: Class that provides all the variables to store the measurements of a button
 *
 */

#ifndef MEASUREMENT_H_
#define MEASUREMENT_H_

#include <QString>

/**
 * Holds all the variables containing to a measurement
 */
class Measurement {
public:

	/**
	 * Default constructor
	 */
	Measurement();
	/**
	 * Default deconstructor
	 */
	virtual ~Measurement();

	/**
	 * Resets all the values
	 */
	void cleanMeasurement();

	//! points to the array of the data saved in this button
	double *missionData;
	//! is the buttonNr of this button
	QString ButtonNr;
	//! is the size of this measurement
	int size;
	//! is the link to the measurement profile
	int MeasurementProfileID;
};

#endif /* MEASUREMENT_H_ */
