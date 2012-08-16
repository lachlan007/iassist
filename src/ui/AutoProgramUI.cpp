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
    ui.setupUi(this);

	autoProg = new AutoProgramThread(deploymentId, this);
	autoProgramRunning = false;

	connect(ui.btnStart, SIGNAL(clicked()), this, SLOT(buttonSwitch()));
	connect(ui.cbFootprintPrefix, SIGNAL(currentIndexChanged(int)), this, SLOT(footprintPrefixChanged(int)));
	connect(ui.cbButtonNrSuffix, SIGNAL(currentIndexChanged(QString)), this, SLOT(buttonNrSuffixChanged(QString)));
	connect(ui.btnClose, SIGNAL(clicked()), this, SLOT(closeClicked()));

	dbButton = new DBButtonTable(deploymentId);

	setupPrefixSelect();
	setupSuffixSelect(1);
}

AutoProgramUI::~AutoProgramUI()
{
    delete dbButton;
}

void AutoProgramUI::setupPrefixSelect() {
    QStringList prefixList;
    // Initialize prefix combo box
    for(int i=1; i<=999; i++)
    {
        QString item = QString::number(i);
        item = item.rightJustified(3, '0');
        prefixList.append(item);
    }
    ui.cbFootprintPrefix->clear();
    ui.cbFootprintPrefix->addItems(prefixList);
}

void AutoProgramUI::setupSuffixSelect(int footprintPrefix)
{
    QStringList items;
    items.append(QString(AUTOINCREMENTTEXT));
    dbButton->open();
    QVector<int> existing = dbButton->getAllButtonNr(footprintPrefix);
    dbButton->close();
    for(int i=0; i<existing.size(); i++)
    {
        items.append(QString::number(existing.at(i)-footprintPrefix*1000).rightJustified(3, '0'));
    }
    ui.cbButtonNrSuffix->clear();
    ui.cbButtonNrSuffix->addItems(items);
    emit setButtonNrSuffix(0); // auto-increment
    currentButtonNrPrefixIdx = 0;
}

void AutoProgramUI::buttonSwitch(){
	if (autoProgramRunning) {
		//Program running
	    autoProgramRunning = false;
		autoProg->stop();
		while(!autoProg->isFinished())
		    usleep(100000);
		ui.btnStart->setText("Start");
		ui.btnClose->setEnabled(true);
	}
	else {
	    //Program not running
	    ui.btnClose->setEnabled(false);
	    autoProgramRunning = true;
	    autoProg->start();
	    ui.btnStart->setText("Stop");
	}
    writeButtonNr("");
    writeSerialNr("");
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

void AutoProgramUI::footprintPrefixChanged(int selected)
{
    writeButtonNr("");
    writeSerialNr("");
    setupSuffixSelect(selected+1);
    emit setFootprintPrefix(selected+1);
}

void AutoProgramUI::buttonNrSuffixChanged(QString selected)
{
    if(selected != "" && selected != QString(AUTOINCREMENTTEXT))
    {
        if(UserDialog::question("Do you really want to replace button " + selected + "?"))
        {
            emit setButtonNrSuffix(selected.toInt());
            currentButtonNrPrefixIdx = ui.cbButtonNrSuffix->currentIndex();
        }
        else
        {
            ui.cbButtonNrSuffix->setCurrentIndex(currentButtonNrPrefixIdx);
        }
    }
    else
    {
        // 0 = auto-increment
        emit setButtonNrSuffix(0);
        currentButtonNrPrefixIdx = 0;
    }
}

void AutoProgramUI::resetButtonNrSuffixCombo()
{
    setupSuffixSelect(ui.cbFootprintPrefix->currentIndex()+1);
}


void AutoProgramUI::writeButtonNr(QString ButtonNr){
	ui.txtButtonNr->setText(ButtonNr);
}

void AutoProgramUI::writeSerialNr(QString SerialNr){
	if(SerialNr.size()<=16)
	{
		QString bIDstr = SerialNr.mid(0,2) + " "
					+ SerialNr.mid(2,2) + " "
					+ SerialNr.mid(4,2) + " "
					+ SerialNr.mid(6,2) + " "
					+ SerialNr.mid(8,2) + " "
					+ SerialNr.mid(10,2) + " "
					+ SerialNr.mid(12,2) + " "
					+ SerialNr.mid(14,2);
		ui.txtButtonID->setText(bIDstr);
	}
	else
	{
		ui.txtButtonID->setText(SerialNr);
	}
}

void AutoProgramUI::setStatusText(QString text, QString styleSheet)
{
    ui.txtStatus->setText(text);
    if(styleSheet!="") ui.txtStatus->setStyleSheet(styleSheet);
}

