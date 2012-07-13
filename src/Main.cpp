#include "IAssist.h"

#include <QtGui>
#include <QApplication>
#include "database/DBConnection.h"
#include "ui/UserDialog.h"
#include "ButtonIO.h"

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    QStringList args = a.arguments();

    //if (args.count() != 2)
    //{
    //    std::cerr << "Please specify the name of your database file, e.g., ./iAssist mydb" << endl;
    //    return 1;
    //}

    IAssist w;

    // Check if all the used directories are
    // existing, otherwise create them
    w.initDir();
    //w.prepareDb(args.at(1));

    // Check if database driver is installed
    if(!DBConnection::isSQLiteDriverInstalled())
    {
    	UserDialog::warning("QSQLite Driver is not installed. Please install it first.");
    	return 1;
    }

    w.show();
    return a.exec();
}
