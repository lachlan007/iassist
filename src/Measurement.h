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
