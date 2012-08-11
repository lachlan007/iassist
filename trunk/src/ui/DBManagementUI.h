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
#ifndef DBMANAGEMENTUI_H
#define DBMANAGEMENTUI_H

#include <QtGui/QDialog>
#include <QFile>
#include "ui_DBManagementUI.h"
#include "../database/DBAreaTable.h"
#include "../database/DBButtonTable.h"
#include "../database/DBMeasurementTable.h"
#include "../database/DBMeasurementProfileTable.h"
#include "UserDialog.h"


#define STYLESHEETWHITE		"background-color: rgb(255, 255, 255);"
#define STYLESHEETRED 		"background-color: rgb(255, 181, 181);"
#define STYLESHEETYELLOW 	"background-color: rgb(255, 246, 207);"
#define STYLESHEETGREEN 	"background-color: rgb(190, 255, 196);"
#define FOLDERPATHLENGTH	6
// currently, the folder path './img/' has the length of 6 characters.


/**
 * Allows deleting data from the HDD and the Database. Used to remove data and all the related information.
 */
class DBManagementUI : public QDialog
{
    Q_OBJECT

public:
    /**
     * Default constructor
     */
    DBManagementUI(int deploymentId, QWidget *parent = 0);
    /**
     * Default deconstructor
     */
    ~DBManagementUI();


private:
    //! Reference to the ui itself
    Ui::DBManagementUIClass ui;
    //! Holds the actual selected iButton
    ButtonData actualButton;
    //! holds the buttonNr of the current Button
    QString ButtonNr;
    //! stores the reports which are created during an action
    QString report;

    /**
     * Initializes the ComboBox which displays the different Areas (Footprints)
     */
    void initComboArea();
    /**
     * Updates the contents of the ComboBox which displays the iButtons to a certain Footprint.
     */
    void updateComboButtonNr();
    /**
     * Sets the status Text.
     * @param text Text to display
     * @param styleSheet Stylesheet to change the status Text display's appearance
     */
    void setStatusText(QString text, QString styleSheet);
    /**
     * Appends a report to the report string.
     * @param rep the report to append to the report string
     */
    void appendReport(QString rep);
    /**
     * Clears the report string.
     */
    void clearReport();
    /**
     * Returns the whole report.
     * @return the report string
     */
    QString getReport();
    /**
     * Deletes an iButton and all it's related information from the database and the HDD. Failures are stored in report.
     * @param buttonNr The number of the chosen iButton
     * @return true if successfully deleted, false otherwise.
     */
    bool deleteButton(QString buttonNr);
    /**
     * Deletes a Footprint and all it's related information from the database and the HDD.
     * Failures are stored in the report.
     * @param area the ID of the footprint to delete
     * @return true if successfully deleted, false otherwise
     */
    bool deleteArea(QString area);



private slots:
	/**
	 * Call this if the ComboBox content has changed. It updates the iButton Combo was well and
	 * changes the UI appearance.
	 */
	void areaChanged();
	/**
	 * Displays information to this iButton and changes the UI appearance.
	 */
	void buttonNrChanged();
	/**
	 * Closes the Window.
	 */
	void closeButtonClicked();
	/**
	 * Call this if delete Footprint Button was clicked. It starts deleting the selected footprint.
	 */
    void deleteAreaClicked();
    /**
     * Call this if delete iButton was clicked. It starts deleting the selected iButton
     */
    void deleteButtonClicked();


public slots:
	/**
	 * This slot increases the selected iButton by one
	 */
	void nextButtonPressed();

};

#endif // DBMANAGEMENTUI_H
