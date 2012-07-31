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
#include "database/DBAreaTable.h"
#include "database/DBMeasurementProfileTable.h"
#include "MeasurementProfile.h"
#include "ButtonData.h"
#include "ButtonIO.h"
#include <QMutex>
#include <QWaitCondition>
#include <QMessageBox>
#include <QtCore>
#include "MissionParameterFile.h"

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
	AutoProgramThread(QObject *parent);
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
	 * @oaram area	new area
	 */
	void setFootprint(QString footprintID);

private:
	/**
	 * Needed by the stop() function, to tell the while loop it should finish
	 * the current loop and then stop.
	 */
	bool threadEnabled;
	/**
	 * Keeps track of the current area. This value must be synchronized between
	 * AutoProgramThread and its parent AutoProgramUI. AutoProgramThread updates
	 * it via the SLOT setArea().
	 */
	QString currentFootprint;
	/**
	 * Manages the database connection to the button table.
	 */
	DBButtonTable *dbButton;
	/**
	 * Manages the database connection to the area table.
	 */
	DBAreaTable *dbArea;
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
	QString lastButtonID;
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
	 * Sends a signal to the AutoProgramUI informing it about the status of
	 * the autoProgram object, so it can be displayed on the GUI.
	 *
	 * @param status	current status
	 */
	void writeStatus(QString status);
	/**
	 * Sends a signal to the AutoProgramUI informing it about the current
	 * ButtonNr of the autoProgram object, so it can be displayed on the GUI.
	 *
	 * @param ButtonNr	current ButtonNr
	 */
	void writeButtonNr(QString ButtonNr);
	/**
	 * Sends a signal to the AutoProgramUI informing it about the current
	 * ButtonID of the autoProgram object, so it can be displayed on the GUI.
	 *
	 * @param ButtonID	current ButtonID
	 */
	void writeButtonID(QString ButtonID);
	/**
	 * Sends a signal to the AutoProgramUI to change the background color of
	 * the status field.<br>
	 * <br>
	 * 0	=	red<br>
	 * 1	=	yellow<br>
	 * 2	=	green<br>
	 *
	 * @param ButtonID	current ButtonID
	 */
	void setStatusColor(int);

	/**
	 * Signal used to ask the user a Retry-Ignore-Abort Question.
	 * @param question the question
	 * @param button here will the answer be stored
	 */
	void askIgnoreQuestion(QString question, QMessageBox::StandardButton *button);

};

#endif /* AUTOPROGRAMTHREAD_H_ */
