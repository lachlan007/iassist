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
#ifndef BUTTONIO_H_
#define BUTTONIO_H_

#include <iostream>
#include <time.h>
#include <QDateTime>
#include "buttonIO/ownet.h"
#include "buttonIO/findtype.h"
#include "Log.h"
#include <QString>
#include "MissionData.h"
#include "ThermoHygrochronButton.h"
#include "ThermochronButton.h"

#define FAMILY_THERMOHYG 0x41
#define FAMILY_THERMO 0x21

using namespace std;
/**
 * ButtonIO provides functions to communicate with iButton sensors.
 */
class ButtonIO
{
	public:

		/**
		 * Constructor
		 */
		ButtonIO();


		/**
		 * Destructor
		 */
		~ButtonIO();

		/**
		 * Release the USB port
		 * @return true for successful / false for unsuccessful
		 */
		bool closePort();


		/**
		 * Reads the logged data from iButton sensor
		 * @return true for successful / false for unsuccessful
		 */
		bool downloadMissionData(uchar *SNum, MissionData &data);


		/**
		 * Attempt to acquire the specified 1-Wire net.
		 * @return true for successful / false for unsuccessful
		 */
		bool openPort();


		/**
		 * Start a new measuring mission
		 * @return true for successful / false for unsuccessful
		 */
		bool startMission(uchar *SNum);


		/**
		 * Stop a running mission.
		 * @return true for successful / false for unsuccessful
		 */
		bool stopMission(uchar *SNum);

		/**
		 * Figures out if there is an iButton connected to the
		 * reader or not.
		 * @return true for a found device / false for no device connected to the reader.
		 */
		bool getConnectedButton(uchar *SNum);

		/**
		 * Determines if a mission is currently running.
		 * @return true for mission is already running.
		 * @return false for no mission is running.
		 */
		bool isMissionInProgress(uchar *SNum);

		static QString buttonIDStr(uchar *SNum);

		static bool isThermoHygrochron(uchar *SNum);

		bool getCalibrationCoefficients(uchar* SNum, double& coeffA, double& coeffB, double& coeffC);

	private:

		int portnum;

		int findDevice(uchar *SNum, int deviceFamily); //This function searches for devices
														//connected to the 1-Wire network

};

#endif // BUTTONIO_H_
