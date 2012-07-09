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
#include "ButtonIO.h"
using namespace std;


//---------------------------------------------------------------------------
// Constructor
//
// 'portnum' - the port number of the port being used for the
//			   1-Wire Network.
//---------------------------------------------------------------------------
ButtonIO::ButtonIO()
{
	portnum = 0;
	sample_Array = NULL;
}


//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
ButtonIO::~ButtonIO()
{

}


unsigned char ButtonIO::getRegisterContent(int Address)
{
	return readByte(portnum,SNum,Address);
}

bool ButtonIO::startMissionOnButton()
{
    if(buttonType == 0)
    {
        return startMissionOnButtonThHyg();
    }
    return startMissionOnButtonThermo();
}

//---------------------------------------------------------------------------
// This function will start a new mission.
//
// 'mission_start_parameter.txt' - contains all settings and information to
//								   start a new mission
// 'programming_delay' - delay in days.
//
// returns:  'true' 	if start the mission succeed
//			 'false' 	if start the mission failed
//---------------------------------------------------------------------------
bool ButtonIO::startMissionOnButtonThHyg()
{
	int u = 0;
	int retry = 5;

	for(int w=0;w<6;w++)
	{
		mission_start_timestamp[w] = 0;
	}

	usleep(5000);


	// open "mission_start_parameter.txt"
	ifstream tmplate;
	tmplate.open (MISSION_PARAMETER_FILEPATH, ifstream::in);

	if (tmplate.is_open())
	{
		while(!tmplate.eof())
	    {
			getline (tmplate,line);
	        input[u] = atoi(line.c_str());
	        u=u+1;
		}
	    tmplate.close();
	}
	else
	{
		Log::writeError("startMissionOnButton: Cannot open mission parameter file");
		return false;
	}

	// check if the settings value for sample rate and start delay
	// are inside their boundaries.
	if((input[1]<1) || (input[1]>982800))
	{
		Log::writeError("mission parameters: Sample rate is out of its limits.");
		return false;
	}
	else if((input[2]<0) || (input[2]>16777215))
	{
		Log::writeError("mission parameters: Start delay is out of its limits.");
		return false;
	}
	else if((input[8]<0) || (input[8])>365)
	{
		Log::writeError("mission parameters: Day delay is out of its limits.");
		return false;
	}

	// check for invalid boolean settings in "mission_start_parameters".
	for(int t=3;t<=5;t++)
	{
		if(!((input[t]==0) || (input[t]==1)))
		{
			Log::writeError("mission parameters: Boolean setting has a wrong value.");
			return false;
		}
	}

	settings.sampleRate = input[1];		// set the sample rate

	// Calculate Delay
	if(input[6]==0)	// calculate mission start timestamp when
	{				// start at 00:00 is disabled.
		settings.startDelay = input[2];
		this->samplingStartTime = QDateTime::currentDateTime();
		this->samplingStartTime = this->samplingStartTime.addSecs(60*input[2]);
	}
	else if(input[6]==1)	// calculate mission start timestamp when
	{						// start at specific time is enabled.
	    QTime dayOffset(input[7]/60, input[7]-(input[7]/60)*60);
	    int extraDay = (dayOffset < QTime::currentTime()) ? 1 : 0;
	    QDateTime startTime = QDateTime(QDate::currentDate().addDays(extraDay + input[8]), dayOffset);
		QDateTime now = QDateTime::currentDateTime();

		double secsDelay = now.secsTo(startTime)/60.0;
		if((secsDelay - (int)secsDelay)>0.5)
		{
			settings.startDelay = ((int) secsDelay) + 1;
		}
		else
		{
			settings.startDelay = (int) secsDelay;
		}
		this->samplingStartTime = startTime;
	}


	// set mission parameter values from "mission_start_parameter" file
	settings.enableRollover = input[3];
	settings.timeSync = input[4];
	settings.tempEnabled = 1;
	settings.dataEnabled = 0;
	settings.highDataResolution = 0;
	settings.highTempAlarm = 0;
	settings.lowTempAlarm = 0;
	settings.highDataAlarm = 0;
	settings.lowDataAlarm = 0;
	settings.tempAlarm = 0;
	settings.highTempResolution = input[5];

	while(retry > 0)
	{
		retry--;

		usleep(1500);
		// check if the USB port is opened
		if(owUSB_is_port_open(portnum) == 0)
		{
			Log::writeError("startMissionOnButton: USB port is closed.");
			return false;
		}
		else if(!(owVerify(portnum, false)))	// check if the current iButton device is on 1-Wire net
		{
			Log::writeError("startMissionOnButton: The current iButton is not on the 1-Wire net.");
			//OWERROR_DUMP(stdout);
			continue;
		}

		usleep(2000);
		// start mission
		if(!startMission(portnum,&SNum[0],settings,&config))
		{
			// OWERROR_DUMP(stdout);
			Log::writeError("startMissionOnButtonThHyg: Cannot start mission.");
		}
		else
		{
			Log::write("startMissionOnButtonThHyg: Mission started.");
			return true;
		}
	}

	return false;
}

bool ButtonIO::startMissionOnButtonThermo() {

    ThermoStateType thermoState;

    int u = 0;
    int retry = 5;

    for(int w=0;w<6;w++)
    {
        mission_start_timestamp[w] = 0;
    }

    usleep(5000);


    // open "mission_start_parameter.txt"
    ifstream tmplate;
    tmplate.open (MISSION_PARAMETER_FILEPATH, ifstream::in);

    if (tmplate.is_open())
    {
        while(!tmplate.eof())
        {
            getline (tmplate,line);
            input[u] = atoi(line.c_str());
            u=u+1;
        }
        tmplate.close();
    }
    else
    {
        Log::writeError("startMissionOnButton: Cannot open mission parameter file");
        return false;
    }

    // check if the settings value for sample rate and start delay
    // are inside their boundaries.
    if((input[1]<1) || (input[1]>982800))
    {
        Log::writeError("mission parameters: Sample rate is out of its limits.");
        return false;
    }
    else if((input[2]<0) || (input[2]>16777215))
    {
        Log::writeError("mission parameters: Start delay is out of its limits.");
        return false;
    }
    else if((input[8]<0) || (input[8])>365)
    {
        Log::writeError("mission parameters: Day delay is out of its limits.");
        return false;
    }

    // check for invalid boolean settings in "mission_start_parameters".
    for(int t=3;t<=5;t++)
    {
        if(!((input[t]==0) || (input[t]==1)))
        {
            Log::writeError("mission parameters: Boolean setting has a wrong value.");
            return false;
        }
    }

     // Calculate Delay
    if(input[6]==0) // calculate mission start timestamp when
    {               // start at 00:00 is disabled.
        thermoState.MissStat.start_delay = input[2];
        this->samplingStartTime = QDateTime::currentDateTime();
        this->samplingStartTime = this->samplingStartTime.addSecs(60*input[2]);
    }
    else if(input[6]==1)    // calculate mission start timestamp when
    {                       // start at specific time is enabled.
        QTime dayOffset(input[7]/60, input[7]-(input[7]/60)*60);
        int extraDay = (dayOffset < QTime::currentTime()) ? 1 : 0;
        QDateTime startTime = QDateTime(QDate::currentDate().addDays(extraDay + input[8]), dayOffset);
        QDateTime now = QDateTime::currentDateTime();

        double secsDelay = now.secsTo(startTime)/60.0;
        if((secsDelay - (int)secsDelay)>0.5)
        {
            thermoState.MissStat.start_delay = ((int) secsDelay) + 1;
        }
        else
        {
            thermoState.MissStat.start_delay = (int) secsDelay;
        }
        this->samplingStartTime = startTime;
    }

    while(retry > 0)
    {
        retry--;

        usleep(1500);

        // check if the USB port is open
        if(owUSB_is_port_open(portnum) == 0)
        {
            Log::writeError("ButtonIO::getButtonID: USB port is closed.");
            return "failed";
        }
        else if(!(owVerify(portnum, false)))    // check if the current iButton device is on 1-Wire net
        {
            Log::writeError("ButtonIO::getButtonID: The current iButton is not on the 1-Wire net.");
            return "failed";
        }

        if (ReadThermoStatus(portnum,&SNum[0],&thermoState,stdout))
        {
            InterpretStatus(&thermoState.MissStat);

            // set mission parameter values from "mission_start_parameter" file
            thermoState.MissStat.rollover_enable = 0; //input[3];
            thermoState.MissStat.sample_rate = 1; // input[1];
            thermoState.MissStat.high_threshold = 220;
            thermoState.MissStat.low_threshold = 0;
            thermoState.MissStat.start_delay = 0;

            if(!MissionThermo(portnum,&SNum[0],&thermoState,stdout))
            {
                // OWERROR_DUMP(stdout);
                Log::writeError("startMissionOnButtonThermo: Cannot start mission.");
            }
            else
            {
                Log::write("startMissionOnButtonThermo: Mission started.");
                return true;
            }
        }
    }

    return false;
}


//---------------------------------------------------------------------------
// This function will return the sampling rate from the mission parameters
//
// Returns:  'input[1]'		sampling rate
//			 '-1'			if reading mission parameters failed.
//---------------------------------------------------------------------------
int ButtonIO::getSamplingRate()
{
	int u = 0;

	// open "mission_start_parameter.txt"
	ifstream tmplate;
	tmplate.open (MISSION_PARAMETER_FILEPATH, ifstream::in);

	if (tmplate.is_open())
	{
		while(!tmplate.eof())
		{
			getline (tmplate,line);
			input[u] = atoi(line.c_str());
			u=u+1;
		}
		tmplate.close();
	}
	else
	{
		Log::writeError("ButtonIO::getSamlingRate: Cannot open mission parameter file.");
		return -1;
	}

	//check if sampling rate is out of its boundary limits
	if((input[1]<1) || (input[1]>982800))
	{
		Log::writeError("mission parameters: Sample rate is out of its limits.");
		return -1;
	}

	return input[1];
}


//---------------------------------------------------------------------------
// This function will return the sampling resolution from the mission
// parameters
//
// Returns:  'rate'		sampling resolution
//			 '-1'		if reading mission parameters failed.
//						or the mission parameter settings contains
//						an invalid number
//---------------------------------------------------------------------------
int ButtonIO::getSamplingResolution()
{
	int u = 0;
	int rate = 0;

	// open "mission_start_parameter.txt"
	ifstream tmplate;
	tmplate.open (MISSION_PARAMETER_FILEPATH, ifstream::in);

	if (tmplate.is_open())
	{
		while(!tmplate.eof())
		{
			getline (tmplate,line);
			input[u] = atoi(line.c_str());
			u=u+1;
		}
		tmplate.close();
	}
	else
	{
		Log::writeError("ButtonIO::getSamplingResolution: Cannot open mission parameter file.");
		return -1;
	}

	//check if the mission parameter 'high temperature resolution' is set to a valid number
	if(!((input[5]==0) || (input[5]==1)))
	{
		Log::writeError("mission parameters: a boolean setting has a wrong value.");
		return -1;
	}

	if(input[5]==0)
	{
		rate = 8; //8 bit
	}
	else if(input[5]==1)
	{
		rate = 16; //16 bit
	}

	return  rate;
}

bool ButtonIO::stopMissionOnButton()
{
    if(buttonType == 0)
    {
        return stopMissionOnButtonThHyg();
    }
    return stopMissionOnButtonThermo();
}

//---------------------------------------------------------------------------
// This function will stop a running mission. If no mission is running it
// will return false.
//
// Returns:  'true' 	if stop the mission succeed
//			 'false' 	if stop the mission failed
//---------------------------------------------------------------------------
bool ButtonIO::stopMissionOnButtonThHyg()
{
	int retry = 5;

	while(retry > 0)
	{
		retry--;

		usleep(1500);
		// check if port is opened
		if(owUSB_is_port_open(portnum) == 0)
		{
			Log::writeError("ButtonIO::stopMissionOnButton: USB port is closed.");
			return false;
		}
		else if(!(owVerify(portnum, false)))	// check if the current iButton device is on 1-Wire net
		{
			Log::writeError("ButtonIO::stopMissionOnButton: The current iButton is not on the 1-Wire net.");
			continue;
		}

		usleep(2000);
		// stops the currently running mission.
		if(!stopMission(portnum,&SNum[0]))
		{
			OWERROR_DUMP(stdout);
			Log::writeError("ButtonIO::stopMissionOnButton: Cannot stop mission.");
		}
		else
		{
			Log::write("ButtonIO::stopMissionOnButton: Mission stopped.");
			// cout << "Mission stopped" << endl;
			return true;
		}
	}

	return false;
}

bool ButtonIO::stopMissionOnButtonThermo()
{
    return true;
}


//---------------------------------------------------------------------------
// This function will figure out, if a mission is currently running.
//
//  Return:  'true'     if a mission is already running
//           'false'    if no mission in progress
//---------------------------------------------------------------------------
bool ButtonIO::isMissionInProgress()
{
    usleep(5000);
    // check if port is opened
    if(owUSB_is_port_open(portnum) == 0)
    {
        Log::writeError("ButtonIO::isMissionInProgress: USB port is closed.");
        return false;
    }
    else if(!(owVerify(portnum, false)))    // check if the current iButton device is on 1-Wire net
    {
        Log::writeError("ButtonIO::isMissionInProgress: The current iButton is not on the 1-Wire net.");
        return false;
    }

    if(buttonType == 0)
    {
        return isMissionInProgressThHyg();
    }
    return isMissionInProgressThermo();

}

//---------------------------------------------------------------------------
// This function will figure out, if a mission is currently running.
//
//	Return:  'true'		if a mission is already running
//			 'false'	if no mission in progress
//---------------------------------------------------------------------------
bool ButtonIO::isMissionInProgressThHyg()
{
	usleep(2000);
	// read device status register
	if (!getFlag(portnum, SNum, GENERAL_STATUS_REGISTER, GSR_BIT_MISSION_IN_PROGRESS))
	{
		// OWERROR(OWERROR_HYGRO_STOP_MISSION_UNNECESSARY);
		Log::write("ButtonIO::isMissionInProgressThHyg: No mission in progress.");
		// cout << "No Mission in progress" << endl;
		return false;
	}
	else
	{
		Log::write("ButtonIO::isMissionInProgressThHyg: Mission is in progress.");
		// cout << "Mission is in progress" << endl;
		return true;
	}
}

//---------------------------------------------------------------------------
// This function will figure out, if a mission is currently running.
//
//  Return:  'true'     if a mission is already running
//           'false'    if no mission in progress
//---------------------------------------------------------------------------
bool ButtonIO::isMissionInProgressThermo()
{
    ThermoStateType thermoState;
    if (ReadThermoStatus(portnum,&SNum[0],&thermoState,stdout))
    {
        InterpretStatus(&thermoState.MissStat);
        if(thermoState.MissStat.mission_in_progress == 1) {
            Log::write("ButtonIO::isMissionInProgressThermo: Mission is in progress.");
            return true;
        } else {
            Log::write("ButtonIO::isMissionInProgressThermo: No mission in progress.");
        }
    }
    return false;
}


//---------------------------------------------------------------------------
// This function will search for devices connected to the 1-Wire network
//
// 'portnum' - the port number of the port being used for the
//			   1-Wire Network.
//
// 'SNum'	 - the serial number of the iButton
//
//	Return:  'numDevice'
//---------------------------------------------------------------------------
int ButtonIO::selectDevice(int portnum, uchar *SNum)
{
	int i;
	int numDevice = 0;
	uchar AllDevices[8];

	usleep(2000);

	// Search for iButton devices connected to the 1-Wire network
	// 1  device found
	// 0  device not found
	// Look for temperature+humidity logger
	buttonType = 0;
	numDevice = FindDevices(portnum, &AllDevices, 0x41, 1);
	if(numDevice == 0) {
	    // Look for temperature logger
	    usleep(500);
	    numDevice = FindDevices(portnum, &AllDevices, 0x21, 1);
	    buttonType = 1;
	}

	if(numDevice == 0)
	{
		return 0;	// no device found
	}
	else
	{
		for(i=0;i<8;i++)	// write ID into array
		{
			SNum[i] = AllDevices[i];
		}
	}

	return numDevice;
}


//---------------------------------------------------------------------------
// This function will figure out, if there is an iButton connected to the
// reader or not.
//
// 'Device' - device number.
//
// Returns:  'true'		if a device is connected to the reader.
//			 'false'	if there is no device connected to the reader
//---------------------------------------------------------------------------
bool ButtonIO::isButtonConnected()
{
	usleep(5000);
	// check if USB port is open
	if(owUSB_is_port_open(portnum) == 0)
	{
		Log::writeError("ButtonIO::isButtonConnected: USB port is closed.");
		return false;
	}

	Device = 0;
	usleep(2000);

	// get device
	Device = ButtonIO::selectDevice(portnum,&SNum[0]);

	if(Device == 0)
	{
		return false;
	}
	else
	{
		return true;
	}
}


//---------------------------------------------------------------------------
// This function will open the USB port.
//
// 'port[]'  - Char array that contains zero terminated port name. For this
//			   platform use format 'DS2490-X' where X is the port number
//
// 'portnum' - number 0 to MAX_PORTNUM-1.  This number is provided to
//             indicate the symbolic port number.
//
// Returns:  'true' 	if open port succeed
//			 'false' 	if open port failed
//---------------------------------------------------------------------------
bool ButtonIO::openPort()
{
	char port1[] = "ds2490-1";
	char port2[] = "ds2490-0";

	usleep(5000);
	// check if USB port is already open
	if(owUSB_is_port_open(portnum) == 0)
	{
		usleep(5000);
		// Attempt to acquire the specified 1-Wire net
		if((portnum = owAcquireEx(port1)) < 0)
		{
			// OWERROR_DUMP(stdout);
			Log::writeError("ButtonIO::openPort: No reader connected to port ds2490-1.");
			usleep(5000);
			// if not successful in setting up the port with port name "port1", try next
			// port namme
			if((portnum = owAcquireEx(port2)) < 0)
			{
				// OWERROR_DUMP(stdout);
				Log::writeError("ButtonIO::openPort: No reader connected to port ds2490-0.");
				return false;
			}
			else
			{
				Log::write("ButtonIO::openPort: USB port (ds2490-0) is open.");
				// cout << "USB port (ds2490-0) is open!" << endl;
				return true;
			}
		}
		else
		{
			Log::write("ButtonIO::openPort: USB port (ds2490-1) is open.");
			// cout << "USB port (ds2490-1)  is open!" << endl;
			return true;
		}
	}
	else
	{
		Log::writeError("ButtonIO::openPort: Cannot open port, because port is already open.");
		// cout << "port is already opened!" << endl;
		return true;
	}
}


//---------------------------------------------------------------------------
// This Function will close the USB port.
//
// 'portnum'  - number 0 to MAX_PORTNUM-1.  This number is provided to
//              indicate the symbolic port number.
//
// returns:  'true'		if close port succeed
//			 'false'	if close port failed
//---------------------------------------------------------------------------
bool ButtonIO::closePort()
{
	usleep(5000);
	// check if USB port is closed.
	if(owUSB_is_port_open(portnum) == false)
	{
		Log::writeError("ButtonIO::closePort: Cannot close port, because port is already closed.");
		return false;
	}
	else	// if USB port is open, close the port.
	{
		usleep(2000);
		owRelease(portnum);		// release the port previously acquired a 1-Wire net.
		Log::write("ButtonIO::closePort: Port closed.");
		// cout << "port closed!" << endl;
		return true;
	}
}


//---------------------------------------------------------------------------
// This function to read the current temperatur.
//
// 'temp_result' - Double variable that contains the value of temperature
//				   measurement.
//
// Returns:  'true' 	if reading the temperature succeed
//			 'false' 	if reading the temperature failed
//---------------------------------------------------------------------------
bool ButtonIO::downloadCurrentTemperature()
{
	// check if USB port is open
	if(owUSB_is_port_open(portnum) == 0)
	{
		Log::writeError("ButtonIO::downloadCurrentTemperature: USB port is closed.");
		return false;
	}
	else if(!(owVerify(portnum, false)))	// check if the current iButton device is on 1-Wire net
	{
		Log::writeError("ButtonIO::downloadCurrentTemperature: The current iButton is not on the 1-Wire net.");
		return false;
	}

	// reading device
	if(readDevice(portnum,&SNum[0],&state[0],&config))
	{
		// temperature conversion
		if(!doTemperatureConvert(portnum,&SNum[0],&state[0]))
	    {
			// OWERROR_DUMP(stdout);
			Log::writeError("ButtonIO::downloadCurrentTemperature: Cannot do temperature conversion.");
			return false;
	    }
	    else
	    {
	    	ifstream tmplate;
	    	tmplate.open (MISSION_PARAMETER_FILEPATH, ifstream::in);

			// read "mission_start_parameter.txt"
    		if (tmplate.is_open())
    		{
    			getline (tmplate,line);
    		    input[0] = atoi(line.c_str());
    		    tmplate.close();
    		}
    		else
    		{
    			Log::writeError("ButtonIO::downloadCurrentTemperature: Cannot open mission parameter file.");
    			return false;
    		}

    		//check if useTemperatureCalibration was set to a valid number.
    		if(!((input[0]==0) || (input[0]==1)))
    		{
    			Log::writeError("mission parameters: Boolean setting has a wrong value.");
 				return false;
 			}

    		// temperature calibration [true, false]
    		config.useTemperatureCalibration =  input[0];

			if(config.useTemperatureCalibration)
	        {
				val = decodeTemperature(&state[12],2,false,config);
	            valsq = val*val;
	            error = config.tempCoeffA*valsq +
	            config.tempCoeffB*val + config.tempCoeffC;
	            val = val - error;
	            temp_result = val;
	            //cout << "\nThe current temperature is:  " << val << endl;
	        }
	        else
	        {
	        	val = decodeTemperature(&state[12],2,false,config);
	        	temp_result = val;
	        	//cout << "\nThe current temperature is:  " << val << endl;
	        }
			return true;
	    }
	}
	else
	{
		// OWERROR_DUMP(stdout);
		Log::writeError("iButton_read_Temp: Cannot read Device (CRC Error)");
		return false;
	}
}


//---------------------------------------------------------------------------
// This function will read the iButton ID number.
//
// 'SNum' 		- Unsigned char array that contains the raw ID number.
// 'iButton_ID' - String that contains the converted ID nubmer
//
// Returns:  'Button_ID' 	if function succeed.
//			 "failed" 		if function failed
//---------------------------------------------------------------------------
QString ButtonIO::getButtonID()
{
	char buffer[3];
	usleep(5000);

	// check if the USB port is open
	if(owUSB_is_port_open(portnum) == 0)
	{
		Log::writeError("ButtonIO::getButtonID: USB port is closed.");
		return "failed";
	}
	else if(!(owVerify(portnum, false)))	// check if the current iButton device is on 1-Wire net
	{
		Log::writeError("ButtonIO::getButtonID: The current iButton is not on the 1-Wire net.");
		return "failed";
	}

	// get device
	// ButtonIO::selectDevice(portnum,&SNum[0]);
	iButton_ID.clear();	//clear ButtonID string

	for(int t=7;t>=0;t--)
	{
		sprintf(buffer, "%02X", SNum[t]); // convert char into HEX format
		iButton_ID = iButton_ID + buffer[0] + buffer[1];
	}

	QString Button_ID = iButton_ID.c_str(); // convert string into QString

	return Button_ID;
}

bool ButtonIO::downloadDeviceTimeStamp()
{
    if(buttonType == 0)
    {
        return downloadDeviceTimeStampThHyg();
    }
    return downloadDeviceTimeStampThermo();
}


//---------------------------------------------------------------------------
// This function will read the iButton RC time and date.
//
// 'device_time[]' - Integer array that contains the current device time
//					 device_time[0] -> hours
//					 device_time[1] -> minutes
//					 device_time[2] -> seconds
//
// 'device_date[]' - Integer array that contains the current device date
//					 device_date[0] -> day
//					 device_date[1] -> month
//					 device_date[2] -> year
//
// Returns:  'true' 	if reading device time succeed
//			 'false' 	if reading device time failed.
//---------------------------------------------------------------------------
bool ButtonIO::downloadDeviceTimeStampThHyg()
{
	hour = 0; min = 0;
	sec = 0; pm = 0;
	year = 0; month = 0;
	day = 0;

	for(int s=0;s<3;s++)
	{
		device_date[s] = 0;
	}
	for(int s=0;s<3;s++)
	{
		device_time[s] = 0;
	}

	usleep(5000);
	// check if USB port is open
	if(owUSB_is_port_open(portnum) == 0)
	{
		Log::writeError("ButtonIO::downloadDeviceTimeStamp: USB port is closed.");
		return false;
	}
	else if(!(owVerify(portnum, false)))	// check if the current iButton device is on 1-Wire net
	{
		Log::writeError("ButtonIO::downloadDeviceTimeStamp: The current iButton is not on the 1-Wire net.");
		return false;
	}

	usleep(2000);
	// read the register contents
	if(!readDevice(portnum,SNum,&state[0],&config))
	{
		Log::writeError("ButtonIO::downloadDeviceTimeStamp: Cannot read device");
		return false;
	}

	// current system time, used for time shift calculation!
	sysDateTime = QDateTime::currentDateTime();

	// get seconds
	lower = state[RTC_TIME&0x3F];
	upper = ((lower >> 4) & 0x07);
	lower = (lower & 0x0f);
	sec   = (int) lower + (int) upper * 10;

	// get minutes
	lower = state [(RTC_TIME&0x3F) + 1];
	upper = ((lower >> 4) & 0x07);
	lower = (lower & 0x0f);
	min   = (int) lower + (int) upper * 10;

	// get hours
	lower = state[(RTC_TIME&0x3F) + 2];
	upper = ((lower >> 4) & 0x07);
	lower = (lower & 0x0f);
	if ((upper&0x04) != 0)
	{
		// extract the AM/PM byte (PM is indicated by a 1)
		if((upper&0x02)>0)
			pm = 12;

		// isolate the 10s place
		upper &= 0x01;
	}
	hour = (int)(upper*10) + (int)lower + (int)pm;

	device_time[0] = hour;
	device_time[1] = min;
	device_time[2] = sec;

	// get day
    lower  = state[RTC_DATE&0x3F];
    upper  = ((lower >> 4) & 0x0f);
    lower  = (lower & 0x0f);
    day    = upper*10 + lower;

    // get month
    lower = state[(RTC_DATE&0x3F) + 1];
    upper = ((lower >> 4) & 0x01);
    lower = (lower & 0x0f);
    month = upper*10 + lower;

    // get year
    if((state[(RTC_DATE&0x3F) + 1]&0x80)==0x80)
       year = 100;
    lower = state[(RTC_DATE&0x3F) + 2];
    upper = ((lower >> 4) & 0x0f);
    lower = (lower & 0x0f);
    year  = upper*10 + lower + year + 2000;

    device_date[0] = day;
   	device_date[1] = month;
   	device_date[2] = year;

	return true;
}

bool ButtonIO::downloadDeviceTimeStampThermo() {

    ThermoStateType thermoState;

    usleep(5000);
    // check if USB port is open
    if(owUSB_is_port_open(portnum) == 0)
    {
        Log::writeError("ButtonIO::downloadDeviceTimeStampThermo: USB port is closed.");
        return false;
    }
    else if(!(owVerify(portnum, false)))    // check if the current iButton device is on 1-Wire net
    {
        Log::writeError("ButtonIO::downloadDeviceTimeStampThermo: The current iButton is not on the 1-Wire net.");
        return false;
    }

    usleep(2000);
    // read the register contents
    if(!readDevice(portnum,SNum,&state[0],&config))
    {
        Log::writeError("ButtonIO::downloadDeviceTimeStampThermo: Cannot read device");
        return false;
    }

    if(!ReadThermoStatus(portnum, &SNum[0], &thermoState, stdout))
    {
        return false;
    }

    InterpretStatus(&thermoState.MissStat);
    QDateTime rtc = QDateTime::fromTime_t(thermoState.MissStat.current_time);

    device_time[0] = rtc.toString("h").toInt();
    device_time[1] = rtc.toString("m").toInt();
    device_time[2] = rtc.toString("s").toInt();

    device_date[0] = rtc.toString("d").toInt();
    device_date[1] = rtc.toString("M").toInt();
    device_date[2] = rtc.toString("yyyy").toInt();

    return true;

}


//---------------------------------------------------------------------------
// This function will read the local system timestamp.
//
// 'local_time[]' - Integer array that contains the current local time
//					local_time[0] -> hours
//					local_time[1] -> minutes
//					local_time[2] -> seconds
//
// 'local_date[]' - Integer array that contains the current device date
//					local_date[0] -> day
//					local_date[1] -> month
//					local_date[2] -> year
//
// Returns:	'dateTime'		system timestamp
//							format: "day.month.year hour:min:sec"
//---------------------------------------------------------------------------
QString ButtonIO::getLocalTimeStamp()
{
	for(int s=0;s<3;s++)
	{
		local_time[s] = 0;
	}
	for(int s=0;s<3;s++)
	{
		local_date[s] = 0;
	}

	QDateTime actTime;
	actTime = actTime.currentDateTime();
	return actTime.toString("dd.MM.yyyy hh:mm:ss");
}


//---------------------------------------------------------------------------
// This function will calculate the time shift from system timestamp to
// the iButton RTC timestamp.
// Reading the sysDateTime is done in function 'iButtonUpdateDeviceTimestamp()'.
//
// Do always call function 'iButtonUpdateDeviceTimestamp()' first!
//
// Returns:  'timeShift'	time shift in seconds.
//---------------------------------------------------------------------------
int ButtonIO::getTimeShift()
{
	int timeShift = 0;

	QDateTime iButtonRTC;
    iButtonRTC = iButtonRTC.fromString(ButtonIO::getDeviceTimeStamp(), "dd.MM.yyyy hh:mm:ss");

    //calculate time shift in seconds
    //if the iButtonRTC datetime is earlier than sysDateTime, the value
    //returned is negative.
    timeShift = sysDateTime.secsTo(iButtonRTC);

	return timeShift;
}


//---------------------------------------------------------------------------
// This function will return the current iButton RC timestamp.
//
// 'device_time[]' - Integer array that contains the current local time
//					 local_time[0] -> hours
//					 local_time[1] -> minutes
//					 local_time[2] -> seconds
//
// 'device_date[]' - Integer array that contains the current device date
//					 device_date[0] -> day
//					 device_date[1] -> month
//					 device_date[2] -> year
//
// Returns:  'dateTime'		iButton RC timestamp
//							format: "day.month.year hour:min:sec"
//---------------------------------------------------------------------------
QString ButtonIO::getDeviceTimeStamp()
{
	QString dateTime = QString::number(device_date[0], 10).rightJustified(2, '0') + QString(".")
					  +QString::number(device_date[1], 10).rightJustified(2, '0') + QString(".")
					  +QString::number(device_date[2], 10) + QString(" ")
					  +QString::number(device_time[0], 10).rightJustified(2, '0') + QString(":")
					  +QString::number(device_time[1], 10).rightJustified(2, '0') + QString(":")
					  +QString::number(device_time[2], 10).rightJustified(2, '0');
	return dateTime;
}


//---------------------------------------------------------------------------
// This function will return the time when the iButton starts sampling.
// Do not call this function before started a mission.
//
// 'mission_start_timestamp' - Integer array that contains the mission start
//							   timestamp.
//							   mission_start_timestamp[0] -> day
//							   mission_start_timestamp[1] -> month
//							   mission_start_timestamp[2] -> year
//							   mission_start_timestamp[3] -> hours
//							   mission_start_timestamp[4] -> minutes
//							   mission_start_timestamp[5] -> seconds
//
// Returns:  'dateTime'		timestamp when Button starts logging data
//							format: "day.month.year hour:min:sec"
//---------------------------------------------------------------------------
QString ButtonIO::getSamplingStartTime()
{
   	return this->samplingStartTime.toString("dd.MM.yyyy hh:mm:ss");
}

//---------------------------------------------------------------------------
// This function will read the sampled data form the iButton.
//
// 'sample_Array' - Name of Array that contains all the sampled data
//                  sample_Array[0] -> first measured data
//                  sample_Array[n] -> nth measured data
// 'sample_No'    - Integer Variable that contains the number of stored samples.
//
// Returns:  'true'     if reading data succeed
//           'false'    if reading data failed.
//---------------------------------------------------------------------------
bool ButtonIO::downloadMissionData()
{
    usleep(5000);
    // check if USB port is open
    if(owUSB_is_port_open(portnum) == 0)
    {
        Log::writeError("ButtonIO::downloadMissionData: USB port is closed.");
        return false;
    }
    else if(!(owVerify(portnum, false)))    // check if the current iButton device is on 1-Wire net
    {
        Log::writeError("ButtonIO::downloadMissionData: The current iButton is not on the 1-Wire net.");
        return false;
    }

    if(buttonType == 0)
    {
        return downloadMissionDataThHyg();
    }
    return downloadMissionDataThermo();
}

//---------------------------------------------------------------------------
// This function will read the sampled data form the iButton.
//
// 'sample_Array' - Name of Array that contains all the sampled data
//					sample_Array[0] -> first measured data
//					sample_Array[n] -> nth measured data
// 'sample_No' 	  - Integer Variable that contains the number of stored samples.
//
// Returns:  'true'		if reading data succeed
//			 'false'	if reading data failed.
//---------------------------------------------------------------------------
bool ButtonIO::downloadMissionDataThHyg()
{
    uchar page[8192];
	int i = 0;
	int tempone,temptwo,tempthr;
	int tempBytes = 0;
	int dataBytes = 0;
	int maxSamples = 0;
	int wrapCount = 0;
	sample_No = 0;

	// enable high speed data download.
	if(!owOverdriveAccess(portnum))
	{
		//OWERROR(OWERROR_DEVICE_SELECT_FAIL);
		Log::writeError("ButtonIO::downloadMissionData: Cannot perform overdrive speed data download");
	}

	usleep(2000);
	// read the register content
	if(!readDevice(portnum,SNum,&state[0],&config))
	{
		Log::writeError("ButtonIO::downloadMissionData: Cannot read device.");
		return false;
	}


	ifstream tmplate;
	tmplate.open (MISSION_PARAMETER_FILEPATH, ifstream::in);

	// read "mission_start_parameter.txt"
	if (tmplate.is_open())
	{
		getline (tmplate,line);
	    input[0] = atoi(line.c_str());
	    tmplate.close();
	}
	else
	{
		Log::writeError("ButtonIO::downloadMissionData: Cannot open mission parameter file.");
		return false;
	}

	// check if useTemperatureCalbration was set to valid number
	if(!((input[0]==0) || (input[0]==1)))
	{
		Log::writeError("mission parameters: Boolean setting has a wrong value.");
		return false;
	}

	// temperature calibration [true, false]
	config.useTemperatureCalibration =  input[0];
	config.useHumidityCalibration = false;			// always false


	// get the number of samples
	tempone = (int)(state[32]&0x0000FF);
	temptwo = (int)(state[33]<<8)&0x00FF00;
	tempthr = (int)(state[34]<<16)&0xFF0000;
	sample_No = tempone + temptwo + tempthr;
	//cout << sample_No << endl;

	//create dynamic array
	if(sample_Array != NULL)
	{
	    delete sample_Array;
	}
	sample_Array = new double[sample_No];

	if(sample_No == 0)
	{
		Log::write("ButtonIO::downloadMissionData: No samples yet for this mission.");
		owSpeed(portnum,MODE_NORMAL);
		return true;
	}

	usleep(2000);
	// figure out how many bytes for each temperature sample
	// if it's being logged, add 1 to the size
	if(getFlag(portnum,SNum,MISSION_CONTROL_REGISTER,MCR_BIT_ENABLE_TEMPERATURE_LOGGING))
	{
		tempBytes += 1;
		// if it's 16-bit resolution, add another 1 to the size
		if(getFlag(portnum,SNum,MISSION_CONTROL_REGISTER,MCR_BIT_TEMPERATURE_RESOLUTION))
		{
			//cout<<"16 Bit resolution"<< endl;
			tempBytes += 1;
		}
	}
	//cout << "tempBytes: " << tempBytes << endl;

	usleep(2000);
	// figure out how many bytes for each data sample
	// if it's being logged, add 1 to the size
	if(getFlag(portnum,SNum,MISSION_CONTROL_REGISTER,MCR_BIT_ENABLE_DATA_LOGGING))
	{
		dataBytes += 1;

		// if it's 16-bit resolution, add another 1 to the size
		if(getFlag(portnum,SNum,MISSION_CONTROL_REGISTER,MCR_BIT_DATA_RESOLUTION))
		{
			dataBytes += 1;
		}
	}
	//cout << "dataBytes: " << dataBytes << endl;

	//cout << "tempBytes + dataBytes = " << (tempBytes + dataBytes) << endl;
	// figure max number of samples
	switch(tempBytes + dataBytes)
	{
		case 1:
			maxSamples = 8192;
			//cout << "max No of samples: " << maxSamples << endl;
			break;
		case 2:
			maxSamples = 4096;
			//cout << "max No of samples: " << maxSamples << endl;
			break;
		case 3:
			maxSamples = 2560;
			//cout << "max No of samples: " << maxSamples << endl;
	        break;
		case 4:
			maxSamples = 2048;
			//cout << "max No of samples: " << maxSamples << endl;
			break;
		default:
		case 0:
			Log::writeError("ButtonIO::downloadMissionData: Figuring out max number of samples failed.");
			// assert! should never, ever get here
			break;
	}

	usleep(2000);
	if(getFlag(portnum,SNum,MISSION_CONTROL_REGISTER,MCR_BIT_ENABLE_ROLLOVER)
       && (sample_No>maxSamples))// intentional assignment
	{
		wrapCount = sample_No % maxSamples;
		//cout << "enable rollover?" << endl;
	}


    //DEBUG: For bad SOICS
	if(!getFlag(portnum,SNum,MISSION_CONTROL_REGISTER,MCR_BIT_ENABLE_ROLLOVER)
       && (sample_No>maxSamples))
	{
		Log::writeError("ButtonIO::downloadMissionData: (device error) Rollover was not enabled, but it did occur.");
		return false;
	}

	if(sample_No>maxSamples) {
	    sample_No = maxSamples;
	}


	if((tempBytes*sample_No)>0)
	{
		// cout << "for loop until: " << ((tempBytes*sampleCnt/32)+((tempBytes*sampleCnt%32)>0?1:0)) << endl;

		for(i=0;i<((tempBytes*sample_No/32)+((tempBytes*sample_No%32)>0?1:0));i++)
		{
			if(!readPageCRCEE77(2,portnum,SNum,i+128,&page[i*32]))
			{
				return false;
				Log::writeError("ButtonIO::downloadMissionData: Read page wasn't completed.");
			}
		}

		for(i=0;i<sample_No;i++)
		{
			val = decodeTemperature(&page[(wrapCount+i*tempBytes)%(maxSamples*tempBytes)],
				  tempBytes,true,config);

			if(config.useTemperatureCalibration)
			{
				valsq = val*val;
				error = config.tempCoeffA*valsq + config.tempCoeffB*val + config.tempCoeffC;
				val = val - error;

				sample_Array[i] = val;
			}
			else
			{
				sample_Array[i] = val;
			}
		}
	}

	Log::write("Reading mission data completed.");
	owSpeed(portnum,MODE_NORMAL);
	return true;
}


//---------------------------------------------------------------------------
// This function will read the sampled data form the iButton.
//
// 'sample_Array' - Name of Array that contains all the sampled data
//                  sample_Array[0] -> first measured data
//                  sample_Array[n] -> nth measured data
// 'sample_No'    - Integer Variable that contains the number of stored samples.
//
// Returns:  'true'     if reading data succeed
//           'false'    if reading data failed.
//---------------------------------------------------------------------------
bool ButtonIO::downloadMissionDataThermo()
{
    ThermoStateType thermoState;

    if (DownloadThermo(portnum,&SNum[0],&thermoState,stdout))
    {
        // interpret the results of the download
        InterpretStatus(&thermoState.MissStat);
        InterpretLog(&thermoState.LogData, &thermoState.MissStat);

        sample_No = thermoState.LogData.num_log;

        //create dynamic array
        if(sample_Array != NULL)
        {
            delete sample_Array;
        }
        sample_Array = new double[sample_No];

        for(int i = 0; i < thermoState.LogData.num_log; i++)
        {
            sample_Array[i] = thermoState.LogData.temp[i];
        }

    }
    else
    {
        return false;
    }

    Log::write("6");
    return true;
}


//---------------------------------------------------------------------------
// Function that returns the number of samples
//---------------------------------------------------------------------------
int ButtonIO::getSampleSize()
{
	return sample_No;
}


//---------------------------------------------------------------------------
// Function that returns a pointer to the sample array
//---------------------------------------------------------------------------
double *ButtonIO::getSampleArray()
{
	return sample_Array;
}
