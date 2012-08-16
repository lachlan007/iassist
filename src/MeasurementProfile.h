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
	//! The index of the corresponding entry in the button table
	int ButtonId;
	//! The amount of Measurements of this button (how many sessions)
	int SessionNr;
	//! The Time when the button starts the measurement
	QString SamplingStartTime;
	//! Time when the Button was collected again (End of the measurement)
	QString CollectingTimeHost;
	//! Time when the Button was collected again (End of the measurement)
	QString CollectingTimeButton;
	//! Time when the Button was distributed
	QString ProgrammingTime;
	//! Sampling Rate
	int SamplingRate;
	//! Enabled high resolution mode
	int Resolution;
	//! Software temperature calibration was enabled when reading values
	int TempCalibUsed;

	/**
	 * Clears all the Data
	 */
	void clearData();
};

#endif /* MEASUREMENTPROFILE_H_ */
