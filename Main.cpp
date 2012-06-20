#include "IAssist.h"

#include <QtGui>
#include <QApplication>
#include "DBConnection.h"
#include "UserDialog.h"
#include "ButtonIO.h"

int main(int argc, char *argv[])
{

	/*ButtonIO Button1;

	Button1.openPort();
	Button1.isButtonConnected();

	printf(" %x\n", Button1.getRegisterContent(0x211));
	printf(" %x\n", Button1.getRegisterContent(0x213));
	printf(" %x\n", Button1.getRegisterContent(0x215));
	printf(" %x\n", Button1.getRegisterContent(0x226));

	Button1.closePort();*/


    QApplication a(argc, argv);

    IAssist w;

    // Check if all the needed installations
    if(SystemCommand::getStringFromSystemCommand("which gpsd")=="")
    {
    	UserDialog::warning("iAssist will start without GPS support. GPSD is not installed.");
    }

    // Check if all the used directories are
    // existing, otherwise create them
    w.initDir();

    // Check if database driver is installed
    if(!DBConnection::isSQLiteDriverInstalled())
    {
    	UserDialog::warning("QSQLite Driver is not installed. Please install it first.");
    	return 1;
    }

    w.show();
    return a.exec();
}
