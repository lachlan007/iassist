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
 * The Log class provides the use of a Log file for error messages
 * and comments.
 */
#ifndef LOG_H_
#define LOG_H_

#include <iostream>
#include <fstream>
#include <QString>
#include <QDateTime>
#include <QtCore>

using namespace std;

class Log
{
	public:

		/**
		 * default Constructor
		 */
		Log();

		/**
		 * default Deconstructor
		 */
		~Log();

		/**
		 * Initializes the Log file. Do always call this function
		 * before using other Log functions.
		 * @return true for successful / false for unsuccessful
		 */
		static bool logfile_init();

		/**
		 * Writes an error message of the form ***Error: ... ***
		 * into the Log file
		 */
		static void writeError(QString errlogline);

		/**
		 * Writes a message into the Log file.
		 */
		static void write(QString logline);


	private:
		static ofstream  m_stream;	//file handle

		static bool close_file(void);	//this function will open the %Log file
										//returns true if successful / false for unsuccessful
		static bool open_file(void);	//this function will close the %Log file
										//returns true if successful / false for unsuccessful
		static time_t tlong;		//used to get the OS time
		static struct tm *tstruct;	//struct that contains the OS time and date

		static QString currentLogFileName;
};

#endif /* LOG_H_ */
