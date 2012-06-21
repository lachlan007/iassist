#ifndef COLLECTUI_H
#define COLLECTUI_H

#include <QtGui/QDialog>
#include "ui_CollectUI.h"
#include "DBAreaTable.h"
#include "DBButtonTable.h"
#include "CollectThread.h"

#define STYLESHEETRED 		"background-color: rgb(255, 181, 181);"
#define STYLESHEETYELLOW 	"background-color: rgb(255, 246, 207);"
#define STYLESHEETGREEN 	"background-color: rgb(190, 255, 196);"

/**
 * GUI Class which allows the user to control collecting iButtons.
 */
class CollectUI : public QDialog
{
    Q_OBJECT

public:
    /**
     * Default constructor
     */
    CollectUI(QWidget *parent = 0);
    /**
     * Default deconstructor
     */
    ~CollectUI();

private:
    //! The ui class. Direct access to the UI itself (QT-default)
    Ui::CollectUIClass ui;
    //! this object holds the data of the
    ButtonData actualButton;
    //! is the thread which handles collecting the buttons
    CollectThread *collectThr;

    /**
     * Call this to initialize the comboBox which shows the %Area
     */
    void initComboArea();
    /**
     * This is called every time the %Area in the comboBox changes. It
     * then shows the buttons belonging to this Footprint.
     */
    void updateComboButtonNr();

private slots:
	/**
	 * If the area in the comboBox changes this slot is called.
	 * This slot simply calls buttonNrChanged()
	 */
	void areaChanged();
	/**
	 * This slot changes the actually selected button in the
	 * thread which controls the collecting of the buttons. And it
	 * sends the photo list to the photoWidget.
	 */
	void buttonNrChanged();
	/**
	 * Closes the Collecting Window
	 * @param buttonNrSelected the selected ButtonNr
	 */
	void closeButtonClicked();
	/**
	 * Is called if the start Button is clicked. It starts the thread.
	 */
	void startButtonClicked();

public slots:
	/**
	 * Call this slot to set the Text of the status display
	 * @param text the text to display
	 * @param styleSheet chose a stylesheet string representation if the
	 * appearance should change. If it doesn't have to change chose an empty string ""
	 */
	void setStatusText(QString text, QString styleSheet);
	/**
	 * Selects the next button in the combo box
	 */
	void nextButtonClicked();
	/**
	 * Tells the Thread if redistribute is activated or not
	 * @param state toggle state
	 */
	void redistributeToggled(int state);
	/**
	 * Is called when the thread was aborted. To recover the UI
	 */
	void threadAborted();

};

#endif /* COLLECTUI_H */
