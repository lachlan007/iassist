
#ifndef MEASUREMENTPROFILE_H_
#define MEASUREMENTPROFILE_H_

#include <QString>

/**
 * Holds all the values containung to a measurement profile.
 */
class MeasurementProfile {
public:
	/**
	 * Default constructor, which clears all the data
	 */
	MeasurementProfile();
	/**
	 * Default deconstructor
	 */
	virtual ~MeasurementProfile();

	//! MeasurementProfile ID
	int MeasurementProfileID;
	//! The number of the Button, the %MeasurementProfile belongs to
	QString ButtonNr;
	//! The amount of Measurements of this button (how many sessions)
	int SessionNr;
	//! The Time when the button starts the measurement
	QString SamplingStartTimeStamp;
	//! Time when the Button was collected again (End of the measurement)
	QString CollectingTime;
	//! Time when the Button was distributed
	QString ProgrammingTime;
	//! Timeshift between the time on the button and the time on the system
	int TimeShift;
	//! Sampling Rate
	int SamplingRate;
	//! Resolution 8bit or 16bit
	int Resolution;

	/**
	 * Clears all the Data
	 */
	void clearData();
};

#endif /* MEASUREMENTPROFILE_H_ */
