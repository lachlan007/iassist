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
#ifndef MISSIONPARAMETERUI_H
#define MISSIONPARAMETERUI_H

#include <QtGui/QDialog>
#include "ui_MissionParameterUI.h"
#include "../Log.h"
#include "../MissionParameterFile.h"

/**
 * Displays a GUI to set the mission parameter for the iButton.
 */

class MissionParameterUI : public QDialog
{
    Q_OBJECT

public:
    /**
     * Displays the mission parameter window and calls the
     * readMissionParameterFile() function, so that the displayed initial
     * values are the same as in the file (if there is one).
     */
    MissionParameterUI(QWidget *parent = 0);
    /**
     * Default destructor.
     */
    ~MissionParameterUI();

private:
    Ui::MissionParameterUIClass ui;
    /**
     * Reads the mission parameter file. Sets the values selected on
     * the GUI to the values in the file.
     */
    void toggleSelectableParam();		//!> If starting time is set to 00:00 disable starting time delay in seconds and vice versa.

    void initUI();

    bool initDone;

private slots:

    /**
     * Writes the mission parameter file. Sets the values in the file to
     * the values selected on the GUI.
     */
    bool storeMissionParameters();
    /**
     * Set the status field so that the user know changes have been made
     * and must be saved or they will be lost.
     */
    void setStatusChangesMade();
    /**
     * Receives a signal when close is clicked and closes the window.
     */
    void closeWindow();

    void setStartTimeNow();

};

#endif // MISSIONPARAMETERUI_H
