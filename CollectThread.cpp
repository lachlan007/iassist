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
#include "CollectThread.h"

CollectThread::CollectThread(QObject *parent) {
	// Setup the values
	measurement.cleanMeasurement();
	measurementProfile.clearData();
	button.clearData();

	latestReadButtonID = "";

	connect(this, SIGNAL(askIgnoreQuestion(QString, QMessageBox::StandardButton*)),
			this, SLOT(replyIgnoreQuestion(QString, QMessageBox::StandardButton*)));

	connect(this, SIGNAL(displayUserMessage(QString)),
			this, SLOT(displayUserMessageSlt(QString)));

}

CollectThread::~CollectThread() {
}

void CollectThread::run()
{
	// set the Thread running variable to true
	running = true;

	// Open the databases
	dbMeasurement.open();
	dbButton.open();
	dbProfile.open();

	// Connect to the Button Reader
	if(!buttonIO.openPort())
	{
		emit this->setStatus("ERROR: Cannot connect to Reader.", STYLESHEETRED);
		this->abort();
		return;
	}

	while(running)
	{
		//==========================
		// Check if there is a button
		// attached and if a button is
		// selected in the UI
		//==========================
		if(buttonIO.isButtonConnected() && button.ButtonID!="")
		{
			// emit setStatus("iButton found. Checking it.", STYLESHEETYELLOW);
			// Wait till the iButton is really connected
			usleep(800000);

			// If a Button is found we check if its really a new button or if it's the wrong button
			// Case 1: it's a new button and its ID matches to the button we are looking for
			if(button.ButtonID == buttonIO.getButtonID() && latestReadButtonID!=button.ButtonID)
			{
				// emit setStatus("iButton found.", STYLESHEETYELLOW);
				// Verify that mission was in progress
				int res = this->verifyMissionRunningOnButton(&buttonIO);
				if(res==-1)
				{
					emit this->setStatus("ERROR: Could not detect a running mission on iButton: " + button.ButtonNr, STYLESHEETRED);
					this->abort();
					return;
				}

				// Stop the running mission, but only if res is not 0
				if(res!=0) res = this->stopMissionOnButton(&buttonIO);
				if(res==-1)
				{
					emit this->setStatus("ERROR: Could not stop the mission on iButton: " + button.ButtonNr, STYLESHEETRED);
					this->abort();
					return;
				}

				// Get the mission data
				emit this->setStatus("Reading mission data.", STYLESHEETYELLOW);
				res = this->getMissionDataOnButton(&buttonIO); //TODO
				if(res==-1)
				{
					emit this->setStatus("ERROR: Could not get mission data of iButton: " + button.ButtonNr, STYLESHEETRED);
					this->abort();
					return;
				}

				//===============================================
				// Get all the values for the database table
				//===============================================
				// to enter them to the database
				measurement.ButtonNr = button.ButtonNr;
				measurement.missionData = buttonIO.getSampleArray();
				measurement.size = buttonIO.getSampleSize();
				measurement.MeasurementProfileID = dbProfile.getLatestAddedProfileIDByButtonNr(button.ButtonNr);

				// Get all the values to update the measurement profile table
				// with the collecting time and so on.
				measurementProfile.ButtonNr = button.ButtonNr;
				measurementProfile.MeasurementProfileID = dbProfile.getLatestAddedProfileIDByButtonNr(button.ButtonNr);
				measurementProfile.CollectingTime = QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss");

				// Calculate time shift
				res = this->downloadTimeStampOnButton(&buttonIO);
				if(res==1)
				{
					measurementProfile.TimeShift = buttonIO.getTimeShift();
				}
				else if(res==-1)
				{
					this->abort();
					return;
				}


				//=================================================
				// Enter the measurement to the database and update
				// the measurement profile
				//=================================================
				emit this->setStatus("Writing mission data to database.", STYLESHEETYELLOW);
				if(!this->addCollectedDataToDB(&measurement, &measurementProfile, &dbMeasurement, &dbProfile))
				{
					this->abort();
					return;
				}

				// GSN export used during SenSys 2010 demo - not in production
				//CSVExport4GSN ex;
				//ex.appendMeasurementProfileToFile(QString::number(measurement.MeasurementProfileID));

				if(redistribute)
				{
				    emit this->setStatus("Starting reprogramming of iButton", STYLESHEETYELLOW);

				    // Clear the old data in the measurement profile
					measurementProfile.clearData();

					//========================================
					// restart a new session
					//========================================
					res = this->startMissionOnButton(&buttonIO);
					if(res==-1)
					{
						this->abort();
						return;
					}

					//========================================
					// Collect data to add a new Measurement
					// Profile to this iButton
					//========================================
					measurementProfile.ButtonNr = button.ButtonNr;
					measurementProfile.SessionNr = dbProfile.getLatestSessionNrByButtonNr(button.ButtonNr) + 1;
					measurementProfile.DistributingTime = QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss");
					measurementProfile.Resolution = buttonIO.getSamplingResolution();
					measurementProfile.SamplingRate = buttonIO.getSamplingRate();
					measurementProfile.SamplingStartTimeStamp = buttonIO.getSamplingStartTimeStamp();

					//========================================
					// Add the measurementProfile to the button
					//========================================
					if(!this->addRedistributeDataToDB(&measurementProfile, &dbProfile))
					{
					    this->abort();
						return;
					}
					emit setStatus("Successfully read iButton data and reprogrammed it.\nPlease select new iButton.", STYLESHEETGREEN);
				}
				else
				{
					emit setStatus("Successfully read data of iButton " + button.ButtonNr
							+ ". Please select new iButton.", STYLESHEETGREEN);
				}

				latestReadButtonID=button.ButtonID;

				// clean up the data, to be ready for new button
				measurement.cleanMeasurement();
				measurementProfile.clearData();
				button.clearData();
				sleep(1);
				emit buttonFinished();
				// FIXME QApplication::beep();
				sleep(5);
			}
			// Case 2: It's a the same button like before
			else if(latestReadButtonID==button.ButtonID)
			{
				emit setStatus("Please select NEW iButton.", STYLESHEETGREEN);
			}
			else if(buttonIO.getButtonID()==latestReadButtonID)
			{
				emit setStatus("Please insert NEW iButton: " + button.ButtonNr);
			}
			// Case 3: It's not the selected button
			else
			{
				emit setStatus("This iButton does not match the selected iButton: " + button.ButtonNr, STYLESHEETRED);
			}
		}
		else
		{
			emit setStatus("Please insert an iButton in the Reader.");
		}

		// Sleep 1 to avoid permanent polling
		if(running) {
			sleep(1);
		}

	}

	// Close the reader port
	buttonIO.closePort();

	// Close the databases
	dbMeasurement.close();
	dbProfile.close();
	dbButton.close();

}

void CollectThread::stop()
{
	running=false;
}

void CollectThread::abort()
{
	// Close the databases
	dbMeasurement.close();
	dbProfile.close();
	dbButton.close();
	buttonIO.closePort();
	emit aborted();
	emit this->setStatus("Stopped collecting iButtons.", STYLESHEETRED);
}

void CollectThread::setButton(ButtonData _button) //TODO protect with semaphore
{
	button = _button;
	if(button.ButtonNr == "")
	{
		emit setStatus("Please select an iButton to look for and press start.", "");
	}
	else
	{
		emit setStatus("Looking for iButton: " + _button.ButtonNr, "");
	}
}

void CollectThread::setRedistribute(bool state)
{
	redistribute = state;
}

void CollectThread::replyIgnoreQuestion(QString question, QMessageBox::StandardButton *button)
{
    QMutexLocker locker(&mutex);
	QMessageBox msgBox;
	msgBox.setIcon(QMessageBox::Question);
	msgBox.setStandardButtons(QMessageBox::Ignore | QMessageBox::Retry | QMessageBox::Abort);
	msgBox.setText(question);
	msgBox.setWindowTitle("Question:");
	int ret = msgBox.exec();
	*button = (QMessageBox::StandardButton)ret;
	waitCondition.wakeOne();
}

void CollectThread::displayUserMessageSlt(QString message)
{
    QMutexLocker locker(&mutex);
	QMessageBox msgBox;
	msgBox.setIcon(QMessageBox::Warning);
	msgBox.setStandardButtons(QMessageBox::Ok);
	msgBox.setText(message);
	msgBox.setWindowTitle("Warning:");
	msgBox.exec();
	waitCondition.wakeOne();
}

int CollectThread::verifyMissionRunningOnButton(ButtonIO *buttonIO)
{
	QMessageBox::StandardButton answer = QMessageBox::Retry;

	while((!buttonIO->isMissionInProgress()) && (answer == QMessageBox::Retry))
	{
		Log::write("WARNING: iButton " + button.ButtonNr + " was NOT running a mission.");
		QMutexLocker locker(&mutex);
		emit askIgnoreQuestion(QString("On this iButton was no mission in progress, therefore ")
				+ "\nit will not contain measurement data. Please choose what to do: "
				+ "\n\nRetry: \tfor checking the iButton again if a Mission was running"
				+ "\nIgnore: \tfor just go ahead and program the iButton now"
				+ "\nAbort: \tfor stop Collecting iButtons"
				, &answer);
		waitCondition.wait(&mutex);
	}

	if(answer == QMessageBox::Retry)
	{
		return 1;
	}
	else if(answer == QMessageBox::Ignore)
	{
		return 0;
	}
	else
	{
		return -1;
	}
}

int CollectThread::stopMissionOnButton(ButtonIO *buttonIO)
{
	QMessageBox::StandardButton answer = QMessageBox::Retry;

	while((!buttonIO->stopMissionOnButton()) && (answer == QMessageBox::Retry))
	{
		QMutexLocker locker(&mutex);
		emit askIgnoreQuestion("The mission on this iButton could not be stopped."
				+ QString("\nPlease choose what to do: ")
				+ "\n\nRetry: \ttry again stopping the Mission"
				+ "\nIgnore: \tfor just go ahead and try to read the measurements"
				+ "\nAbort: \tfor stop collecting iButtons"
				, &answer);
		waitCondition.wait(&mutex);
	}

	if(answer == QMessageBox::Retry)
	{
		return 1;
	}
	else if(answer == QMessageBox::Ignore)
	{
		return 0;
	}
	else
	{
		return -1;
	}
}

int CollectThread::startMissionOnButton(ButtonIO *buttonIO)
{
	QMessageBox::StandardButton answer = QMessageBox::Retry;

	while((!buttonIO->startMissionOnButton()) && (answer == QMessageBox::Retry))
	{
		QMutexLocker locker(&mutex);
		emit askIgnoreQuestion("The mission on this iButton could not be started."
				+ QString("\nPlease choose what to do: ")
				+ "\n\nRetry: \ttry again starting the Mission"
				+ "\nIgnore: \tfor just go ahead and don't program the iButton"
				+ "\nAbort: \tfor stop collecting iButtons"
				, &answer);
		waitCondition.wait(&mutex);
	}

	if(answer == QMessageBox::Retry)
	{
		return 1;
	}
	else if(answer == QMessageBox::Ignore)
	{
		return 0;
	}
	else
	{
		return -1;
	}
}

int CollectThread::getMissionDataOnButton(ButtonIO *buttonIO)
{
	QMessageBox::StandardButton answer = QMessageBox::Retry;

	while((!buttonIO->downloadMissionData()) && (answer == QMessageBox::Retry))
	{
		QMutexLocker locker(&mutex);
		emit askIgnoreQuestion("The Data could not be loaded from the iButton."
				+ QString("\nPlease choose what to do: ")
				+ "\n\nRetry: \ttry again getting the data"
				+ "\nIgnore: \tfor just go ahead"
				+ "\nAbort: \tfor stop collecting iButtons"
				, &answer);
		waitCondition.wait(&mutex);
	}

	if(answer == QMessageBox::Retry)
	{
		return 1;
	}
	else if(answer == QMessageBox::Ignore)
	{
		return 0;
	}
	else
	{
		return -1;
	}

}

int CollectThread::downloadTimeStampOnButton(ButtonIO *buttonIO)
{
	QMessageBox::StandardButton answer = QMessageBox::Retry;

	while((!buttonIO->downloadDeviceTimeStamp()) && (answer == QMessageBox::Retry))
	{
		QMutexLocker locker(&mutex);
		emit askIgnoreQuestion("The actual Time on the iButton could not be read."
				+ QString("\nPlease choose what to do: ")
				+ "\n\nRetry: \ttry again getting the time"
				+ "\nIgnore: \tfor just go ahead without saving the time"
				+ "\nAbort: \tfor stop collecting iButtons"
				, &answer);
		waitCondition.wait(&mutex);
	}

	if(answer == QMessageBox::Retry)
	{
		return 1;
	}
	else if(answer == QMessageBox::Ignore)
	{
		return 0;
	}
	else
	{
		return -1;
	}

}

bool CollectThread::addCollectedDataToDB(Measurement *_measurement, MeasurementProfile *_measurementProfile, DBMeasurementTable *_dbMeasurement, DBMeasurementProfileTable *_dbProfile)
{
	if(_measurement->size>0)
	{
		if(!_dbMeasurement->addMeasurement(*_measurement))
		{
			Log::writeError("collectThread: Cannot store measurement data to the database. Error Report: " + _dbMeasurement->getErrorReport());
			QMutexLocker locker(&mutex);
			emit displayUserMessage(QString("iAssist could not store the measurement data to")
									+ " the database. Collecting iButtons will abort.");
			emit this->setStatus("ERROR: Could not save new measurement data to database.", STYLESHEETRED);
			waitCondition.wait(&mutex);
			return false;
		}
	}

	if(!_dbProfile->updateProfile(*_measurementProfile))
	{
			Log::writeError("collectThread: Cannot update measurement Profile in the database. Error Report: " + _dbProfile->getErrorReport());
			QMutexLocker locker(&mutex);
			emit displayUserMessage(QString("iAssist could not update the measurement profile in")
									+ " the database. Collecting iButtons will abort.");
			emit this->setStatus("ERROR: Could not update the measurement profile in the database.", STYLESHEETRED);
			waitCondition.wait(&mutex);
			return false;
	}

	return true;
}

bool CollectThread::addRedistributeDataToDB(MeasurementProfile *_measurementProfile, DBMeasurementProfileTable *_dbProfile)
{
	if(!_dbProfile->addProfile(*_measurementProfile))
	{
		Log::writeError("collectThread: Cannot add measurement Profile in the database. Error Report: " + _dbProfile->getErrorReport());
		QMutexLocker locker(&mutex);
		emit displayUserMessage(QString("iAssist could not add the measurement profile in")
								+ " the database. Collecting iButtons will abort.");
		emit this->setStatus("ERROR: Could not save new measurement profile to database.", STYLESHEETRED);
		waitCondition.wait(&mutex);
		return false;
	}
	else
	{
		return true;
	}
}
