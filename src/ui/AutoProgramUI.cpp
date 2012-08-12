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
#include "AutoProgramUI.h"

AutoProgramUI::AutoProgramUI(int deploymentId, QWidget *parent)
    : QDialog(parent)
{
	currentArea="AA";

	ui.setupUi(this);

	autoProg = new AutoProgramThread(deploymentId, this);
	autoProgramRunning = false;

	connect(ui.btnStart, SIGNAL(clicked()), this, SLOT(buttonSwitch()));
	connect(ui.btnDecArea, SIGNAL(clicked()), this, SLOT(decArea()));
	connect(ui.btnIncArea, SIGNAL(clicked()), this, SLOT(incArea()));
	connect(ui.btnDecAreaMore, SIGNAL(clicked()), this, SLOT(decAreaMore()));
	connect(ui.btnIncAreaMore, SIGNAL(clicked()), this, SLOT(incAreaMore()));
	connect(ui.btnClose, SIGNAL(clicked()), this, SLOT(closeClicked()));

}

AutoProgramUI::~AutoProgramUI()
{

}

void AutoProgramUI::setStatusColor(int color){

	switch(color){
	case 0: // red
		ui.txtStatus->setStyleSheet("background-color: rgb(255, 181, 181);"); // red
		break;
	case 1: // yellow
		ui.txtStatus->setStyleSheet("background-color: rgb(255, 246, 207);"); // yellow
		break;
	case 2: // green
		ui.txtStatus->setStyleSheet("background-color: rgb(190, 255, 196);"); // green
		break;
	}
}

void AutoProgramUI::buttonSwitch(){
	if (autoProgramRunning) {
		//Program running
		autoProgramRunning = false;
		autoProg->stop();
		while(!autoProg->isFinished())
			usleep(100000);
		ui.btnStart->setText("Start");
	}
	else {
		//Program not running
		autoProgramRunning = true;
		autoProg->start();
		ui.btnStart->setText("Stop");
	}

}

void AutoProgramUI::closeClicked()
{
	if (autoProg->isRunning()){
		autoProg->stop();
		while(!autoProg->isFinished()) usleep(100000);
	}
	delete autoProg;
	this->close();
}

void AutoProgramUI::readStatus(QString status){
	ui.txtStatus->setText(status.toStdString().c_str());
}

void AutoProgramUI::readButtonNr(QString ButtonNr){
	ui.txtButtonNr->setText(ButtonNr);
}

void AutoProgramUI::readButtonID(QString ButtonID){
	if(ButtonID.size()<=16)
	{
		QString bIDstr = ButtonID.mid(0,2) + " "
					+ ButtonID.mid(2,2) + " "
					+ ButtonID.mid(4,2) + " "
					+ ButtonID.mid(6,2) + " "
					+ ButtonID.mid(8,2) + " "
					+ ButtonID.mid(10,2) + " "
					+ ButtonID.mid(12,2) + " "
					+ ButtonID.mid(14,2);
		ui.txtButtonID->setText(bIDstr);
	}
	else
	{
		ui.txtButtonID->setText(ButtonID);
	}
}

void AutoProgramUI::incArea(){
	currentArea = ui.lblArea->text();

	// Increase the area by 1

	QChar charAr1 = currentArea[0];
	QChar charAr2 = currentArea[1];
	ushort value1 = charAr1.unicode();
	ushort value2 = charAr2.unicode();

	if(value2<65)	// If value2 < (Capital ASCII letters)
	{
		currentArea.replace(1,1, QString("A")); // Set to A
	}
	else if(value2>=90)	// If value2 > (Capital ASCII letters) OR "Z"
	{
		currentArea.replace(1,1, QString("A")); // Set to A

		if(value1<65)	// If value1 < (Capital ASCII letters)
		{
			currentArea.replace(0,1, QString("A")); // Set to A
		}
		else if(value1>=90)	// If value1 > (capital ASCII letters) OR "Z"
		{
			currentArea.replace(0,1, QString("A")); // Set to A
		}
		else // If value1 == (Capital ASCII letter)
		{
			currentArea.replace(0,1, QString(QChar(value1+1))); // Increase by 1
		}
	}
	else // If value2 == (Capital ASCII letter)
	{
		currentArea.replace(1,1, QString(QChar(value2+1))); // Increase by 1
	}

	ui.lblArea->setText(currentArea);
	emit setArea(currentArea);
}

void AutoProgramUI::decArea(){
	currentArea = ui.lblArea->text();

	// Decrease the area by 1
	QChar charAr1 = currentArea[0];
	QChar charAr2 = currentArea[1];
	ushort value1 = charAr1.unicode();
	ushort value2 = charAr2.unicode();

	if(value2<=65)
	{
		currentArea.replace(1 , 1, QString("Z"));
		if(value1<=65)
		{
			currentArea.replace(0, 1, QString("Z"));
		}
		else if(value1>90)
		{
			currentArea.replace(0, 1, QString("Z"));
		}
		else
		{
			currentArea.replace(0, 1, QString(QChar(value1-1)));
		}
	}
	else if(value2>90)
	{
		currentArea.replace(1 , 1, QString("Z"));
	}
	else
	{
		currentArea.replace(1, 1, QString(QChar(value2-1)));
	}

	ui.lblArea->setText(currentArea);
	emit setArea(currentArea);
}


void AutoProgramUI::incAreaMore(){
	currentArea = ui.lblArea->text();

	// Increase the area by 26 (left letter)

	QChar charAr1 = currentArea[0];
	ushort value1 = charAr1.unicode();

		if(value1<65)	// If value1 < (Capital ASCII letters)
		{
			currentArea.replace(0,1, QString("A")); // Set to A
		}
		else if(value1>=90)	// If value1 > (capital ASCII letters) OR "Z"
		{
			currentArea.replace(0,1, QString("A")); // Set to A
		}
		else // If value1 == (Capital ASCII letter)
		{
			currentArea.replace(0,1, QString(QChar(value1+1))); // Increase by 1
		}

	ui.lblArea->setText(currentArea);
	emit setArea(currentArea);
}

void AutoProgramUI::decAreaMore(){
	currentArea = ui.lblArea->text();

	// Decrease the area by 26 (left letter)
	QChar charAr1 = currentArea[0];
	ushort value1 = charAr1.unicode();

		if(value1<=65)
		{
			currentArea.replace(0, 1, QString("Z"));
		}
		else if(value1>90)
		{
			currentArea.replace(0, 1, QString("Z"));
		}
		else
		{
			currentArea.replace(0, 1, QString(QChar(value1-1)));
		}

	ui.lblArea->setText(currentArea);
	emit setArea(currentArea);
}
