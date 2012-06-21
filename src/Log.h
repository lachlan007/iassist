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
