#ifndef BUTTONIO_H_
#define BUTTONIO_H_

// filepath for "mission_start_parameter.ext" file
#define	MISSION_PARAMETER_FILEPATH	"./mission_start_parameter.txt"

#include <iostream>
#include <time.h>
#include <qdatetime.h>
#include "buttonIO/ownet.h"
#include "buttonIO/findtype.h"
#include "buttonIO/humutil.h"
#include "buttonIO/mbee77.h"
#include "buttonIO/thermo21.h"
#include "Log.h"
#include <QString>

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

		unsigned char getRegisterContent(int Address);

		/**
		 * Result of a single temperature measurement. Do not use
		 * before function 'downloadCurrentTemperature()' called.
		 */
		double temp_result;		//result of single temperature measurement


		/**
		 * Reads the sampling rate set in the mission start parameters.
		 * @return The sampling rate if successful.
		 * @return '-1' if unsucessful.
		 */
		int getSamplingRate();


		/**
		 * Reads the sampling resolution set in the mission start parameters.
		 * @return The sampling resolution if successful.
		 * @return '-1' if unsucessful.
		 */
		int getSamplingResolution();


		/**
		 * Reads the number of stored measurements. Do not call this function
		 * before reading the stored data with the function 'downloadMissionData()'.
		 * @return The number of stored measurements.
		 */
		int getSampleSize();


		/**
		 * Returns a pointer to the sample array. Do not call this function
		 * before reading the stored data with the function 'downloadMissionData()'.
		 * @return Pointer to the array that contains the sampled data.
		 */
		double *getSampleArray();


		/**
		 * Release the USB port
		 * @return true for successful / false for unsuccessful
		 */
		bool closePort();


		/**
		 * Reads the logged data from iButton sensor
		 * @return true for successful / false for unsuccessful
		 */
		bool downloadMissionData();


		/**
		 * Attempt to acquire the specified 1-Wire net.
		 * @return true for successful / false for unsuccessful
		 */
		bool openPort();


		/**
		 * Start a new measuring mission
		 * @return true for successful / false for unsuccessful
		 */
		bool startMissionOnButton();


		/**
		 * Stop a running mission.
		 * @return true for successful / false for unsuccessful
		 */
		bool stopMissionOnButton();


		/**
		 * Measure the current temperature
		 * @return true for successful / false for unsuccessful
		 */
		bool downloadCurrentTemperature();


		/**
		 * Figures out if there is an iButton connected to the
		 * reader or not.
		 * @return true for a found device / false for no device connected to the reader.
		 */
		bool isButtonConnected();


		/**
		 * Reads the iButton RC time and date.
		 * @return true successful / false for unsuccessful.
		 */
		bool downloadDeviceTimeStamp();


		/**
		 * Determines if a mission is currently running.
		 * @return true for mission is already running.
		 * @return false for no mission is running.
		 */
		bool isMissionInProgress();


		/**
		 * Reads the iButton ID number
		 * @return The Button ID if sucessful.
		 * @return "false" if unsuccessful.
		 */
		QString getButtonID();


		/**
		 * Reads the OS time and date
		 * @return The OS timestamp.
		 */
		QString getLocalTimeStamp();


		/**
		 * Converts the device timestamp into a QString. Do not call this
		 * function before reading the device timestamp with 'downloadDeviceTimeStamp()'.
		 * @return The device timestamp.
		 */
		QString getDeviceTimeStamp();


		/**
		 * Converts the mission start timestamp into a QString. Do not call
		 * this functin before starting a new mission with 'startMission()'.
		 * @return The OS timestamp.
		 */
		QString getSamplingStartTime();


		/**
		 * Calculates the iButton RTC time shift compared to local system time.
		 * If the iButton RTC timestamp is earlier than the system timestamp,
		 * the value returned is negative.
		 * Do always call the function 'downloadDeviceTimeStamp()'.
		 * @return time shift in seconds.
		 */
		int getTimeShift();


	private:

		int portnum;				//port number of USB port
		int buttonType;
		int sample_No;				//number of sampled data
		int input[8];				//array that contains mission parameter settings
		int hour,min,sec;
		int month,day,year;
		int pm;
		int Device;					//device number
		int local_time[3];			//array with the OS time
									//..[0] = hours
									//..[1] = minutes
									//..[2] = seconds
		int local_date[3];			//array with the OS date
									//..[0] = day
									//..[1] = month
									//..[2] = year
		int device_time[3];			//array with the device time
									//..[0] = hours
									//..[1] = minutes
									//..[2] = seconds
		int device_date[3];			//array with the device date
									//..[0] = day
									//..[1] = month
									//..[2] = year
		int mission_start_timestamp[6];		//array with mission start timestamp
											//..[0] = day, ..[1] = month, ..[2] = year
											//..[0] = hour, ..[1] = min, ..[2] = sec
		QDateTime samplingStartTime; // holds the time when the current iButton starts sampling
		double val;
		double valsq;
		double error;
		double *sample_Array;		//array that containss all the sampled data
									//after reading it form the device
		string line;
		string iButton_ID;			//string with iButton ID

		uchar upper, lower;
		uchar SNum[8];				//char array that contains the iButton ID
		uchar state[96];			//array that contains the current state of the iButton
									//and all the register flags and setting values

		configLog config;			//struct with configuration data used to read
									//the logged data from the device
		startMissionData settings;	//struct with settings used to start a mission
		time_t tlong;				//used to get the OS time
		struct tm *tstruct;			//struct with the OS time and date
		QDateTime sysDateTime;		//system time as a QDateTime object


		int selectDevice(int portnum, uchar *SNum);		//This function searches for devices
														//connected to the 1-Wire network

        /**
         * Start a new measuring mission
         * @return true for successful / false for unsuccessful
         */
        bool startMissionOnButtonThHyg();

        /**
         * Start a new measuring mission
         * @return true for successful / false for unsuccessful
         */
        bool startMissionOnButtonThermo();

        /**
         * Stop a running mission.
         * @return true for successful / false for unsuccessful
         */
        bool stopMissionOnButtonThHyg();

        /**
         * Stop a running mission.
         * @return true for successful / false for unsuccessful
         */
        bool stopMissionOnButtonThermo();

        /**
         * Determines if a mission is currently running.
         * @return true for mission is already running.
         * @return false for no mission is running.
         */
        bool isMissionInProgressThHyg();

        /**
         * Determines if a mission is currently running.
         * @return true for mission is already running.
         * @return false for no mission is running.
         */
        bool isMissionInProgressThermo();

        /**
         * Reads the iButton RC time and date.
         * @return true successful / false for unsuccessful.
         */
        bool downloadDeviceTimeStampThHyg();

        /**
         * Reads the iButton RC time and date.
         * @return true successful / false for unsuccessful.
         */
        bool downloadDeviceTimeStampThermo();

        /**
         * Reads the logged data from iButton sensor
         * @return true for successful / false for unsuccessful
         */
        bool downloadMissionDataThHyg();

        /**
         * Reads the logged data from iButton sensor
         * @return true for successful / false for unsuccessful
         */
        bool downloadMissionDataThermo();


};

#endif // BUTTONIO_H_
