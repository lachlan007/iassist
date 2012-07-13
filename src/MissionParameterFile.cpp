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

#include "MissionParameterFile.h"

MissionParameterFile::MissionParameterFile()
{

    fieldDescriptions.insert(SAMPLING_RATE, "sampling rate [s]\t\t\t\t\t\t\t\t[1,982800]");
    fieldDescriptions.insert(EN_ROLLOVER, "enable rollover\t\t\t\t\t\t\t\t[0,1]");
    fieldDescriptions.insert(HIGH_TEMP_RES, "high temperature resolution\t\t\t\t\t\t[0,1]\t0: 8 Bit | 1: 16 Bit");
    fieldDescriptions.insert(SET_MISSION_START_TIME, "start mission at specified time\t\t\t\t\t\t\t[0,1]");
    fieldDescriptions.insert(MISSION_START_TIME, "mission start time\tunix timestamp");
    fieldDescriptions.insert(EN_AUTO_TEMP_CALIB, "automatic temperature calibration\t\t\t\t\t[0,1]");

    QStringList fieldDescriptions; //!> List containing all descriptions of the different values that can be set.
    configSamplingRate = 60;
    configMissionStartTime = QDateTime::currentDateTime().toTime_t();

}

MissionParameterFile::~MissionParameterFile()
{
}

bool MissionParameterFile::loadMissionParameters()
{

    // Create a file handle
    QFile file(MISSION_PARAMETER_FILE);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) // Open the file
    {
        Log::writeError("MissionParameterFile: Can't open mission parameter file.");
        return false;
    }

    QTextStream in(&file);
    int lineCount = 0;
    while (!in.atEnd())
    {
        QString line = in.readLine();
        if(!checkAndSetValue(lineCount, line.split("\t")[0].toInt()))
        {
            file.close();
            return false;
        }
        lineCount++;
    }
    file.close();

    return true;

}

bool MissionParameterFile::checkAndSetValue(int index, long value)
{
    switch(index)
    {
    case EN_AUTO_TEMP_CALIB:
        if(value == 1) {
            configEnableAutoTempCalib = true;
        } else if(value == 0) {
            configEnableAutoTempCalib = false;
        } else {
            // Value is out of range
            Log::writeError("MissionParameterFile: Auto calibration bit must be either 0 or 1.");
            return false;
        }
        break;
    case SAMPLING_RATE:
        if(value < 1 || value > 982800)
        {
            // Value is out of range
            Log::writeError("MissionParameterFile: Sampling rate is out of its limits.");
            return false;
        }
        configSamplingRate = (int)value;
        break;
    case EN_ROLLOVER:
        if(value == 1) {
            configEnableRollover = true;
        } else if(value == 0) {
            configEnableRollover = false;
        } else {
            // Value is out of range
            Log::writeError("MissionParameterFile: Rollover bit must be either 0 or 1.");
            return false;
        }
        break;
    case HIGH_TEMP_RES:
        if(value == 1) {
            configHighTemperatureResolution = true;
        } else if(value == 0) {
            configHighTemperatureResolution = false;
        } else {
            // Value is out of range
            Log::writeError("MissionParameterFile: Temperature resolution bit must be either 0 or 1.");
            return false;
        }
        break;
    case SET_MISSION_START_TIME:
            if(value == 1) {
                configSetMissionStartTime = true;
            } else if(value == 0) {
                configSetMissionStartTime = false;
            } else {
                // Value is out of range
                Log::writeError("MissionParameterFile: Enable mission start time bit must be either 0 or 1.");
                return false;
            }
            break;
    case MISSION_START_TIME:
        if(value < 1325397600 || value > 2524629600) // timestamp must be between 2012 and 2050
        {
            // Value is out of range
            Log::writeError("MissionParameterFile: Mission start time is out of its limits.");
            return false;
        }
        configMissionStartTime = value;
        break;
      }

    return true;

}

long MissionParameterFile::getValue(int index)
{
    int retVal = -1;

    switch(index)
    {
    case EN_AUTO_TEMP_CALIB:
        retVal = (long)configEnableAutoTempCalib;
        break;
    case SAMPLING_RATE:
        retVal = (long)configSamplingRate;
        break;
    case EN_ROLLOVER:
        retVal = (long)configEnableRollover;
        break;
    case HIGH_TEMP_RES:
        retVal = (long)configHighTemperatureResolution;
        break;
    case SET_MISSION_START_TIME:
        retVal = (long)configSetMissionStartTime;
        break;
    case MISSION_START_TIME:
        retVal = configMissionStartTime;
        break;
    }

    return retVal;
}

bool MissionParameterFile::storeMissionParameters()
{
    QFile file(MISSION_PARAMETER_FILE);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) // Open the file
    {
        Log::writeError("MissionParameterFile: Cannot open mission parameter file.");
        return false;
    }
    else
    {
        QTextStream out(&file);
        for(int lineCount=0; lineCount<CONFIG_END; lineCount++)
        {
            out << getValue(lineCount) << "\t" << fieldDescriptions[lineCount] << "\n";
        }
    }
    file.close();

    return true;
}


