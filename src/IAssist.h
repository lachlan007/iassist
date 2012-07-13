#ifndef IASSIST_H
#define IASSIST_H

#include <QtGui/QDialog>
#include "ui_IAssist.h"
#include "ui/AutoProgramUI.h"
#include "ui/CollectUI.h"
#include "ui/MissionParameterUI.h"
#include "database/DBManagementUI.h"
#include "SystemCommand.h"
#include "ui/DataPlotUI.h"


/**
 * This is the main window of the iAssist software. It starts all the different subprogramms
 * if a corresponding button is clicked.
 */

class IAssist : public QDialog
{
    Q_OBJECT

public:
    /**
     * Default constructor. It initializes the iAssist main window.
     */
    IAssist(QWidget *parent = 0);
    /**
     * Default deconstructor
     */
    ~IAssist();

    /**
     * Checks if the necessary directories exist. If they don't exist
     * they will be created.
     */
    void initDir();

    void prepareDb(QString dbFileName);

private slots:

	/**
	* Called if the Exit Button is clicked. It closes IAssist.
	*/
	void exitButtonClicked();

	/**
	* Called if the Remove DB Content Button is clicked. It starts the corresponding
	* Routine.
	*/
	void removeDbContClicked();

	/**
	* Called if the programming button is clicked. It starts the corresponding
	* routine.
	*/
	void programButtonClicked();

	/**
	* Called if the Collect Button is clicked. It starts the corresponding
	* routine.
	*/
	void collectButtonClicked();

	/**
	 * Called if the Mission Parameter File is clicked.
	 */
	void missionParameterButtonClicked();

	void dataPlotButtonClicked();

private:
	//! Link to the ui itself
    Ui::IAssistClass ui;

};

#endif // IASSIST_H
