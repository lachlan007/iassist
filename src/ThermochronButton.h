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
#ifndef THERMOCHRONBUTTON_H_
#define THERMOCHRONBUTTON_H_

#include "Log.h"
#include "MissionParameterFile.h"
#include "buttonIO/ownet.h"
#include "buttonIO/findtype.h"
#include "buttonIO/thermo21.h"

using namespace std;

class ThermochronButton {
public:
    ThermochronButton();
    virtual ~ThermochronButton();

    static bool startButtonMission(int portnum, uchar* SNum);

    static bool stopButtonMission(int portnum, uchar* SNum);

    static bool isMissionInProgress(int portnum, uchar* SNum);

    static bool getButtonTime(int portnum, uchar* SNum, QDateTime& buttonTime, QDateTime& hostTime);

    static bool downloadMissionData(int portnum, uchar* SNum, int& numSamples, double*& samples);

    static bool getMissionStartTime(int portnum, uchar* SNum, QDateTime& missionStartTime);

};

#endif /* THERMOCHRONBUTTON_H_ */
