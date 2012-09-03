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
#include "MissionData.h"

MissionData::MissionData()
{
    samples = 0;
    numSamples = 0;
    collectTimeHost = QDateTime();
    collectTimeButton = QDateTime();
    missionStartTime = QDateTime();
}

MissionData::~MissionData()
{
    if(samples) {
        delete samples;
    }
}

void MissionData::clear()
{
    if(samples) {
        delete samples;
    }
    samples = 0;
    numSamples = 0;
    collectTimeHost = QDateTime();
    collectTimeButton = QDateTime();
    missionStartTime = QDateTime();
}

