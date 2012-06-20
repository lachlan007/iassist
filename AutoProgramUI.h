#ifndef AUTOPROGRAMUI_H
#define AUTOPROGRAMUI_H

#include <QtGui/QDialog>
#include "ui_AutoProgramUI.h"
#include "AutoProgramThread.h"

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
    AutoProgramUI(QWidget *parent = 0);
    /**
     * This is a default-destructor. Nothing to do here.
     */
    ~AutoProgramUI();

public slots:
	/**
	 * Receives the desired color as a signal and sets the background of the
	 * status field to that color.<br>
	 * <br>
	 * 0	=	red<br>
	 * 1	=	yellow<br>
	 * 2	=	green<br>
	 *
	 * @param color	desired background color
	 */
	void setStatusColor(int color);
	/**
	 *  Recieves a signal when the Start/Stop button has been pushed.
	 *  Toggles the Button between Start <-> Stop and starts/stops the
	 *  AutoProgramThread thread.
	 */
	void buttonSwitch();
	/**
	 * Receives and passes the status from AutoProgramThread to the GUI.
	 *
	 * @param status	received status
	 */
	void readStatus(QString status);
	/**
	 * Receives and passes the current ButtonNr from AutoProgramThread
	 * to the GUI.
	 *
	 * @param ButtonNr	received ButtonNr
	 */
	void readButtonNr(QString ButtonNr);
	/**
	 * Receives and passes the current ButtonID from AutoProgramThread
	 * to the GUI.
	 *
	 * @param ButtonID	received ButtonID
	 */
	void readButtonID(QString ButtonID);


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
	 * Keeps track of the currently set area.
	 */
	QString currentArea;

private slots:
	/**
	 * Receives a sinal when the close-button has been pushed.
	 * Closes the window.
	 */
	void closeClicked();
	/**
	 * Receives a signal when the Area-Increase-Button has been pushed.
	 * Increases the area displayed on the GUI by 1 and informs AutoDistributeThread
	 * about the new area.
	 */
	void incArea();
	/**
	 * Receives a signal when the Area-Decrease-Button has been pushed.
	 * Decreses the area displayed on the GUI by 1 and informs AutoDistributeThread
	 * about the new area.
	 */
	void decArea();
	/**
	 * Receives a signal when the Area-Increase-More-Button has been pushed.
	 * Increases the area displayed on the GUI by 26 (i.e. first letter changes) and informs AutoDistributeThread
	 * about the new area.
	 */
	void incAreaMore();
	/**
	 * Receives a signal when the Area-Decrease-More-Button has been pushed.
	 * Decreses the area displayed on the GUI by 26 (i.e. first letter changes) and informs AutoDistributeThread
	 * about the new area.
	 */
	void decAreaMore();

signals:
	/**
	 * Sends a signal to the AutoProgramThread to inform it about the
	 * changed currentArea.
	 *
	 * @param area	new area
	 */
	void setArea(QString area);


};

#endif /* AUTOPROGRAMUI_H */