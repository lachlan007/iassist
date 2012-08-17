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

AutoProgramThread::AutoProgramThread(int deploymentId, QObject *parent){

    this->deploymentId = deploymentId;
    threadEnabled = false;
    connect(this, SIGNAL(buttonSwitch()), parent, SLOT(buttonSwitch()));
    connect(this, SIGNAL(setStatus(QString, QString)), parent, SLOT(setStatusText(QString, QString)));
    connect(this, SIGNAL(writeButtonNr(QString)), parent, SLOT(writeButtonNr(QString)));
    connect(this, SIGNAL(writeSerialNr(QString)), parent, SLOT(writeSerialNr(QString)));
    connect(parent, SIGNAL(setFootprintPrefix(int)), this, SLOT(setFootprintPrefix(int)));
    connect(parent, SIGNAL(setButtonNrSuffix(int)), this, SLOT(setButtonNrSuffix(int)));
    connect(this, SIGNAL(resetButtonNrSuffix()), parent, SLOT(resetButtonNrSuffixCombo()));
    connect(this, SIGNAL(askIgnoreQuestion(QString, QMessageBox::StandardButton*)),
            this, SLOT(replyIgnoreQuestion(QString, QMessageBox::StandardButton*)));

    dbButton = new DBButtonTable(deploymentId);
    dbFootprint = new DBFootprintTable(deploymentId);
    dbProfile = new DBMeasurementProfileTable();

    // Creating an instance for iButton I/O
    iButtonCon = new ButtonIO();

    // By default, the next ButtonNr is assigned using an auto-incremented value
    footprintPrefix = 1;
    buttonNrSuffix = -1;

}

AutoProgramThread::~AutoProgramThread(){
    delete dbButton;
    delete dbFootprint;
    delete dbProfile;
    delete iButtonCon;
}

void AutoProgramThread::run(){

    uchar SNum[8];
    MissionParameterFile& mp = MissionParameterFile::Instance();
    threadEnabled = true;

    //===========================
    // Checking the ParameterFile
    //===========================
    if(!mp.loadMissionParameters())
    {
        this->abort();
        emit setStatus("Check Mission Parameter file.", STYLESHEETRED); // Parameter file is not correct
        Log::writeError("autoProgramThread: The Mission Parameter file is missing or corrupt.");
        return;
    }

    // Check if the iButton reader is connected
    if (!iButtonCon->openPort())
    {
        this->abort();
        emit setStatus("ERROR: No reader connected!", STYLESHEETRED);
        return;
    }

    // Creating and opening databases
    dbButton->open();
    dbFootprint->open();
    dbProfile->open();

    lastSerialNr="noID"; // No buttonID so far

    while (threadEnabled)
    {
        // Check if mission start time is ok
        if(mp.getSetMissionStartTime() && mp.getMissionStartTime() < QDateTime::currentDateTime().toTime_t())
        {
            this->abort();
            emit setStatus("Mission start time is in the past.", STYLESHEETRED);
            Log::writeError("autoProgramThread: The mission start time is in the past.");
            return;
        }

        // Check if a Button is connected
        if(iButtonCon->getConnectedButton(&SNum[0]))
        {
            // to be sure the user has really plugged the button into the reader
            sleep(1);

            // Clear values (of last loop) in temporary button object
            button.clearData();

            // Get the Button's ID
            button.SerialNr = ButtonIO::buttonIDStr(&SNum[0]);

            // Check if it is really a new button
            if ((button.SerialNr == lastSerialNr) || (button.SerialNr=="failed"))
            {
                emit setStatus("Please insert NEW iButton.", STYLESHEETYELLOW);
            }
            else
            {

                emit setStatus("Starting to program ...", STYLESHEETYELLOW);

                //====================================
                // If a mission is running, stop it
                //====================================
                int res=1;
                if(iButtonCon->isMissionInProgress(&SNum[0]))
                {
                    emit setStatus("Mission was running, stopping it.", STYLESHEETYELLOW);
                    res = this->stopMissionOnButton(iButtonCon, &SNum[0]);
                    if(res==-1)
                    {
                        this->abort();
                        emit setStatus("Cannot stop running mission.", STYLESHEETRED);
                        Log::writeError("AutoProgramThread: Cannot stop running mission: " + button.SerialNr);
                        return;
                    }
                }

                //=========================
                // Start a new mission
                //=========================
                res = this->startMissionOnButton(iButtonCon, &SNum[0]);
                if(res==-1)
                {
                    this->abort();
                    emit setStatus("Programming iButton FAILED.", STYLESHEETRED);
                    Log::writeError("AutoProgramThread: The mission could not be started: " + button.SerialNr);
                    return;
                }
                // Save the ID of the latest programmed iButton
                lastSerialNr = button.SerialNr;

                //===============================
                // Store settings to DB
                //===============================
                // Calculate next ButtonNr for the selected area and save to button
                if(buttonNrSuffix == 0)
                {
                    button.ButtonNr = dbButton->getLastAddedButtonNr(footprintPrefix)+1;
                }
                else
                {
                    button.ButtonNr = 1000*footprintPrefix+buttonNrSuffix;
                }

                int insertId;
                if (!dbButton->addButton(button, insertId))
                {
                    emit setStatus("Programming iButton FAILED.", STYLESHEETRED);
                    this->abort();
                    Log::writeError("autoProgram: Tried to call dbButton::addButton() - returned false.");
                    return;
                }

                // Save mission information for button
                if(mp.getSetMissionStartTime())
                {
                    profile.SamplingStartTime = QDateTime::fromTime_t(mp.getMissionStartTime()).toString("dd.MM.yyyy hh:mm:ss");
                }
                else
                {
                    profile.SamplingStartTime = QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss");
                }
                profile.SamplingRate = mp.getSamplingRate();
                profile.HighResolutionEn = (ButtonIO::isThermoHygrochron(&SNum[0]) && mp.getHighTemperatureResolution()) ? 1 : 0;
                profile.ButtonId = insertId;
                profile.ProgrammingTime = QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss");

                emit writeButtonNr(QString::number(button.ButtonNr).rightJustified(6, '0').insert(3, ' '));
                emit writeSerialNr(button.SerialNr);

                if (!dbProfile->addProfile(profile))
                {
                    emit setStatus("Programming iButton FAILED.", STYLESHEETRED);
                    this->abort();
                    Log::writeError("autoProgram: Tried to call dbProfile::addProfile() - returned false.");
                    return;
                }

                // Check if the chosen Area is already existing
                if(!dbFootprint->isFootprintExisting(footprintPrefix))
                {
                    // if not create it in the database
                    if(!dbFootprint->addFootprint(footprintPrefix))
                    {
                        emit setStatus("Programming iButton FAILED.", STYLESHEETRED);	// DB Error
                        this->abort();
                        Log::writeError("autoProgram: Tried to call dbArea::addArea() - returned false.");
                        return;
                    }
                }

                emit setStatus("Programming iButton DONE.", STYLESHEETGREEN);
                emit resetButtonNrSuffix();
                QApplication::beep();
                sleep(2);
            }
        }
        // There is no iButton in the Reader
        else
        {
            emit setStatus("Please insert iButton.", STYLESHEETYELLOW);
        }

        msleep(1000);
    }

     // Close the databases
    dbButton->close();
    dbFootprint->close();
    dbProfile->close();

    // Terminate the iButton I/O
    iButtonCon->closePort();

    return;
}

void AutoProgramThread::abort()
{
    this->stop();
    emit writeButtonNr("");
    emit writeSerialNr("");
    emit resetButtonNrSuffix();
    emit buttonSwitch();
    // Close the databases
    dbButton->close();
    dbFootprint->close();
    dbProfile->close();

    // Terminate the iButton I/O
    iButtonCon->closePort();
}

void AutoProgramThread::stop(){
    threadEnabled=false;
}

void AutoProgramThread::setFootprintPrefix(int prefix)
{
    footprintPrefix = prefix;
    button.clearData();
}

void AutoProgramThread::setButtonNrSuffix(int suffix)
{
    buttonNrSuffix = suffix;
    button.clearData();
}

int AutoProgramThread::stopMissionOnButton(ButtonIO *buttonIO, uchar* SNum)
{
    QMessageBox::StandardButton answer = QMessageBox::Retry;

    while((!buttonIO->stopMission(SNum)) && (answer == QMessageBox::Retry))
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

int AutoProgramThread::startMissionOnButton(ButtonIO *buttonIO, uchar* SNum)
{
    QMessageBox::StandardButton answer = QMessageBox::Retry;

    while((!buttonIO->startMission(SNum)) && (answer == QMessageBox::Retry))
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
