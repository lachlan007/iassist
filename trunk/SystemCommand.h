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
