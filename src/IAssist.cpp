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
#include "IAssist.h"

IAssist::IAssist(QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);

	// Connect the exit button with a slot to exit IAssist
	connect(ui.btnExit, SIGNAL(clicked()), this, SLOT(exitButtonClicked()));

	// Connect the Programm Button with a slot to start the programming of buttons
	connect(ui.btnProgram, SIGNAL(clicked()), this, SLOT(programButtonClicked()));

	// Connect the Collect Button with a slot to start the collecting of buttons
	connect(ui.btnCollect, SIGNAL(clicked()), this, SLOT(collectButtonClicked()));

	// Connect the Mission Parameter Button with a slot to start the Mission Parameter Settings
	connect(ui.btnMissionParameter, SIGNAL(clicked()), this, SLOT(missionParameterButtonClicked()));

	// Connect the Remove Database Content Button with a slot to start the Remove DB Content UI
	connect(ui.btnRemoveDbCont, SIGNAL(clicked()), this, SLOT(removeDbContClicked()));

	connect(ui.btnDataPlot, SIGNAL(clicked()), this, SLOT(dataPlotButtonClicked()));

	// Initialize the Log file (create, clear)
	Log::logfile_init();

	// Set icon
	QIcon title ("./ico/iassist.png");
	this->setWindowIcon(title);

}

IAssist::~IAssist()
{
}

void IAssist::programButtonClicked()
{
	AutoProgramUI *program = new AutoProgramUI(deploymentId, this);
	program->show();
	program->activateWindow();
}

void IAssist::collectButtonClicked()
{
	CollectUI *collect = new CollectUI(deploymentId, this);
	collect->show();
	collect->activateWindow();
}

void IAssist::missionParameterButtonClicked()
{
	MissionParameterUI *missionParam = new MissionParameterUI(this);
	missionParam->show();
	missionParam->activateWindow();
}

void IAssist::removeDbContClicked()
{
	DBManagementUI *dbManUI = new DBManagementUI(deploymentId, this);
	dbManUI->show();
	dbManUI->activateWindow();
}

void IAssist::dataPlotButtonClicked()
{
    DataPlotUI *dataPlot = new DataPlotUI(deploymentId, this);
    dataPlot->show();
    dataPlot->activateWindow();
}

/*
 * The Application has to end if the user presses the exit button.
 */
void IAssist::exitButtonClicked()
{
	exit(0);
}

void IAssist::initDir()
{
	QDir log("log");
	QDir dir(QDir::current());

	if(!log.exists())
		dir.mkdir("log");

}

void IAssist::setDeploymentId(int deploymentId)
{
    DBDeploymentTable db;
    this->deploymentId = deploymentId;
    db.open();
    ui.txtDeploymentName->setText("Deployment: " + db.getDeploymentName(deploymentId) +
            " (ID " + QString::number(deploymentId) + ")");
    db.close();
}



