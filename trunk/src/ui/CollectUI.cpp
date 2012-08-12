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
#include "CollectUI.h"

CollectUI::CollectUI(int deploymentId, QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	ui.txtStatus->setText("Please select an iButton to look for and press start.");
	ui.txtStatus->setStyleSheet(STYLESHEETWHITE);

	// Set Redistribute activated
	ui.cbRedistribute->setChecked(true);

	// init combo boxes
	initComboArea();

	// init collect Thread
	collectThr = new CollectThread(deploymentId, this);
	collectThr->setRedistribute(ui.cbRedistribute->isChecked());

	// connect signals and slots
	connect(ui.comboArea, SIGNAL(currentIndexChanged(QString)), this, SLOT(areaChanged()));
	connect(ui.comboButtonNr, SIGNAL(currentIndexChanged(QString)), this, SLOT(buttonNrChanged()));
	connect(ui.btnNextButton, SIGNAL(clicked()), this, SLOT(nextButtonClicked()));
	connect(ui.cbRedistribute, SIGNAL(stateChanged(int)), this, SLOT(redistributeToggled(int)));

	connect(ui.btnClose, SIGNAL(clicked()), this, SLOT(closeButtonClicked()));
	connect(ui.btnStart, SIGNAL(clicked()), this, SLOT(startButtonClicked()));

	connect(collectThr, SIGNAL(setStatus(QString, QString)), this, SLOT(setStatusText(QString, QString)));
	connect(collectThr, SIGNAL(aborted()), this, SLOT(threadAborted()));
	connect(collectThr, SIGNAL(buttonFinished()), this, SLOT(nextButtonClicked()));

	this->deploymentId = deploymentId;
}

CollectUI::~CollectUI()
{

}

void CollectUI::initComboArea()
{
	DBAreaTable db(deploymentId);
	db.open();
	QStringList listNone;
	listNone.append("none");
	QStringList listArea;
	listArea = db.getAllArea();
	listArea.sort();
	ui.comboArea->addItems(listNone +listArea);
	db.close();
	updateComboButtonNr();
}

void CollectUI::updateComboButtonNr()
{
	ui.comboButtonNr->clear();
	DBButtonTable db(deploymentId);
	db.open();
	QStringList list;
	list.append("none");
	QStringList list2 =db.getAllButtonNr(ui.comboArea->currentText());
	list2.sort();
	ui.comboButtonNr->addItems(list+list2);
	ui.comboButtonNr->setCurrentIndex(0);
	db.close();
}

void CollectUI::areaChanged()
{
	updateComboButtonNr();
}

void CollectUI::buttonNrChanged()
{
    DBButtonTable dbButton(deploymentId);
    dbButton.open();
    actualButton = dbButton.getButtonByButtonNr(ui.comboButtonNr->currentText());
    dbButton.close();

    // Display the actual iButton ID
    if(actualButton.SerialNr != "")
    {
        QString bIDstr = actualButton.SerialNr.mid(0,2) + " "
                + actualButton.SerialNr.mid(2,2) + " "
                + actualButton.SerialNr.mid(4,2) + " "
                + actualButton.SerialNr.mid(6,2) + " "
                + actualButton.SerialNr.mid(8,2) + " "
                + actualButton.SerialNr.mid(10,2) + " "
                + actualButton.SerialNr.mid(12,2) + " "
                + actualButton.SerialNr.mid(14,2);
        ui.txtButtonID->setText(bIDstr);
    }
    else
    {
        ui.txtButtonID->setText("");
    }
    // Tell the thread what iButton is selected
    collectThr->setButton(actualButton);


}

void CollectUI::closeButtonClicked()
{
	this->close();
}

void CollectUI::startButtonClicked()
{
	if(ui.btnStart->text()=="&Start")
	{
		ui.btnStart->setText("&Stop");
		ui.btnClose->setEnabled(false);
		collectThr->start();
		collectThr->setButton(actualButton);
		collectThr->setRedistribute(ui.cbRedistribute->isChecked());
	}
	else
	{
		collectThr->stop();
		while(!collectThr->isFinished())
		{
			usleep(10000);
		}
		ui.btnStart->setText("&Start");
		ui.btnClose->setEnabled(true);
	}
}

void CollectUI::setStatusText(QString text, QString styleSheet)
{
	QString temp = ui.txtStatus->toPlainText();
	QStringList tmpList = temp.split('\n');

	if(tmpList.last()!=text)
	{
		ui.txtStatus->append(text);
		if(styleSheet!="") ui.txtStatus->setStyleSheet(styleSheet);
		QTextCursor cursor(ui.txtStatus->textCursor());
		cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
		ui.txtStatus->setTextCursor(cursor);
	}
}

void CollectUI::nextButtonClicked()
{
	int size = ui.comboButtonNr->count();
	int act = ui.comboButtonNr->currentIndex();
	act++;
	if(act>size-1)
	{
		ui.comboButtonNr->setCurrentIndex(0);
	}
	else if(act<=size-1)
	{
		ui.comboButtonNr->setCurrentIndex(act);
	}
}

void CollectUI::redistributeToggled(int state)
{
	if(state > 0) {
		collectThr->setRedistribute(true);
	}
	else {
		collectThr->setRedistribute(false);
	}
}

void CollectUI::threadAborted()
{
	this->startButtonClicked();
}



