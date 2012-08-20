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
#ifndef COLLECTTHREAD_H_
#define COLLECTTHREAD_H_

#include <QThread>
#include <QObject>
#include <QWaitCondition>
#include <QMutex>
#include <QtCore>
#include "ButtonData.h"
#include "ButtonIO.h"
#include "Measurement.h"
#include "database/DBButtonTable.h"
#include "database/DBMeasurementTable.h"
#include "database/DBMeasurementProfileTable.h"
#include "MeasurementProfile.h"
#include "QMessageBox"
#include "MissionParameterFile.h"

#define STYLESHEETRED 		"background-color: rgb(255, 181, 181);"
#define STYLESHEETYELLOW 	"background-color: rgb(255, 246, 207);"
#define STYLESHEETGREEN 	"background-color: rgb(190, 255, 196);"
#define STYLESHEETWHITE		"background-color: rgb(255, 255, 255);"

/**
 * This thread manages the whole collecting of the iButtons. It sends signals to know what happens
 * and has slots and functions to control it's behaviour. It's possible to simply collect
 * iButtons and store their data in the database or to directly redistribute them after the data are read.
 */

class CollectThread : public QThread{
	Q_OBJECT
public:
	/**
	 * Default constructor
	 */
	CollectThread(int deploymentId, QObject *parent);
	/**
	 * Default deconstructor
	 */
	virtual ~CollectThread();

	/**
	 * Is running if the Thread is started.
	 * <br>It checkes always for a new button attached.
	 * <br>  If one is attached it does:
	 * <br>  - check if the right button is attached
	 * <br>  - read the measurement data
	 * <br>  - reprogramms it or just stops the mission
	 */
	void run();

	/**
	 * Is used to set the button, which should actually be attached.
	 * @param _button the button which should be attached next.
	 */
	void setButton(ButtonData button);

	/**
	 * Switch between redistribute or simply collect the buttons.
	 * @param state chose true for redistribute and false for only collect and stop mission
	 */
	void setRedistribute(bool state);

	/**
	 * Call this to stop the thread.
	 */
	void stop();

signals:
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

	/**
	 * Signal used to display a message
	 * @param message the message to display
	 * @param button the answer of the user will be stored in button
	 */
	void displayUserMessage(QString message, QMessageBox::StandardButton *button=NULL);

	/**
	 * Emitted if the thread had to abort because of an error or a non defined situation.
	 */
	void aborted();

	/**
	 * Emitted if the thread is ready for a new iButton
	 */
	void buttonFinished();


private:
	int deploymentId;

	//! is true if the thread is running, is set to false if the thread should stop
	bool running;
	//! is used to store all the button information
	ButtonData button;
	//! is used to store a measurement
	Measurement measurement;
	//! is used to store a measurement profile
	MeasurementProfile measurementProfile;
	//! is ButtonNr of the button which is added as latest. Used to check if there's
	//! a new button attached
	QString latestReadButtonID;
	//! is used to know if a button should be redistributed or only collected
	bool redistribute;
	//! manages the connection to the %MeasurementProfile table in the database
	DBMeasurementProfileTable* dbProfile;
	//! manages the connection to the iButton table in the database
	DBButtonTable* dbButton;
	//! manages the connection to the %Measurement table in the database
	DBMeasurementTable* dbMeasurement;
	//! used to communicate with the iButton Reader
	ButtonIO iButtonCon;
	//! used to lock and stop the thread while a dialog box is displayed
    QWaitCondition waitCondition;
    //! used to lock and stop the thread while a dialog box is displayed
    QMutex mutex;

    /**
     * Checks if a Mission is running on the Button.
     * @param buttonIO pointer to an ButtonIO connection which is already opened
     * @return 1 for successfully found an running Mission
     * <br> -1 if no mission is running or an error appeared and the thread should end
     * <br> 0 if no mission is running or an error appeared and the thread should continue
     */
    int verifyMissionRunningOnButton(ButtonIO *buttonIO, uchar* SNum);

    /**
     * Tries to stop a Mission on a button
     * @param buttonIO pointer to a ButtonIO connection which is already opened
     * @return 1 for successfully stopped the mission
     * <br> -1 if the mission could not be stopped and the thread should end
     * <br> 0 if the mission could not be stopped and the thread should continue
     */
    int stopMissionOnButton(ButtonIO *buttonIO, uchar* SNum);

    /**
     * Tries to get the Mission Data of a button
     * @param buttonIO pointer to a ButtonIO connection which is already opened
     * @return 1 for successfully read the mission data
     * <br> -1 if the mission data could not be read and the thread should end
     * <br> 0 if the mission data could not be read and the thread should continue
     */
    int getMissionDataFromButton(ButtonIO *buttonIO, uchar* SNum, MissionData& data);

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

    /**
     * Tries to add the collected data to the database. This means: add the measurement data
     * to the datase and update the measurement profile.
     * @param _measurement pointer to the measurement of the button
     * @param _measurementProfile pointer to the measurement Profile
     * @param _dbMeasurement pointer to the measurement Database
     * @param _dbProfile pointer to the measurement profile Database
     * @return true for successfully entered, false for not entered (thread has to abort)
     */
    bool addCollectedDataToDB(Measurement *_measurement, MeasurementProfile *_measurementProfile, DBMeasurementTable *_dbMeasurement, DBMeasurementProfileTable *_dbProfile);

    /**
     * Tries to add the measurement profile which holds the data of the new measurement session
     * (if redistributed).
     * @param _measurementProfile pointer to the measurement profile
     * @param _dbProfile pointer to the measurement profile database
     * @return true for successfully entered, false for not entered (thread has to abort)
     */
    bool addRedistributeDataToDB(MeasurementProfile *_measurementProfile, DBMeasurementProfileTable *_dbProfile);


private slots:
	/**
	 * If this slot is called a Retry-Ignore-Abort question will be displayed to the user.
	 * @param question the question to ask.
	 * @param button in this variable the answer of the user will be stored
	 */
	void replyIgnoreQuestion(QString question, QMessageBox::StandardButton *button);
	/**
	 * If this slot is called a simple message will be displayed to the user.
	 * @param message the message to display
	 */
	void displayUserMessageSlt(QString message);

};

#endif /* COLLECTTHREAD_H_ */
