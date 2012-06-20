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
#include "AutoProgramThread.h"

AutoProgramThread::AutoProgramThread(QObject *parent){

    currentArea = "AA";
    threadEnabled = false;
    connect(this, SIGNAL(buttonSwitch()), parent, SLOT(buttonSwitch()));
    connect(this, SIGNAL(writeStatus(QString)), parent, SLOT(readStatus(QString)));
    connect(this, SIGNAL(writeButtonNr(QString)), parent, SLOT(readButtonNr(QString)));
    connect(this, SIGNAL(writeButtonID(QString)), parent, SLOT(readButtonID(QString)));
    connect(parent, SIGNAL(setArea(QString)), this, SLOT(setArea(QString)));
    connect(this, SIGNAL(setStatusColor(int)), parent, SLOT(setStatusColor(int)));
    connect(this, SIGNAL(askIgnoreQuestion(QString, QMessageBox::StandardButton*)),
            this, SLOT(replyIgnoreQuestion(QString, QMessageBox::StandardButton*)));

    dbButton = new DBButtonTable();
    dbArea = new DBAreaTable();
    dbProfile = new DBMeasurementProfileTable();

    // Creating an instance for iButton I/O
    iButtonCon = new ButtonIO();
}

AutoProgramThread::~AutoProgramThread(){
    delete dbButton;
    delete dbArea;
    delete dbProfile;
    delete iButtonCon;
}

void AutoProgramThread::run(){

    threadEnabled = true;

    // Check if the iButton reader is connected
    if (!iButtonCon->openPort())
    {
        this->abort();
        emit writeStatus("ERROR: No reader connected!");
        emit setStatusColor(0);
        return;
    }

    //===========================
    // Checking the ParameterFile
    //===========================
    if(!this->checkMissionParameterFile())
    {
        this->abort();
        emit writeStatus("Check Mission Parameter file."); // Parameter file is not correct
        emit setStatusColor(0);
        Log::writeError("autoProgramThread: The Mission Parameter file is missing or corrupt.");
        return;
    }

    // Creating and opening databases
    dbButton->open();
    dbArea->open();
    dbProfile->open();

    lastButtonID="noID"; // No buttonID so far

    while (threadEnabled)
    {
        QString cachedCurrentArea = currentArea;

        // Check if a Button is connected
        if(iButtonCon->isButtonConnected())
        {
            // to be sure the user has really plugged the button into the reader
            sleep(1);

            // Clear values (of last loop) in temporary button object
            button.clearData();

            // Get the Button's ID
            button.ButtonID = iButtonCon->getButtonID();

            // Check if it is really a new button
            if ((button.ButtonID == lastButtonID) || (button.ButtonID=="failed"))
            {
                emit writeStatus("Please insert NEW iButton.");
            }
            else
            {

                emit setStatusColor(1);
                emit writeStatus("Starting to program ...");

                //====================================
                // If a mission is running, stop it
                //====================================
                int res=1;
                if(iButtonCon->isMissionInProgress())
                {
                    emit setStatusColor(1);
                    emit writeStatus("Mission was running, stopping it.");
                    res = this->stopMissionOnButton(iButtonCon);
                    if(res==-1)
                    {
                        this->abort();
                        emit setStatusColor(0);
                        emit writeStatus("Cannot stop running mission.");
                        Log::writeError("AutoProgramThread: Cannot stop running mission: " + button.ButtonID);
                        return;
                    }
                }

                //=========================
                // Start a new mission
                //=========================
                res = this->startMissionOnButton(iButtonCon);
                if(res==-1)
                {
                    this->abort();
                    emit writeStatus("Programming iButton FAILED.");
                    emit setStatusColor(0);
                    Log::writeError("AutoProgramThread: The mission could not be started: " + button.ButtonID);
                    return;
                }
                // Save the ID of the latest programmed iButton
                lastButtonID = button.ButtonID;

                //===============================
                // Store settings to DB
                //===============================
                // Calculate next ButtonNr for the selected area and save to button
                QString ButtonNrRight = QString::number(dbButton->getLastAddedButtonNrInt(cachedCurrentArea) + 1);
                ButtonNrRight = ButtonNrRight.rightJustified(3,'0');
                button.ButtonNr = cachedCurrentArea + ButtonNrRight;

                // Save mission information for button
                profile.SamplingStartTimeStamp = iButtonCon->getSamplingStartTimeStamp();
                profile.SamplingRate = iButtonCon->getSamplingRate();
                profile.Resolution = iButtonCon->getSamplingResolution();
                profile.ButtonNr = button.ButtonNr;

                emit writeButtonNr(button.ButtonNr);
                emit writeButtonID(button.ButtonID);

                if (!(dbButton->addButton(button) && dbProfile->addProfile(profile)))
                {
                    emit writeStatus("Programming iButton FAILED.");
                    emit setStatusColor(0);
                    this->abort();
                    Log::writeError("autoProgram: Tried to call dbButton::addButton() or dbProfile::addProfile() - returned false.");
                    return;
                }

                // Check if the chosen Area is already existing
                if(!dbArea->isAreaExisting(cachedCurrentArea))
                {
                    // if not create it in the database
                    Area temparea;
                    temparea.AreaID = cachedCurrentArea;
                    if(!dbArea->addArea(temparea))
                    {
                        emit writeStatus("Programming iButton FAILED.");	// DB Error
                        emit setStatusColor(0);
                        this->abort();
                        Log::writeError("autoProgram: Tried to call dbArea::addArea() - returned false.");
                        return;
                    }
                }

                emit writeStatus("Programming iButton DONE.");
                emit setStatusColor(2);
                QApplication::beep();
                sleep(2);
            }
        }
        // There is no iButton in the Reader
        else
        {
            emit writeStatus("Please insert iButton.");
            emit setStatusColor(2);
        }

        msleep(1000);
    }

     // Close the databases
    dbButton->close();
    dbArea->close();
    dbProfile->close();

    // Terminate the iButton I/O
    iButtonCon->closePort();

    return;
}

void AutoProgramThread::abort()
{
    this->stop();
    emit writeButtonNr("");
    emit writeButtonID("");
    emit buttonSwitch();
    // Close the databases
    dbButton->close();
    dbArea->close();
    dbProfile->close();

    // Terminate the iButton I/O
    iButtonCon->closePort();
}

void AutoProgramThread::stop(){
    threadEnabled=false;
}

bool AutoProgramThread::checkMissionParameterFile(){
    int missionParameter[16];
    // Create a file handle
    QFile file(MISSION_PARAMETER_FILEPATH);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) // Open the file
    {
        Log::writeError("autoProgram: Can't open mission parameter file.");
        return false;
    }
    else
    {
        QTextStream in(&file);
        int lineCount;
        for(lineCount=0; lineCount<8; lineCount++)
        {
            if (!in.atEnd())
            {
                QString line = in.readLine();
                missionParameter[lineCount] = line.split("\t")[0].toInt();

            }
        }
        file.close();
    }

    // check if the settings value for sample rate and start delay
    // are inside their boundaries.
    if((missionParameter[1]<1) || (missionParameter[1]>982800))
    {
        Log::writeError("autoProgram: Sample rate in file is out of its limits.");
        return false;
    }
    else if((missionParameter[2]<0) || (missionParameter[2]>16777215))
    {
        Log::writeError("autoProgram: Start delay in file is out of its limits.");
        return false;
    }
    else if((missionParameter[7]<0) || (missionParameter[7])>365)
    {
        Log::writeError("autoProgram: Day delay in file is out of its limits.");
        return false;
    }

    // check for invalid boolean settings in "mission_start_parameters".
    for(int t=3;t<=6;t++)
    {
        if(!((missionParameter[t]==0) || (missionParameter[t]==1)))
        {
            Log::writeError("autoProgram: Boolean setting in file has a wrong value.");
            return false;
        }
    }
    return true;
}

void AutoProgramThread::setArea(QString area){
    currentArea = area;
    button.clearData();
    emit writeButtonNr("");
    emit writeButtonID("");
}

int AutoProgramThread::stopMissionOnButton(ButtonIO *buttonIO)
{
    QMessageBox::StandardButton answer = QMessageBox::Retry;

    while((!buttonIO->stopMissionOnButton()) && (answer == QMessageBox::Retry))
    {
        QMutexLocker locker(&mutex);
        emit askIgnoreQuestion("The mission on this iButton could not be stopped."
                + QString("\nPlease choose what to do: ")
                + "\n\nRetry: \ttry again stopping the Mission (recommended)"
                + "\nIgnore: \tfor just go ahead and try to start the new mission"
                + "\nAbort: \tfor stop programming iButtons"
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

void AutoProgramThread::replyIgnoreQuestion(QString question, QMessageBox::StandardButton *button)
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

int AutoProgramThread::startMissionOnButton(ButtonIO *buttonIO)
{
    QMessageBox::StandardButton answer = QMessageBox::Retry;

    while((!buttonIO->startMissionOnButton()) && (answer == QMessageBox::Retry))
    {
        QMutexLocker locker(&mutex);
        emit askIgnoreQuestion("The mission on this iButton could not be started."
                + QString("\nPlease choose what to do: ")
                + "\n\nRetry: \ttry again starting the Mission"
                + "\nIgnore: \tfor just go ahead and don't program this iButton"
                + "\nAbort: \tfor stop programming iButtons"
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
