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
#include "DBManagementUI.h"

DBManagementUI::DBManagementUI(QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);

	ui.txtStatus->setStyleSheet(STYLESHEETWHITE);

	// init combo boxes
	initComboArea();

	// connect signals and slots
	connect(ui.comboArea, SIGNAL(currentIndexChanged(QString)), this, SLOT(areaChanged()));
	connect(ui.comboButtonNr, SIGNAL(currentIndexChanged(QString)), this, SLOT(buttonNrChanged()));
	connect(ui.btnNextButton, SIGNAL(clicked()), this, SLOT(nextButtonPressed()));

	connect(ui.btnDelArea, SIGNAL(clicked()), this, SLOT(deleteAreaClicked()));
	connect(ui.btnDelButton, SIGNAL(clicked()), this, SLOT(deleteButtonClicked()));

	connect(ui.btnClose, SIGNAL(clicked()), this, SLOT(closeButtonClicked()));
}


DBManagementUI::~DBManagementUI()
{

}


void DBManagementUI::initComboArea()
{
	ui.comboArea->clear();

	DBAreaTable db;
	db.open();
	QStringList listArea;
	QStringList listNone;
	listNone.append("none");
	listArea = db.getAllArea();
	listArea.sort();
	ui.comboArea->addItems(listNone + listArea);
	db.close();
	updateComboButtonNr();

	ui.btnDelArea->setEnabled(false);
}


void DBManagementUI::updateComboButtonNr()
{
	ui.comboButtonNr->clear();
	DBButtonTable db;
	db.open();
	QStringList listNone;
	listNone.append("none");
	QStringList listButton = db.getAllButtonNr(ui.comboArea->currentText());

	if(listButton.size() == 0)
	{
		ui.btnDelButton->setEnabled(false);
		ui.btnNextButton->setEnabled(false);
	}
	else
	{
		ui.btnDelButton->setEnabled(true);
		ui.btnNextButton->setEnabled(true);
	}

	listButton.sort();
	ui.comboButtonNr->addItems(listNone+listButton);
	ui.comboButtonNr->setCurrentIndex(0);
	db.close();
}


void DBManagementUI::areaChanged()
{
	updateComboButtonNr();
	if(ui.comboArea->currentText()=="none")
	{
		ui.btnDelArea->setEnabled(false);
	}
	else
	{
		ui.btnDelArea->setEnabled(true);
	}
}


void DBManagementUI::buttonNrChanged()
{
	if(ui.comboButtonNr->currentText() != "none")
	{
		ui.btnDelButton->setEnabled(true);
		DBButtonTable dbButton;
		dbButton.open();
		actualButton = dbButton.readButton(ui.comboButtonNr->currentText());
		ButtonNr = ui.comboButtonNr->currentText();
		dbButton.close();

		if(actualButton.ButtonID!="")
		{
			QString bIDstr = actualButton.ButtonID.mid(0,2) + " "
						+ actualButton.ButtonID.mid(2,2) + " "
						+ actualButton.ButtonID.mid(4,2) + " "
						+ actualButton.ButtonID.mid(6,2) + " "
						+ actualButton.ButtonID.mid(8,2) + " "
						+ actualButton.ButtonID.mid(10,2) + " "
						+ actualButton.ButtonID.mid(12,2) + " "
						+ actualButton.ButtonID.mid(14,2);
			ui.txtButtonID->setText(bIDstr);
		}
		else
		{
			ui.txtButtonID->setText("");
		}
	}
	else
	{
		ui.btnDelButton->setEnabled(false);
	}
}

void DBManagementUI::closeButtonClicked()
{
	this->close();
}

void DBManagementUI::setStatusText(QString text, QString styleSheet)
{
	ui.txtStatus->setText(text);
	ui.txtStatus->setStyleSheet(styleSheet);
}

void DBManagementUI::nextButtonPressed()
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

void DBManagementUI::deleteAreaClicked()
{
	QString curArea = ui.comboArea->currentText();
	int curIndex = ui.comboArea->currentIndex();
	if(curIndex == (ui.comboArea->count()-1))
	{
		curIndex = 0;
	}

	if(!UserDialog::question("Do you really want to delete\n"
							 "Footprint " + curArea + " and all\n"
							 "associated information?"))
	{
		return;
	}

	if(curArea=="none")
	{
		this->setStatusText("No footprint selected.", STYLESHEETRED);
	}
	else
	{
		if(this->deleteArea(curArea))
		{
			this->setStatusText("Footprint " + curArea + " successfully deleted.", STYLESHEETGREEN);
		}
		else
		{
			this->setStatusText("Footprint " + curArea + " could not be deleted.", STYLESHEETRED);
			UserDialog::warning("Could not delete Footprint " + curArea, this->getReport());
			this->clearReport();
		}
	}

	this->initComboArea();
	ui.comboArea->setCurrentIndex(curIndex);
}

void DBManagementUI::deleteButtonClicked()
{
	QString curButton = ui.comboButtonNr->currentText();

	if(!UserDialog::question("Do you really want to delete\n"
							"Button " + curButton + " and all\n"
							"associated information?"))
	{
		return;
	}

	if(curButton=="none")
	{
		this->setStatusText("No iButton selected.", STYLESHEETRED);
	}
	else
	{
		if(this->deleteButton(curButton))
		{
			this->setStatusText("iButton " + curButton + " successfully deleted.", STYLESHEETGREEN);
		}
		else
		{
			this->setStatusText("iButton " + curButton + " could not be deleted.", STYLESHEETRED);
			UserDialog::warning("Could not delete iButton " + curButton, this->getReport());
			this->clearReport();
		}
	}

	// update apperance
	int curIndex = ui.comboButtonNr->currentIndex();
	if(curIndex == (ui.comboButtonNr->count()-1))
	{
		curIndex = 0;
	}
	this->updateComboButtonNr();
	ui.comboButtonNr->setCurrentIndex(curIndex);
}

void DBManagementUI::appendReport(QString error)
{
	if(report=="")
	{
		report.append(error);
	}
	else
	{
		report.append("\n" + error);
	}
}

void DBManagementUI::clearReport()
{
	report = "";
}

QString DBManagementUI::getReport()
{
	return report;
}

bool DBManagementUI::deleteButton(QString buttonNr)
{
	// Delete all the measurements to this iButton
	DBMeasurementTable dbMeas;
	dbMeas.open();
	if(!dbMeas.deleteMeasurementByButtonNr(buttonNr))
	{
		dbMeas.close();
		this->appendReport(QString("An Error occured, during deleting the Measurement from the database.")
				+ "\nSummary:\nMeasurements: not deleted\nMeasurementProfiles: not deleted\niButton: not deleted");
		return false;
	}
	dbMeas.close();

	// Delete all the measurementProfiles to this iButton
	DBMeasurementProfileTable dbProfile;
	dbProfile.open();
	if(!dbProfile.deleteProfileByButtonNr(buttonNr))
	{
		dbProfile.close();
		this->appendReport(QString("An Error occured, during deleting the Measurement Profiles from the database.")
				+ "Summary:\nMeasurements: deleted\nMeasurementProfiles: not deleted\niButton: not deleted");
		return false;
	}
	dbProfile.close();

	// Delete the iButton entry itself
	DBButtonTable dbButton;
	dbButton.open();
	if(!dbButton.deleteButtonByButtonNr(buttonNr))
	{
		dbButton.close();
		this->appendReport(QString("An Error occured, during deleting the iButton from the database.")
				+ "Summary:\nMeasurements: deleted\nMeasurementProfiles: deleted\niButton: not deleted");
		return false;
	}
	dbButton.close();

	return true;
}

bool DBManagementUI::deleteArea(QString area)
{
	// Get all buttons belonging to this area
	DBButtonTable dbButton;
	dbButton.open();

	QStringList buttonList = dbButton.getAllButtonNr(area);
	dbButton.close();

	// Delete all these buttons
	bool success = true;
	for(int i = 0; i<buttonList.size(); i++)
	{
		if(success)
		{
			success = this->deleteButton(buttonList[i]);
		}
	}

	if(!success)
	{
		this->appendReport("Could not delete all iButtons, therefore this Footprint and its Photos will not be deleted.");
		return false;
	}

	// Delete the Area itself
	DBAreaTable dbArea;
	dbArea.open();
	if(!dbArea.deleteArea(area))
	{
		this->appendReport(QString("Could not delete Footprint from the Database.")
				+ "Summary:\nPhotos iButton/Footprint: deleted\nMeasurements: deleted\nMeasurementProfiles: deleted\niButtons: deleted\nFootprint: not deleted");
		dbArea.close();
		return false;
	}

	return true;
}

