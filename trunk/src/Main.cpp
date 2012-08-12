#include "IAssist.h"

#include <QtGui>
#include <QApplication>
#include "database/DBConnection.h"
#include "ui/UserDialog.h"
#include "ui/DeploymentSelectUI.h"

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

    // Let the user select a deployment
    DeploymentSelectUI d;
    if(d.exec())
    {
        w.setDeploymentId(d.getSelectedDeploymentId());
    }
    else
    {
        return 1;
    }

    w.show();
    return a.exec();
}
