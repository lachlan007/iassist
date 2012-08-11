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
