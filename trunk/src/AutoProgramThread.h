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
#ifndef AUTOPROGRAMTHREAD_H_
#define AUTOPROGRAMTHREAD_H_

#include <QThread>
#include <QString>
#include <QObject>
#include <QApplication>
#include <QFile>
#include <QTextStream>
#include "Log.h"
#include "database/DBButtonTable.h"
#include "database/DBFootprintTable.h"
#include "database/DBMeasurementProfileTable.h"
#include "MeasurementProfile.h"
#include "ButtonData.h"
#include "ButtonIO.h"
#include <QMutex>
#include <QWaitCondition>
#include <QMessageBox>
#include <QtCore>
#include "MissionParameterFile.h"

#define STYLESHEETRED       "background-color: rgb(255, 181, 181);"
#define STYLESHEETYELLOW    "background-color: rgb(255, 246, 207);"
#define STYLESHEETGREEN     "background-color: rgb(190, 255, 196);"
#define STYLESHEETWHITE     "background-color: rgb(255, 255, 255);"

/**
 * This is the thread class for the automatic programming of iButtons at home.
 * It connects to the iButton reader and chcecks whether an iButton is present
 * or not and if applicable programs it.
 */
class AutoProgramThread : public QThread {
	Q_OBJECT

public:
	/**
	 * Default constructor.
	 *
	 * @param parent	reference to the parent
	 */
	AutoProgramThread(int deploymentId, QObject *parent);
	/**
	 * Default destructor.
	 */
	~AutoProgramThread();
	/**
	 * Checks if there is an iButton in the reader and programs it if applicable.
	 */
	void run();
	/**
	 * Used to stop the continuous calling of the iButton-check function
	 * (offered by the autoProgram object) in the run-loop of this thread.
	 */
	void stop();

public slots:
	/**
	 * Receives the updated area from AutoProgramUI, when the user has changed it
	 * on the GUI.
	 *
	 * @param prefix	Footprint prefix
	 */
	void setFootprintPrefix(int prefix);

    /**
     * Receives the updated area from AutoProgramUI, when the user has changed it
     * on the GUI.
     *
     * @param suffix    ButtonNr suffix
     */
	void setButtonNrSuffix(int suffix);

private:
	int deploymentId;

	/**
	 * Needed by the stop() function, to tell the while loop it should finish
	 * the current loop and then stop.
	 */
	bool threadEnabled;

	/**
	 * Keeps track of the current ButtonNr prefix. This value must be synchronized between
	 * AutoProgramThread and its parent AutoProgramUI. AutoProgramThread updates
	 * it via the SLOT setButtonNrPrefix().
	 */
	int footprintPrefix;

	/**
	 * Keeps track of the current ButtonNr suffix. If set, an existing ButtonNr is assigned. If set to -1, an auto-incremented suffix is chosen.
	 */
	int buttonNrSuffix;

	/**
	 * Manages the database connection to the button table.
	 */
	DBButtonTable *dbButton;
	/**
	 * Manages the database connection to the area table.
	 */
	DBFootprintTable *dbFootprint;
	/**
	 * Manages the database connection to the Measurement Profile table
	 */
	DBMeasurementProfileTable *dbProfile;
	/**
	 * ButtonIO instance used for the communication to the iButton reader.
	 */
	ButtonIO *iButtonCon;
	/**
	 * If the connection to the iButton reader can't be opened this bool is
	 * set to false for further reference.
	 */
	bool readerAvailable;
	/**
	 * If an error occures while programming the iButton this bool is set to
	 * true. This is needed for the following programming operations, so that
	 * they aren't executed.
	 */
	bool error;
	/**
	 * Object keeping track of informations about the currently present iButton.
	 * Needed to pass the informations as one piece.
	 */
	ButtonData button;
	/**
	 * Object keeping track of informations about the currently used Measurement profile
	 */
	MeasurementProfile profile;
	/**
	 * Keeps track of the last ButtonID. This is used to ensure a button isn't
	 * programmed twice in a row.
	 */
	QString lastSerialNr;
	//! used to lock and stop the thread while a dialog box is displayed
    QWaitCondition waitCondition;
    //! used to lock and stop the thread while a dialog box is displayed
    QMutex mutex;

    /**
     * Tries to stop a Mission on a button
     * @param buttonIO pointer to a ButtonIO connection which is already opened
     * @return 1 for successfully stopped the mission
     * <br> -1 if the mission could not be stopped and the thread should end
     * <br> 0 if the mission could not be stopped and the thread should continue
     */
    int stopMissionOnButton(ButtonIO *buttonIO, uchar* SNum);

    /**
     * Tries to start a Mission on a iButton.
     * @param buttonIO pointer to a ButtonIO connection which is already opened
     * @return 1 for successfully started the mission on the button
     * <br>-1 if the mission could not be started and the thread should end
     * <br> 0 if the mission could not be started and the thread should continue
     */
    int startMissionOnButton(ButtonIO *buttonIO, uchar* SNum);

    /**
     * Aborts the thread and closes all the open databases. Only used to abort the thread
     * because of an Error.
     */
    void abort();

private slots:
	/**
	 * If this slot is called a Retry-Ignore-Abort question will be displayed to the user.
	 * @param question the question to ask.
	 * @param button in this variable the answer of the user will be stored
	 */
	void replyIgnoreQuestion(QString question, QMessageBox::StandardButton *button);

signals:
	/**
	 * Sends a signal to the AutoProgramUI telling it to toggle the caption
	 * of the start/stop button on the GUI. (e.g. if an error occurred and the
	 * thread has to be stopped)
	 */
	void buttonSwitch();

	/**
	 * Sends a signal to the AutoProgramUI informing it about the current
	 * ButtonNr of the autoProgram object, so it can be displayed on the GUI.
	 *
	 * @param ButtonNr	current ButtonNr
	 */
	void writeButtonNr(QString buttonNr);

	/**
	 * Sends a signal to the AutoProgramUI informing it about the current
	 * SerialNr of the autoProgram object, so it can be displayed on the GUI.
	 *
	 * @param serialNr	current SerialNr
	 */
	void writeSerialNr(QString serialNr);

    /**
     * Emits a text which describes the collect status. A stylesheet could be sent as well.
     * @param text the text which describes the collect status.
     * @param styleSheet a StyleSheet representation
     */
    void setStatus(QString text, QString styleSheet = "");

	/**
	 * Signal used to ask the user a Retry-Ignore-Abort Question.
	 * @param question the question
	 * @param button here will the answer be stored
	 */
	void askIgnoreQuestion(QString question, QMessageBox::StandardButton *button);

	void resetButtonNrSuffix();

};

#endif /* AUTOPROGRAMTHREAD_H_ */
