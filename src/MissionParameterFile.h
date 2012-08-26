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
#ifndef MISSIONPARAMETERFILE_H_
#define MISSIONPARAMETERFILE_H_

#include <iostream>
#include <QFile>
#include <QTextStream>
#include "Log.h"

using namespace std;

// Location of mission parameter file
#define MISSION_PARAMETER_FILE  "./mission_start_parameter.txt"

class MissionParameterFile {
public:

    // Singleton
    static MissionParameterFile& Instance()
    {
        static MissionParameterFile instance;
        return instance; // always returns the same instance
    }

    bool getEnableAutoTempCalib() { return configEnableAutoTempCalib; }

    bool setEnableAutoTempCalib(bool enableAutoTempCalib) { return checkAndSetValue(EN_AUTO_TEMP_CALIB, enableAutoTempCalib); }

    int getSamplingRate() { return configSamplingRate; }

    bool setSamplingRate(int samplingRate) { return checkAndSetValue(SAMPLING_RATE, samplingRate); }

    bool getEnableRollover() { return configEnableRollover; }

    bool setEnableRollover(bool enableRollover) { return checkAndSetValue(EN_ROLLOVER, enableRollover); }

    bool getHighTemperatureResolution() { return configHighTemperatureResolution; }

    bool setHighTemperatureResolution(bool highTemperatureResolution) { return checkAndSetValue(HIGH_TEMP_RES, highTemperatureResolution); }

    bool getSetMissionStartTime() { return configSetMissionStartTime; }

    bool setSetMissionStartTime(bool enableMissionStartTime) { return checkAndSetValue(SET_MISSION_START_TIME, enableMissionStartTime); }

    long getMissionStartTime() { return configMissionStartTime; }

    bool setMissionStartTime(long missionStartTime) { return checkAndSetValue(MISSION_START_TIME, missionStartTime); }

    bool loadMissionParameters();

    bool storeMissionParameters();

private:
    MissionParameterFile();
    MissionParameterFile(const MissionParameterFile &old); // disallow copy constructor
    const MissionParameterFile &operator=(const MissionParameterFile &old); //disallow assignment operator
    ~MissionParameterFile();

    bool configEnableAutoTempCalib;

    int configSamplingRate;

    bool configEnableRollover;

    bool configHighTemperatureResolution;

    bool configSetMissionStartTime;

    unsigned int configMissionStartTime;

    bool checkAndSetValue(int index, unsigned int value);

    unsigned int getValue(int index);

    enum configLines { SAMPLING_RATE, EN_ROLLOVER, HIGH_TEMP_RES, SET_MISSION_START_TIME, MISSION_START_TIME, EN_AUTO_TEMP_CALIB, CONFIG_END };

    QStringList fieldDescriptions; //!> List containing all descriptions of the different values that can be set.

};

#endif /* MISSIONPARAMETERFILE_H_ */
