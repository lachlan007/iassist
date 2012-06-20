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

	descriptions.insert(0,"automatic temperature calibration\t\t\t\t\t[0,1]");
	descriptions.insert(1, "sample rate [s]\t\t\t\t\t\t\t\t[1,982800]");
	descriptions.insert(2, "start delay [min]\t\t\t\t\t\t\t[0,16777215]");
	descriptions.insert(3, "enable rollover\t\t\t\t\t\t\t\t[0,1]");
	descriptions.insert(4, "synchronize time with computer\t\t\t\t\t\t[0,1]");
	descriptions.insert(5, "high temperature resolution\t\t\t\t\t\t[0,1]\t0: 8 Bit | 1: 16 Bit");
	descriptions.insert(6, "start mission at 00:00\t\t\t\t\t\t\t[0,1]");
	descriptions.insert(7, "day delay, if you want to start mission at 00:00 several days later\t[0,365]");

	connect(ui.btnSave, SIGNAL(clicked()), this, SLOT(writeMissionParameterFile()));
	connect(ui.btnClose, SIGNAL(clicked()), this, SLOT(closeWindow()));
	connect(ui.param0, SIGNAL(clicked()), this, SLOT(setStatusChangesMade()));
	connect(ui.param1, SIGNAL(valueChanged(int)), this, SLOT(setStatusChangesMade()));
	connect(ui.param2, SIGNAL(valueChanged(int)), this, SLOT(setStatusChangesMade()));
	connect(ui.param3, SIGNAL(clicked()), this, SLOT(setStatusChangesMade()));
	connect(ui.param4, SIGNAL(clicked()), this, SLOT(setStatusChangesMade()));
	connect(ui.param5, SIGNAL(clicked()), this, SLOT(setStatusChangesMade()));
	connect(ui.param6, SIGNAL(clicked()), this, SLOT(setStatusChangesMade()));
	connect(ui.param7, SIGNAL(valueChanged(int)), this, SLOT(setStatusChangesMade()));

	if(!readMissionParameterFile()){
		ui.txtStatus->setStyleSheet(RED);
		ui.txtStatus->setText("Could not load mission parameter file.");
	}
	else{
		ui.txtStatus->setStyleSheet(GREEN);
		ui.txtStatus->setText("Mission parameter file loaded.");
		toggleSelectableParam();
	}
}

MissionParameterUI::~MissionParameterUI()
{

}

void MissionParameterUI::closeWindow(){
	this->close();
	return;
}

bool MissionParameterUI::readMissionParameterFile()
{
	ui.txtStatus->setStyleSheet(YELLOW);
	ui.txtStatus->setText("Loading mission parameter file.");

	// Create a file handle
	QFile file(MISSION_PARAMETER_FILEPATH);

	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) // Open the file
	{
		Log::writeError("missionParameterUI: Can't open mission parameter file.");
		return false;
	}
	else
	{
		QTextStream in(&file);
		int lineCount;
		for(lineCount=0; lineCount<8; lineCount++)
		{
			if (!in.atEnd())
			{
		         QString line = in.readLine();
		         missionParameter[lineCount] = line.split("\t")[0].toInt();

			}
		}
		//file.close();
	}
	file.close();

	// check if the settings value for sample rate and start delay
	// are inside their boundaries.
	if((missionParameter[1]<1) || (missionParameter[1]>982800))
	{
		Log::writeError("missionParameterUI: Sample rate in file is out of its limits.");
		return false;
	}
	else if((missionParameter[2]<0) || (missionParameter[2]>16777215))
	{
		Log::writeError("missionParameterUI: Start delay in file is out of its limits.");
		return false;
	}
	else if((missionParameter[7]<0) || (missionParameter[7])>365)
	{
		Log::writeError("missionParameterUI: Day delay in file is out of its limits.");
		return false;
	}

	// check for invalid boolean settings in "mission_start_parameters".
	for(int t=3;t<=6;t++)
	{
		if(!((missionParameter[t]==0) || (missionParameter[t]==1)))
		{
			Log::writeError("missionParameterUI: Boolean setting in file has a wrong value.");
			return false;
		}
	}

	// Copy missionParameter array status to window
	ui.param0->setChecked(missionParameter[0]);
	ui.param1->setValue(missionParameter[1]);
	ui.param2->setValue(missionParameter[2]);
	ui.param3->setChecked(missionParameter[3]);
	ui.param4->setChecked(missionParameter[4]);
	ui.param5->setChecked(missionParameter[5]);
	ui.param6->setChecked(missionParameter[6]);
	ui.param7->setValue(missionParameter[7]);

	return true;

}

bool MissionParameterUI::writeMissionParameterFile()
{
	ui.txtStatus->setStyleSheet(YELLOW);
	ui.txtStatus->setText("Saving mission parameter file.");

	// Copy window status to missionParameter array
	missionParameter[0] = ui.param0->isChecked();
	missionParameter[1] = ui.param1->value();
	missionParameter[2] = ui.param2->value();
	missionParameter[3] = ui.param3->isChecked();
	missionParameter[4] = ui.param4->isChecked();
	missionParameter[5] = ui.param5->isChecked();
	missionParameter[6] = ui.param6->isChecked();
	missionParameter[7] = ui.param7->value();

	// Create a file handle
	QFile file(MISSION_PARAMETER_FILEPATH);

	if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) // Open the file
	{
		Log::writeError("missionParameterUI: Can't open mission parameter file.");
		ui.txtStatus->setStyleSheet(RED);
		ui.txtStatus->setText("Saving mission parameter file failed.");
		return false;
	}
	else
	{
		QTextStream out(&file);
		int lineCount;
		for(lineCount=0; lineCount<8; lineCount++)
		{
			out << missionParameter[lineCount] << "\t" << descriptions[lineCount] << "\n";
		}
		//file.close();
	}
	file.close();
	ui.txtStatus->setStyleSheet(GREEN);
	ui.txtStatus->setText("Mission parameter file saved.");
	return true;
}

void MissionParameterUI::setStatusChangesMade(){
	ui.txtStatus->setStyleSheet(YELLOW);
	ui.txtStatus->setText("Changes have been made. Please save or they will be lost.");

	toggleSelectableParam();
}

void MissionParameterUI::toggleSelectableParam(){
	if (ui.param6->isChecked())
	{
		ui.param2->setEnabled(false);
		ui.label2->setEnabled(false);
		ui.param7->setEnabled(true);
		ui.label7->setEnabled(true);
		ui.param2->setValue(0);
	}
	else
	{
		ui.param2->setEnabled(true);
		ui.label2->setEnabled(true);
		ui.param7->setEnabled(false);
		ui.label7->setEnabled(false);
		ui.param7->setValue(0);
	}
}
