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

	//! ButtonNr of the Button (3-digit footprint prefix, 3-digit suffix) e.g. 123456
	int ButtonNr;
	//! Serial number of the iButton e.g. 00:A2:00:00:B2:04:72:1B
	QString SerialNr;

	// Factory-set calibration coefficients for DS1922
	double CalibCoeffA;
	double CalibCoeffB;
	double CalibCoeffC;

};

#endif /* BUTTONDATA_H_ */
