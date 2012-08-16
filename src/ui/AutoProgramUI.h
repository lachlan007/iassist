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
#ifndef AUTOPROGRAMUI_H
#define AUTOPROGRAMUI_H

#include <QtGui/QDialog>
#include <QVector>
#include "ui_AutoProgramUI.h"
#include "../AutoProgramThread.h"
#include "../database/DBButtonTable.h"
#include "UserDialog.h"

#define AUTOINCREMENTTEXT   "<auto>"
#define STYLESHEETRED   "background-color: rgb(255, 181, 181);" // red
#define STYLESHEETYELLOW "background-color: rgb(255, 246, 207);" // yellow
#define STYLESHEETGREEN "background-color: rgb(190, 255, 196);" // green

/**
 * This is the GUI class for the automatic programming of iButtons at home.
 */
class AutoProgramUI : public QDialog
{
    Q_OBJECT

public:
    /**
     * This constructor sets the currentArea to "AA", connects all SLOTS and SIGNALS
     * and creates an instance of AutoProgramThread, which is not started yet.
     *
     * @oaram parent	reference to parent
     */
    AutoProgramUI(int deploymentId, QWidget *parent = 0);
    /**
     * This is a default-destructor. Nothing to do here.
     */
    ~AutoProgramUI();

public slots:
    /**
     * Call this slot to set the Text of the status display
     * @param text the text to display
     * @param styleSheet chose a stylesheet string representation if the
     * appearance should change. If it doesn't have to change chose an empty string ""
     */
    void setStatusText(QString text, QString styleSheet = "");

    /**
	 *  Recieves a signal when the Start/Stop button has been pushed.
	 *  Toggles the Button between Start <-> Stop and starts/stops the
	 *  AutoProgramThread thread.
	 */
	void buttonSwitch();

	/**
	 * Receives and passes the current ButtonNr from AutoProgramThread
	 * to the GUI.
	 *
	 * @param ButtonNr	received ButtonNr
	 */
	void writeButtonNr(QString ButtonNr);

	/**
	 * Receives and passes the current SerialNr from AutoProgramThread
	 * to the GUI.
	 *
	 * @param SerialNr	received SerialNr
	 */
	void writeSerialNr(QString SerialNr);

	void resetButtonNrSuffixCombo();

private:
    Ui::AutoProgramUIClass ui;
	/**
	 * Pointer to the thread which controls automatic programming of the iButtons.
	 */
	AutoProgramThread *autoProg;
	/**
	 * Tells whether or not the AutoProgramThread is running.
	 * This is needed to know if the Button should spell 'Stop' or 'Start'.
	 */
	bool autoProgramRunning;

	/**
	 * Keeps track of the currently set ButtonNr prefix
	 */
	int currentButtonNrPrefixIdx;

	void setupPrefixSelect();

	void setupSuffixSelect(int footprintPrefix);

	DBButtonTable* dbButton;

private slots:
	/**
	 * Receives a signal when the close-button has been pushed.
	 * Closes the window.
	 */
	void closeClicked();

	void footprintPrefixChanged(int selected);

	void buttonNrSuffixChanged(QString selected);

signals:
	/**
	 * Sends a signal to the AutoProgramThread
	 */
	void setFootprintPrefix(int prefix);

	void setButtonNrSuffix(int suffix);


};

#endif /* AUTOPROGRAMUI_H */
