#ifndef MISSIONPARAMETERUI_H
#define MISSIONPARAMETERUI_H

// filepath for "mission_start_parameter.ext" file
#define	MISSION_PARAMETER_FILEPATH	"./mission_start_parameter.txt"

#include <iostream>
#include <QFile>
#include <QTextStream>
#include <QtGui/QDialog>
#include "ui_MissionParameterUI.h"
#include "Log.h"

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
    bool readMissionParameterFile();	//!> Load the mission parameter file if one is available and valid.
	int missionParameter[16];			//!> Array that contains mission parameter settings
	QStringList descriptions;			//!> QStringList containing all descriptions of the different values that can be set.
private slots:

    /**
     * Writes the mission parameter file. Sets the values in the file to
     * the values selected on the GUI.
     */
    bool writeMissionParameterFile();
    /**
     * Set the status field so that the user know changes have been made
     * and must be saved or they will be lost.
     */
    void setStatusChangesMade();
    /**
     * Receives a signal when close is clicked and closes the window.
     */
    void closeWindow();

};

#endif // MISSIONPARAMETERUI_H
