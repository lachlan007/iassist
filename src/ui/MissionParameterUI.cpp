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
#include "MissionParameterUI.h"

#define RED	"background-color: rgb(255, 181, 181);" // red
#define YELLOW "background-color: rgb(255, 246, 207);" // yellow
#define GREEN "background-color: rgb(190, 255, 196);" // green

using namespace std;

MissionParameterUI::MissionParameterUI(QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);
	initDone = false;

	connect(ui.btnSave, SIGNAL(clicked()), this, SLOT(storeMissionParameters()));
	connect(ui.btnClose, SIGNAL(clicked()), this, SLOT(closeWindow()));
	connect(ui.pEnRollover, SIGNAL(clicked()), this, SLOT(setStatusChangesMade()));
	connect(ui.pSamplingRate, SIGNAL(valueChanged(int)), this, SLOT(setStatusChangesMade()));
	connect(ui.pEnHighTempRes, SIGNAL(clicked()), this, SLOT(setStatusChangesMade()));
	connect(ui.pSetMissionStartTime, SIGNAL(clicked()), this, SLOT(setStatusChangesMade()));
	connect(ui.pEnTempCalib, SIGNAL(clicked()), this, SLOT(setStatusChangesMade()));
	connect(ui.pMissionStartTime, SIGNAL(dateTimeChanged(QDateTime)), this, SLOT(setStatusChangesMade()));
	connect(ui.btnTimeNow, SIGNAL(clicked()), this, SLOT(setStartTimeNow()));

	MissionParameterFile& mp = MissionParameterFile::Instance();

	ui.txtStatus->setStyleSheet(YELLOW);
	ui.txtStatus->setText("Loading mission parameter file.");

	if(!mp.loadMissionParameters()){
		ui.txtStatus->setStyleSheet(RED);
		ui.txtStatus->setText("Could not load mission parameter file.");
	}
	else{
		ui.txtStatus->setStyleSheet(GREEN);
		ui.txtStatus->setText("Mission parameter file loaded.");
	}
	initUI();
	toggleSelectableParam();
}

MissionParameterUI::~MissionParameterUI()
{

}

void MissionParameterUI::closeWindow()
{
	this->close();
	return;
}

void MissionParameterUI::initUI()
{
    MissionParameterFile& mp = MissionParameterFile::Instance();
	ui.pEnTempCalib->setChecked(mp.getEnableAutoTempCalib());
	ui.pSamplingRate->setValue(mp.getSamplingRate());
	ui.pEnRollover->setChecked(mp.getEnableRollover());
	ui.pEnHighTempRes->setChecked(mp.getHighTemperatureResolution());
	ui.pSetMissionStartTime->setChecked(mp.getSetMissionStartTime());
	ui.pMissionStartTime->setDateTime(QDateTime::fromTime_t(mp.getMissionStartTime()));
	initDone = true;
}

bool MissionParameterUI::storeMissionParameters()
{
    MissionParameterFile& mp = MissionParameterFile::Instance();
	ui.txtStatus->setStyleSheet(YELLOW);
	ui.txtStatus->setText("Saving mission parameter file.");

	// Copy window status to missionParameter array
	mp.setEnableAutoTempCalib(ui.pEnTempCalib->isChecked());
	mp.setSamplingRate(ui.pSamplingRate->value());
	mp.setEnableRollover(ui.pEnRollover->isChecked());
	mp.setHighTemperatureResolution(ui.pEnHighTempRes->isChecked());
	mp.setSetMissionStartTime(ui.pSetMissionStartTime->isChecked());

	QDateTime missionStart;
	// Ensure that second field is 0
	missionStart.setDate(ui.pMissionStartTime->date());
	missionStart.setTime(QTime(ui.pMissionStartTime->time().hour(), ui.pMissionStartTime->time().minute(), 0));
	mp.setMissionStartTime(missionStart.toTime_t());

	mp.storeMissionParameters();
	ui.txtStatus->setStyleSheet(GREEN);
	ui.txtStatus->setText("Mission parameter file saved.");
	return true;
}

void MissionParameterUI::setStatusChangesMade()
{
    if(initDone)
    {
        ui.txtStatus->setStyleSheet(YELLOW);
        ui.txtStatus->setText("Changes have been made. Please save or they will be lost.");
        toggleSelectableParam();
    }
}

void MissionParameterUI::toggleSelectableParam()
{
	if (ui.pSetMissionStartTime->isChecked())
	{
		ui.lMissionStartTime->setEnabled(true);
		ui.pMissionStartTime->setEnabled(true);
		ui.btnTimeNow->setEnabled(true);
	}
	else
	{
	    ui.lMissionStartTime->setEnabled(false);
	    ui.pMissionStartTime->setEnabled(false);
	    ui.btnTimeNow->setEnabled(false);
	}
}

void MissionParameterUI::setStartTimeNow()
{
    ui.pMissionStartTime->setDateTime(QDateTime::currentDateTime());
}
