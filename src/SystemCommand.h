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
#ifndef SYSTEMCOMMAND_H_
#define SYSTEMCOMMAND_H_

#include <QString>

/**
 * This class supports writing to the shell and receive the shell's answer.
 */

class SystemCommand {
public:
	/**
	 * Default constructor
	 */
	SystemCommand();
	/**
	 * Default deconstructor
	 */
	virtual ~SystemCommand();

    /**
     * Executes a system command and returns the command line answer.
     * @param cmd the system command
     * @return
     */
	static QString getStringFromSystemCommand(QString cmd);
};

#endif /* SYSTEMCOMMAND_H_ */
