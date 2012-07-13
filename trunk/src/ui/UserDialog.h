#ifndef USERDIALOG_H_
#define USERDIALOG_H_

#include <QString>
#include <QMessageBox>

/**
 * UserDialog is a class which supports static functions to show
 * a Dialog to the user. This functions must be called by the GUI-Thread.
 */
class UserDialog {
public:
	/**
	 * Default constructor
	 */
	UserDialog();
	/**
	 * Default decontructor
	 */
	virtual ~UserDialog();

	/**
	 * Shows a warning window to the user
	 * @param text the text to display to the user
	 */
	static void warning(QString text);

	/**
	 * Shows a warning window to the user
	 * @param text the text to display to the user
	 * @param detailedInformation detailed information to display if 'Details...' is clicked in the window
	 */
	static void warning(QString text, QString detailedInformation);

	/**
	 * Shows a question window to the user
	 * @param question the question to ask
	 * @return true for yes and fals for no
	 */
	static bool question(QString question);
};

#endif /* USERDIALOG_H_ */
