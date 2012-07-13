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
	portnum = -1;
}

ButtonIO::~ButtonIO()
{
}


//unsigned char ButtonIO::getRegisterContent(int Address)
//{
	//return readByte(portnum,SNum,Address);
//}

int ButtonIO::findDevice(uchar *SNum, int deviceFamily)
{
	int numDevices = 0;
	uchar AllDevices[8];

	usleep(2000);

	// 0x41, 0x21
	// Search for iButton devices connected to the 1-Wire network
	// 1  device found
	// 0  device not found
	numDevices = FindDevices(portnum, &AllDevices, deviceFamily, 1);

	if(numDevices == 0)
	{
		return 0; // no device found
	}
	else
	{
		for(int i=0; i<8; i++) // write ID into array
		{
			SNum[i] = AllDevices[i];
		}
	}

	return numDevices;
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

bool ButtonIO::closePort()
{
	usleep(5000);
	// check if USB port is closed.
	if(owUSB_is_port_open(portnum) == false)
	{
		Log::writeError("ButtonIO::closePort: Cannot close port, because port is already closed.");
		portnum = -1;
		return false;
	}
	else	// if USB port is open, close the port.
	{
		usleep(2000);
		owRelease(portnum);		// release the port previously acquired a 1-Wire net.
		Log::write("ButtonIO::closePort: Port closed.");
		portnum = -1;
		// cout << "port closed!" << endl;
		return true;
	}
}

bool ButtonIO::downloadMissionData(uchar *SNum, MissionData &data)
{

    if(portnum < 0)
    {
        Log::writeError("ButtonIO::downloadMissionData: USB port is closed.");
        return false;
    }

    if(SNum[0] == FAMILY_THERMOHYG)
    {
        if(!ThermoHygrochronButton::downloadMissionData(portnum, &SNum[0], data.numSamples, data.samples))
        {
            return false;
        }
        if(!ThermoHygrochronButton::getButtonTime(portnum, &SNum[0], data.collectTimeButton, data.collectTimeHost))
        {
            return false;
        }
    }
    else if(SNum[0] == FAMILY_THERMO)
    {
        if(!ThermochronButton::downloadMissionData(portnum, &SNum[0], data.numSamples, data.samples))
        {
            return false;
        }
        if(!ThermochronButton::getButtonTime(portnum, &SNum[0], data.collectTimeButton, data.collectTimeHost))
        {
            return false;
        }
    }
    else
    {
        Log::writeError("ButtonIO::downloadMissionData: Unsupported button family.");
        return false;
    }

    return true;
}

bool ButtonIO::startMission(uchar *SNum)
{
    if(portnum < 0)
    {
        Log::writeError("ButtonIO::startMission: USB port is closed.");
        return false;
    }

    if(SNum[0] == FAMILY_THERMOHYG)
    {
        return ThermoHygrochronButton::startButtonMission(portnum, &SNum[0]);
    }
    else if(SNum[0] == FAMILY_THERMO)
    {
        return ThermochronButton::startButtonMission(portnum, &SNum[0]);
    }
    else
    {
        Log::writeError("ButtonIO::startMission: Unsupported button family.");
    }

    return false;
}

bool ButtonIO::stopMission(uchar *SNum)
{
    if(portnum < 0)
    {
        Log::writeError("ButtonIO::stopMission: USB port is closed.");
        return false;
    }

    if(SNum[0] == FAMILY_THERMOHYG)
    {
        return ThermoHygrochronButton::stopButtonMission(portnum, &SNum[0]);
    }
    else if(SNum[0] == FAMILY_THERMO)
    {
        return ThermochronButton::stopButtonMission(portnum, &SNum[0]);
    }
    else
    {
        Log::writeError("ButtonIO::stopMission: Unsupported button family.");
    }

    return false;
}

bool ButtonIO::getConnectedButton(uchar *SNum)
{
    int numDevices = 0;
    usleep(5000);
    // check if USB port is open
    if(owUSB_is_port_open(portnum) == 0)
    {
        Log::writeError("ButtonIO::isButtonConnected: USB port is closed.");
        return false;
    }

    usleep(2000);

    // Try ThermoHygrochron first
    numDevices = findDevice(&SNum[0], FAMILY_THERMOHYG);
    if(numDevices != 1)
    {
        // Alternatively try Thermochron
        numDevices = findDevice(&SNum[0], FAMILY_THERMO);
    }

    if(numDevices != 1)
    {
        return false;
    }
    return true;
}

bool ButtonIO::isMissionInProgress(uchar *SNum)
{
    if(portnum < 0)
    {
        Log::writeError("ButtonIO::isMissionInProgress: USB port is closed.");
        return false;
    }

    Log::write(ButtonIO::buttonIDStr(&SNum[0]));

    if(SNum[0] == FAMILY_THERMOHYG)
    {
        return ThermoHygrochronButton::isMissionInProgress(portnum, &SNum[0]);
    }
    else if(SNum[0] == FAMILY_THERMO)
    {
        return ThermochronButton::isMissionInProgress(portnum, &SNum[0]);
    }
    else
    {
        Log::writeError("ButtonIO::isMissionInProgress: Unsupported button family.");
    }

    return false;
}

QString ButtonIO::buttonIDStr(uchar *SNum) {
    QString buttonID = QString("");
    for(int i=7; i >= 0; i--)
    {
        buttonID += QString("%1").arg(SNum[i], 0, 16);
    }
    return buttonID.toUpper();
}

bool ButtonIO::isThermoHygrochron(uchar *SNum)
{
    if(SNum[0] == FAMILY_THERMOHYG)
    {
        return true;
    }

    return false;
}

