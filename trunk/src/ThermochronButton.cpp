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

#include "ThermochronButton.h"

ThermochronButton::ThermochronButton() {}

ThermochronButton::~ThermochronButton() {}

bool ThermochronButton::startButtonMission(int portnum, uchar* SNum)
{
    ThermoStateType thermoState;
    MissionParameterFile& mp = MissionParameterFile::Instance();
    int retry = 5;

    usleep(5000);

    while(retry > 0)
    {
        retry--;

        usleep(1500);

        // check if the USB port is open
        if(owUSB_is_port_open(portnum) == 0)
        {
            Log::writeError("ThermochronButton::startButtonMission: USB port is closed.");
            return "failed";
        }
        else if(!(owVerify(portnum, false)))    // check if the current iButton device is on 1-Wire net
        {
            Log::writeError("ThermochronButton::startButtonMission: The current iButton is not on the 1-Wire net.");
            return "failed";
        }

        if (ReadThermoStatus(portnum,&SNum[0],&thermoState,stdout))
        {
            InterpretStatus(&thermoState.MissStat);

            // set mission parameter values from "mission_start_parameter" file
            thermoState.MissStat.rollover_enable = mp.getEnableRollover();
            double samplingRate = (double)mp.getSamplingRate()/60.0;
            if(samplingRate-(int)samplingRate>0) {
                samplingRate = samplingRate + 1; // increment to next full minute
            }
            thermoState.MissStat.sample_rate = (int)(samplingRate);
            thermoState.MissStat.high_threshold = 220;
            thermoState.MissStat.low_threshold = 0;

            thermoState.MissStat.start_delay = 0;
            // Calculate Delay
            if(mp.getSetMissionStartTime()) {
                QDateTime now = QDateTime::currentDateTime();
                double secsDelay = now.secsTo(QDateTime::fromTime_t(mp.getMissionStartTime()));
                if((secsDelay - (int)secsDelay)>0.5)
                {
                    thermoState.MissStat.start_delay = ((int) secsDelay) + 1;
                }
                else
                {
                    thermoState.MissStat.start_delay = (int) secsDelay;
                }
            }

            if(!MissionThermo(portnum,&SNum[0],&thermoState,stdout))
            {
                Log::writeError("ThermochronButton::startButtonMission: Cannot start mission.");
            }
            else
            {
                Log::write("ThermochronButton::startButtonMission: Mission started.");
                return true;
            }
        }
    }

    return false;
}

bool ThermochronButton::stopButtonMission(int portnum, uchar* SNum)
{
    return true;
}

bool ThermochronButton::isMissionInProgress(int portnum, uchar* SNum)
{
    ThermoStateType thermoState;
    usleep(5000);
    // check if port is opened
    if(owUSB_is_port_open(portnum) == 0)
    {
        Log::writeError("ThermochronButton::isMissionInProgress: USB port is closed.");
        return false;
    }
    else if(!(owVerify(portnum, false)))    // check if the current iButton device is on 1-Wire net
    {
        Log::writeError("ThermochronButton::isMissionInProgress: The current iButton is not on the 1-Wire net.");
        return false;
    }

    if (ReadThermoStatus(portnum,&SNum[0],&thermoState,stdout))
    {
        InterpretStatus(&thermoState.MissStat);
        if(thermoState.MissStat.mission_in_progress == 1) {
            Log::write("ThermochronButton::isMissionInProgress: Mission is in progress.");
            return true;
        } else {
            Log::write("ThermochronButton::isMissionInProgress: No mission in progress.");
        }
    }
    else
    {
        Log::write("ThermochronButton::isMissionInProgress: Problem while reading button status.");
    }

    return false;
}

bool ThermochronButton::getButtonTime(int portnum, uchar* SNum, QDateTime& buttonTime, QDateTime& hostTime)
{
    ThermoStateType thermoState;

    usleep(5000);
    // check if USB port is open
    if(owUSB_is_port_open(portnum) == 0)
    {
        Log::writeError("ThermochronButton::getButtonTime: USB port is closed.");
        return false;
    }
    else if(!(owVerify(portnum, false)))    // check if the current iButton device is on 1-Wire net
    {
        Log::writeError("ThermochronButton::getButtonTime: The current iButton is not on the 1-Wire net.");
        return false;
    }

    usleep(2000);
    hostTime = QDateTime::currentDateTime();

    if(!ReadThermoStatus(portnum, &SNum[0], &thermoState, stdout))
    {
        return false;
    }

    InterpretStatus(&thermoState.MissStat);
    buttonTime = QDateTime::fromTime_t(thermoState.MissStat.current_time);

    Log::write("Device time: " + buttonTime.toString());

    return true;
}

bool ThermochronButton::downloadMissionData(int portnum, uchar* SNum, int& numberOfSamples, double* samples)
{
    ThermoStateType thermoState;

    usleep(5000);
    // check if USB port is open
    if(owUSB_is_port_open(portnum) == 0)
    {
        Log::writeError("ThermochronButton::downloadMissionData: USB port is closed.");
        return false;
    }
    else if(!(owVerify(portnum, false)))    // check if the current iButton device is on 1-Wire net
    {
        Log::writeError("ThermochronButton::downloadMissionData: The current iButton is not on the 1-Wire net.");
        return false;
    }

    if (DownloadThermo(portnum,&SNum[0],&thermoState,stdout))
    {
        // interpret the results of the download
        InterpretStatus(&thermoState.MissStat);
        InterpretLog(&thermoState.LogData, &thermoState.MissStat);

        numberOfSamples = thermoState.LogData.num_log;
        samples = new double[numberOfSamples];

        for(int i = 0; i < thermoState.LogData.num_log; i++)
        {
            samples[i] = thermoState.LogData.temp[i];
        }

    }
    else
    {
        Log::writeError("ThermochronButton::downloadMissionData: Error while downloading mission data.");
        return false;
    }

    return true;
}
