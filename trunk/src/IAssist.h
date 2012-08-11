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
#ifndef IASSIST_H
#define IASSIST_H

#include <QtGui/QDialog>
#include "ui_IAssist.h"
#include "ui/AutoProgramUI.h"
#include "ui/CollectUI.h"
#include "ui/MissionParameterUI.h"
#include "database/DBManagementUI.h"
#include "SystemCommand.h"
#include "ui/DataPlotUI.h"


/**
 * This is the main window of the iAssist software. It starts all the different subprogramms
 * if a corresponding button is clicked.
 */

class IAssist : public QDialog
{
    Q_OBJECT

public:
    /**
     * Default constructor. It initializes the iAssist main window.
     */
    IAssist(QWidget *parent = 0);
    /**
     * Default deconstructor
     */
    ~IAssist();

    /**
     * Checks if the necessary directories exist. If they don't exist
     * they will be created.
     */
    void initDir();

    void setDeploymentId(int deploymentId) { this->deploymentId = deploymentId; }

private slots:

	/**
	* Called if the Exit Button is clicked. It closes IAssist.
	*/
	void exitButtonClicked();

	/**
	* Called if the Remove DB Content Button is clicked. It starts the corresponding
	* Routine.
	*/
	void removeDbContClicked();

	/**
	* Called if the programming button is clicked. It starts the corresponding
	* routine.
	*/
	void programButtonClicked();

	/**
	* Called if the Collect Button is clicked. It starts the corresponding
	* routine.
	*/
	void collectButtonClicked();

	/**
	 * Called if the Mission Parameter File is clicked.
	 */
	void missionParameterButtonClicked();

	void dataPlotButtonClicked();

private:
	//! Link to the ui itself
    Ui::IAssistClass ui;

    int deploymentId;

};

#endif // IASSIST_H
