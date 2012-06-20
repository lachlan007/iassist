#include "IAssist.h"

#include <QtGui>
#include <QApplication>
#include "DBConnection.h"
#include "UserDialog.h"
#include "ButtonIO.h"

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);

    IAssist w;

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
