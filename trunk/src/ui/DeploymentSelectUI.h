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
#ifndef DEPLOYMENTSELECTUI_H
#define DEPLOYMENTSELECTUI_H

#include <QtGui/QDialog>
#include "ui_DeploymentSelectUI.h"
#include "UserDialog.h"
#include "../database/DBDeploymentTable.h"
#include "../database/DBFootprintTable.h"
#include "../database/DBButtonTable.h"
#include "../database/DBMeasurementProfileTable.h"
#include "../database/DBMeasurementTable.h"

/**
 * Displays a GUI to set the mission parameter for the iButton.
 */

class DeploymentSelectUI : public QDialog
{
    Q_OBJECT

public:
    /**
     * Displays the mission parameter window and calls the
     * readMissionParameterFile() function, so that the displayed initial
     * values are the same as in the file (if there is one).
     */
    DeploymentSelectUI(QWidget *parent = 0);
    /**
     * Default destructor.
     */
    ~DeploymentSelectUI();

    int getSelectedDeploymentId() { return selectedDeploymentId; }

private:
    Ui::DeploymentSelectUIClass ui;

    void initUI();

    int selectedDeploymentId;

    QVector<QPair<int, QString> > deployments;

private slots:

    void loadDeployment();

    void createNewDeployment();

    void deleteDeployment();

    void quitApplication();

};

#endif // DEPLOYMENTSELECTUI_H
