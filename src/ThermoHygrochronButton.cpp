//---------------------------------------------------------------------------
//  iAssist - Rapid deployment of Maxim iButton(c) miniature logger devices
//  Copyright (C) 2012 ETH Zurich, Computer Engineering and Networks
//      Laboratory, Matthias Keller <kellmatt at tik.ee.ethz.ch>
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

#include "ThermoHygrochronButton.h"

ThermoHygrochronButton::ThermoHygrochronButton() {}

ThermoHygrochronButton::~ThermoHygrochronButton() {}

bool ThermoHygrochronButton::startButtonMission(int portnum, uchar* SNum)
{

    startMissionData settings;
    int retry = 5;
    configLog config;
    MissionParameterFile& mp = MissionParameterFile::Instance();

    // Set mission parameters
    settings.enableRollover = (short)mp.getEnableRollover();
    settings.timeSync = 1;
    settings.tempEnabled = 1;
    settings.dataEnabled = 0;
    settings.highDataResolution = 0;
    settings.highTempAlarm = 0;
    settings.lowTempAlarm = 0;
    settings.highDataAlarm = 0;
    settings.lowDataAlarm = 0;
    settings.tempAlarm = 0;
    settings.highTempResolution = (short)mp.getHighTemperatureResolution();
    settings.sampleRate = (int)mp.getSamplingRate();

    settings.startDelay = 0;
    // Calculate Delay
    if(mp.getSetMissionStartTime()) {
        QDateTime now = QDateTime::currentDateTime();
        double secsDelay = static_cast<double>(mp.getMissionStartTime()-now.toTime_t())/60.0;
        if((secsDelay - (int)secsDelay)>0.5)
        {
            settings.startDelay = ((int) secsDelay) + 1;
        }
        else
        {
            settings.startDelay = (int) secsDelay;
        }
    }

    while(retry > 0)
    {
        retry--;

        usleep(1500);
        // check if the USB port is opened
        if(owUSB_is_port_open(portnum) == 0)
        {
            Log::writeError("ThermoHygrochronButton::startButtonMission: USB port is closed.");
            return false;
        }
        else if(!(owVerify(portnum, false)))    // check if the current iButton device is on 1-Wire net
        {
            Log::writeError("ThermoHygrochronButton::startButtonMission: The current iButton is not on the 1-Wire net.");
            continue;
        }

        usleep(2000);
        // start mission
        if(!startMission(portnum, &SNum[0], settings, &config))
        {
            Log::writeError("ThermoHygrochronButton::startButtonMission: Cannot start mission.");
        }
        else
        {
            Log::write("ThermoHygrochronButton::startButtonMission: Mission started.");
            return true;
        }
    }

    return false;
}

bool ThermoHygrochronButton::stopButtonMission(int portnum, uchar* SNum)
{
    int retry = 5;

    while(retry > 0)
    {
        retry--;

        usleep(1500);
        // check if port is opened
        if(owUSB_is_port_open(portnum) == 0)
        {
            Log::writeError("ThermoHygrochronButton::stopButtonMission: USB port is closed.");
            return false;
        }
        else if(!(owVerify(portnum, false)))    // check if the current iButton device is on 1-Wire net
        {
            Log::writeError("ThermoHygrochronButton::stopButtonMission: The current iButton is not on the 1-Wire net.");
            continue;
        }

        usleep(2000);
        // stops the currently running mission.
        if(!stopMission(portnum,&SNum[0]))
        {
            OWERROR_DUMP(stdout);
            Log::writeError("ThermoHygrochronButton::stopButtonMission: Cannot stop mission.");
        }
        else
        {
            Log::write("ThermoHygrochronButton::stopButtonMission: Mission stopped.");
            // cout << "Mission stopped" << endl;
            return true;
        }
    }

    return false;
}

bool ThermoHygrochronButton::isMissionInProgress(int portnum, uchar* SNum)
{

    usleep(5000);
    // check if port is opened
    if(owUSB_is_port_open(portnum) == 0)
    {
        Log::writeError("ThermoHygrochronButton::isMissionInProgress: USB port is closed.");
        return false;
    }
    else if(!(owVerify(portnum, false)))    // check if the current iButton device is on 1-Wire net
    {
        Log::writeError("ThermoHygrochronButton::isMissionInProgress: The current iButton is not on the 1-Wire net.");
        return false;
    }

    usleep(2000);
    // read device status register
    if (!getFlag(portnum, SNum, GENERAL_STATUS_REGISTER, GSR_BIT_MISSION_IN_PROGRESS))
    {
        // OWERROR(OWERROR_HYGRO_STOP_MISSION_UNNECESSARY);
        Log::write("ThermoHygrochronButton::isMissionInProgressThHyg: No mission in progress.");
        // cout << "No Mission in progress" << endl;
        return false;
    }
    else
    {
        Log::write("ThermoHygrochronButton::isMissionInProgressThHyg: Mission is in progress.");
        // cout << "Mission is in progress" << endl;
        return true;
    }
}

bool ThermoHygrochronButton::getButtonTime(int portnum, uchar* SNum, QDateTime& buttonTime, QDateTime& hostTime)
{

    QDateTime rtcTime;
    uchar upper, lower;
    int hour = 0, min = 0, sec = 0, pm = 0, year = 0, month = 0, day = 0;
    configLog config;
    uchar state[96];

    usleep(5000);
    // Check if USB port is open
    if(owUSB_is_port_open(portnum) == 0)
    {
        Log::writeError("ThermoHygrochronButton::getButtonTime: USB port is closed.");
        return false;
    }
    else if(!(owVerify(portnum, false))) // Check if the current iButton device is on 1-Wire net
    {
        Log::writeError("ThermoHygrochronButton::getButtonTime: The current iButton is not on the 1-Wire net.");
        return false;
    }

    usleep(2000);
    // Read the register contents
    if(!readDevice(portnum,SNum,&state[0],&config))
    {
        Log::writeError("ThermoHygrochronButton::getButtonTime: Cannot read device");
        return false;
    }

    // Current system time, used for time shift calculation!
    hostTime = QDateTime::currentDateTime();

    // Get seconds
    lower = state[RTC_TIME&0x3F];
    upper = ((lower >> 4) & 0x07);
    lower = (lower & 0x0f);
    sec   = (int) lower + (int) upper * 10;

    // Get minutes
    lower = state [(RTC_TIME&0x3F) + 1];
    upper = ((lower >> 4) & 0x07);
    lower = (lower & 0x0f);
    min   = (int) lower + (int) upper * 10;

    // Get hours
    lower = state[(RTC_TIME&0x3F) + 2];
    upper = ((lower >> 4) & 0x07);
    lower = (lower & 0x0f);
    if ((upper&0x04) != 0)
    {
        // Extract the AM/PM byte (PM is indicated by a 1)
        if((upper&0x02)>0)
        {
            pm = 12;
        }
        // Isolate the 10s place
        upper &= 0x01;
    }
    hour = (int)(upper*10) + (int)lower + (int)pm;

    // Get day
    lower  = state[RTC_DATE&0x3F];
    upper  = ((lower >> 4) & 0x0f);
    lower  = (lower & 0x0f);
    day    = upper*10 + lower;

    // Get month
    lower = state[(RTC_DATE&0x3F) + 1];
    upper = ((lower >> 4) & 0x01);
    lower = (lower & 0x0f);
    month = upper*10 + lower;

    // Get year
    if((state[(RTC_DATE&0x3F) + 1]&0x80)==0x80)
       year = 100;
    lower = state[(RTC_DATE&0x3F) + 2];
    upper = ((lower >> 4) & 0x0f);
    lower = (lower & 0x0f);
    year  = upper*10 + lower + year + 2000;

    QString dateTimeStr = QString::number(day) + "." + QString::number(month) + "." + QString::number(year)
        + " " + QString::number(hour) + "." + QString::number(min) + "." + QString::number(sec);
    buttonTime.fromString(dateTimeStr, "d.M.yyyy H:m:s");

    Log::write("Device time: " + buttonTime.toString());

    return true;
}

bool ThermoHygrochronButton::downloadMissionData(int portnum, uchar* SNum, int& numSamples, double*& samples)
{
     uchar page[8192];
     uchar state[96];
     configLog config;
     MissionParameterFile& mp = MissionParameterFile::Instance();
     int i = 0;
     int tempone, temptwo, tempthr;
     int tempBytes = 0;
     int dataBytes = 0;
     int maxSamples = 0;
     int wrapCount = 0;
     double val, valsq, error;

     usleep(5000);
     // check if USB port is open
     if(owUSB_is_port_open(portnum) == 0)
     {
         Log::writeError("ThermoHygrochronButton::downloadMissionData: USB port is closed.");
         return false;
     }
     else if(!(owVerify(portnum, false)))    // check if the current iButton device is on 1-Wire net
     {
         Log::writeError("ThermoHygrochronButton::downloadMissionData: The current iButton is not on the 1-Wire net.");
         return false;
     }

     // enable high speed data download.
     if(!owOverdriveAccess(portnum))
     {
         //OWERROR(OWERROR_DEVICE_SELECT_FAIL);
         Log::writeError("ThermoHygrochronButton::downloadMissionData: Cannot perform overdrive speed data download");
     }

     usleep(2000);
     // read the register content
     if(!readDevice(portnum,SNum,&state[0],&config))
     {
         Log::writeError("ThermoHygrochronButton::downloadMissionData: Cannot read device.");
         return false;
     }

     // temperature calibration [true, false]
     config.useTemperatureCalibration = mp.getEnableAutoTempCalib();
     config.useHumidityCalibration = false; // always false

     // get the number of samples
     tempone = (int)(state[32]&0x0000FF);
     temptwo = (int)(state[33]<<8)&0x00FF00;
     tempthr = (int)(state[34]<<16)&0xFF0000;
     numSamples = tempone + temptwo + tempthr;

     if(numSamples == 0)
     {
         Log::write("ThermoHygrochronButton::downloadMissionData: No samples yet for this mission.");
         owSpeed(portnum,MODE_NORMAL);
         return true;
     }

     samples = new double[numSamples];

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
             Log::writeError("ThermoHygrochronButton::downloadMissionData: Figuring out max number of samples failed.");
             // assert! should never, ever get here
             break;
     }

     usleep(2000);
     if(getFlag(portnum,SNum,MISSION_CONTROL_REGISTER,MCR_BIT_ENABLE_ROLLOVER)
        && (numSamples>maxSamples))// intentional assignment
     {
         wrapCount = numSamples % maxSamples;
         //cout << "enable rollover?" << endl;
     }


     //DEBUG: For bad SOICS
     if(!getFlag(portnum,SNum,MISSION_CONTROL_REGISTER,MCR_BIT_ENABLE_ROLLOVER)
        && (numSamples>maxSamples))
     {
         Log::writeError("ThermoHygrochronButton::downloadMissionData: (device error) Rollover was not enabled, but it did occur.");
         return false;
     }

     if(numSamples>maxSamples) {
         numSamples = maxSamples;
     }


     if((tempBytes*numSamples)>0)
     {
         // cout << "for loop until: " << ((tempBytes*sampleCnt/32)+((tempBytes*sampleCnt%32)>0?1:0)) << endl;

         for(i=0;i<((tempBytes*numSamples/32)+((tempBytes*numSamples%32)>0?1:0));i++)
         {
             if(!readPageCRCEE77(2,portnum,SNum,i+128,&page[i*32]))
             {
                 return false;
                 Log::writeError("ThermoHygrochronButton::downloadMissionData: Read page wasn't completed.");
             }
         }

         for(i=0;i<numSamples;i++)
         {
             val = decodeTemperature(&page[(wrapCount+i*tempBytes)%(maxSamples*tempBytes)],
                   tempBytes,true,config);

             if(config.useTemperatureCalibration)
             {
                 valsq = val*val;
                 error = config.tempCoeffA*valsq + config.tempCoeffB*val + config.tempCoeffC;
                 val = val - error;

                 samples[i] = val;
             }
             else
             {
                 samples[i] = val;
             }
         }
     }

     Log::write("ThermoHygrochronButton::downloadMissionData: Reading mission data completed.");
     owSpeed(portnum,MODE_NORMAL);
     return true;
}

