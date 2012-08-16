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

DBManagementUI::DBManagementUI(int deploymentId, QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);
    this->deploymentId = deploymentId;

	ui.txtStatus->setStyleSheet(STYLESHEETWHITE);

	// init combo boxes
	initComboFootprint();

	// connect signals and slots
	connect(ui.comboFootprint, SIGNAL(currentIndexChanged(QString)), this, SLOT(footprintChanged()));
	connect(ui.comboButtonNr, SIGNAL(currentIndexChanged(QString)), this, SLOT(buttonNrChanged()));
	connect(ui.btnNextButton, SIGNAL(clicked()), this, SLOT(nextButtonPressed()));

	connect(ui.btnDelFootprint, SIGNAL(clicked()), this, SLOT(deleteFootprintClicked()));
	connect(ui.btnDelButton, SIGNAL(clicked()), this, SLOT(deleteButtonClicked()));

	connect(ui.btnClose, SIGNAL(clicked()), this, SLOT(closeButtonClicked()));

}


DBManagementUI::~DBManagementUI()
{

}


void DBManagementUI::initComboFootprint()
{
	ui.comboFootprint->clear();
	DBFootprintTable db(deploymentId);
	db.open();
	QStringList listCombo;
	listCombo.append("none");
	QVector<int> listFootprint = db.getAllFootprints();
	for(int i=0; i<listFootprint.size(); i++)
	{
	    listCombo.append(QString::number(listFootprint.at(i)).rightJustified(3, '0'));
	}
	ui.comboFootprint->addItems(listCombo);
	db.close();
	updateComboButtonNr();

	ui.btnDelFootprint->setEnabled(false);
}


void DBManagementUI::updateComboButtonNr()
{
	ui.comboButtonNr->clear();
	DBButtonTable db(deploymentId);
	db.open();
	QStringList listCombo;
	int footprintPrefix = ui.comboFootprint->currentText().toInt();
	listCombo.append("none");
	QVector<int> listButton = db.getAllButtonNr(footprintPrefix);
	for(int i=0; i<listButton.size(); i++)
	{
	    listCombo.append(QString::number(listButton.at(i)-footprintPrefix*1000).rightJustified(3, '0'));
	}

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

	ui.comboButtonNr->addItems(listCombo);
	ui.comboButtonNr->setCurrentIndex(0);
	db.close();
}


void DBManagementUI::footprintChanged()
{
	updateComboButtonNr();
	if(ui.comboFootprint->currentText()=="none")
	{
		ui.btnDelFootprint->setEnabled(false);
	}
	else
	{
		ui.btnDelFootprint->setEnabled(true);
	}
}


void DBManagementUI::buttonNrChanged()
{
	if(ui.comboButtonNr->currentText() != "none")
	{
		ui.btnDelButton->setEnabled(true);
		DBButtonTable dbButton(deploymentId);
		dbButton.open();
		actualButton = dbButton.getButtonByButtonNr(ui.comboButtonNr->currentText().toInt()+1000*ui.comboFootprint->currentText().toInt());
		ButtonNr = ui.comboButtonNr->currentText();
		dbButton.close();

		if(actualButton.SerialNr!="")
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

void DBManagementUI::deleteFootprintClicked()
{
	QString curFootprint = ui.comboFootprint->currentText();
	int curIndex = ui.comboFootprint->currentIndex();
	// resets combo box when last item is deleted
	if(curIndex == (ui.comboFootprint->count()-1))
	{
		curIndex = 0;
	}

	if(!UserDialog::question("Do you really want to delete\n"
							 "Footprint " + curFootprint + " and all\n"
							 "associated information?"))
	{
		return;
	}

	if(curFootprint=="none")
	{
		this->setStatusText("No footprint selected.", STYLESHEETRED);
	}
	else
	{
		if(this->deleteFootprint(curFootprint.toInt()))
		{
			this->setStatusText("Footprint " + curFootprint + " successfully deleted.", STYLESHEETGREEN);
		}
		else
		{
			this->setStatusText("Footprint " + curFootprint + " could not be deleted.", STYLESHEETRED);
			UserDialog::warning("Could not delete Footprint " + curFootprint, this->getReport());
			this->clearReport();
		}
	}

	this->initComboFootprint();
	ui.comboFootprint->setCurrentIndex(curIndex);
}

void DBManagementUI::deleteButtonClicked()
{
	QString curButton = ui.comboFootprint->currentText() + " " + ui.comboButtonNr->currentText();

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
		if(this->deleteButton(1000*ui.comboFootprint->currentText().toInt()+ui.comboButtonNr->currentText().toInt()))
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

	// update appearance
	int curIndex = ui.comboButtonNr->currentIndex();
	// resets combo box when last item is deleted
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

bool DBManagementUI::deleteButton(int buttonNr)
{
	// Delete all the measurements to this iButton
	DBMeasurementTable dbMeas;
	DBMeasurementProfileTable dbProfile;
	DBButtonTable dbButton(deploymentId);
	dbMeas.open();
	dbProfile.open();
    dbButton.open();
	int buttonId = dbButton.getButtonIdByButtonNr(buttonNr);
	QVector<MeasurementProfile> profiles = dbProfile.getProfilesByButtonID(buttonId);

	for(int i=0; i<profiles.size(); i++)
	{
	    if(!dbMeas.deleteMeasurementsByMeasurementProfileID(profiles.at(i).MeasurementProfileID))
	    {
	        dbMeas.close();
	        dbProfile.close();
	        dbButton.close();
	        this->appendReport(QString("An Error occured, during deleting the Measurement from the database.")
	                + "\nSummary:\nMeasurements: not deleted\nMeasurementProfiles: not deleted\niButton: not deleted");
	        return false;
	    }
	}
	dbMeas.close();

	// Delete all the measurementProfiles to this iButton
	if(!dbProfile.deleteProfileByButtonId(buttonId))
	{
		dbProfile.close();
		dbButton.close();
		this->appendReport(QString("An Error occurred, during deleting the Measurement Profiles from the database.")
				+ "Summary:\nMeasurements: deleted\nMeasurementProfiles: not deleted\niButton: not deleted");
		return false;
	}
	dbProfile.close();

	// Delete the iButton entry itself
	if(!dbButton.deleteButtonByButtonId(buttonId))
	{
		dbButton.close();
		this->appendReport(QString("An Error occured, during deleting the iButton from the database.")
				+ "Summary:\nMeasurements: deleted\nMeasurementProfiles: deleted\niButton: not deleted");
		return false;
	}
	dbButton.close();

	return true;
}

bool DBManagementUI::deleteFootprint(int footprint)
{
	// Get all buttons belonging to this area
	DBButtonTable dbButton(deploymentId);
	dbButton.open();

	QVector<int> buttonList = dbButton.getAllButtonNr(footprint);
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
		this->appendReport("Could not delete all iButtons, therefore this Footprint will not be deleted.");
		return false;
	}

	// Delete the footprint itself
	DBFootprintTable dbFootprint(deploymentId);
	dbFootprint.open();
	if(!dbFootprint.deleteFootprint(footprint))
	{
		this->appendReport(QString("Could not delete Footprint from the Database.")
				+ "Summary:\nPhotos iButton/Footprint: deleted\nMeasurements: deleted\nMeasurementProfiles: deleted\niButtons: deleted\nFootprint: not deleted");
		dbFootprint.close();
		return false;
	}

	return true;
}

