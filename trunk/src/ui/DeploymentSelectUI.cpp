//---------------------------------------------------------------------------
//  iAssist - Rapid deployment of Maxim iButton(c) miniature logger devices
//  Copyright (C) 2012 ETH Zurich, Computer Engineering and Networks
//      Laboratory, Matthias Keller <kellmatt at tik.ee.ethz.ch>
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
#include "DeploymentSelectUI.h"

using namespace std;

DeploymentSelectUI::DeploymentSelectUI(QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);

    connect(ui.btnLoadDeployment, SIGNAL(clicked()), this, SLOT(loadDeployment()));
	connect(ui.btnCreateDeployment, SIGNAL(clicked()), this, SLOT(createNewDeployment()));
	connect(ui.btnDeleteDeployment, SIGNAL(clicked()), this, SLOT(deleteDeployment()));
	connect(ui.btnQuit, SIGNAL(clicked()), this, SLOT(quitApplication()));

	initUI();
}

DeploymentSelectUI::~DeploymentSelectUI()
{

}

void DeploymentSelectUI::initUI()
{
    DBDeploymentTable db;

    ui.selDeploymentLoad->clear();
    ui.selDeploymentDelete->clear();

    ui.selDeploymentLoad->addItem("<Please select an item>");
    ui.selDeploymentDelete->addItem("<Please select an item>");

    db.open();
    deployments = db.getAllDeployments();
    db.close();

    for(int i=0; i <deployments.size(); i++)
    {
        ui.selDeploymentLoad->addItem(deployments.at(i).second);
        ui.selDeploymentDelete->addItem(deployments.at(i).second);
    }
}

void DeploymentSelectUI::loadDeployment()
{
    if(ui.selDeploymentLoad->currentIndex() == 0)
    {
        UserDialog::warning("Please select a deployment.");
        return;
    }

    // Get the selected item
    selectedDeploymentId = deployments.at(ui.selDeploymentLoad->currentIndex()-1).first;
    done(Accepted);
}

void DeploymentSelectUI::createNewDeployment()
{
    QString deploymentName = ui.txtDeploymentName->text();

    if(deploymentName.length() == 0)
    {
        UserDialog::warning("Please enter a deployment name.");
        return;
    }

    if(deploymentName.length() > 100)
    {
        UserDialog::warning("Please use at most 100 characters.");
        return;
    }

    if(UserDialog::question("Really create deployment: " + deploymentName + "?"))
    {
        DBDeploymentTable db;
        db.open();
        db.addDeployment(deploymentName, selectedDeploymentId);
        db.close();
        done(Accepted);
    }

}

void DeploymentSelectUI::deleteDeployment()
{
    if(ui.selDeploymentDelete->currentIndex() == 0)
    {
        UserDialog::warning("Please select a deployment.");
        return;
    }

    // Get the selected item
    int deploymentId = deployments.at(ui.selDeploymentDelete->currentIndex()-1).first;
    QString deploymentName = deployments.at(ui.selDeploymentDelete->currentIndex()-1).second;

    if(UserDialog::question("Do you really want to delete the deployment " + deploymentName + " including all related data?"))
    {
        DBDeploymentTable dbDeploy;
        DBAreaTable dbArea(deploymentId);
        DBButtonTable dbButton(deploymentId);
        DBMeasurementProfileTable dbMeasProf;
        DBMeasurementTable dbMeas;
        dbDeploy.open();
        dbArea.open();
        dbButton.open();
        dbMeasProf.open();
        dbMeas.open();

        QStringList areas = dbArea.getAllArea();
        for(int i=0; i<areas.size(); i++)
        {
            QString area = areas.at(i);
            QVector<int> buttons = dbButton.getButtonIdsByArea(area);
            for(int j=0; j<buttons.size(); j++)
            {
                int buttonId = buttons.at(j);
                QVector<MeasurementProfile> measProf = dbMeasProf.getProfilesByButtonID(buttonId);
                for(int k=0; k<measProf.size(); k++)
                {
                    dbMeas.deleteMeasurementsByMeasurementProfileID(measProf.at(k).MeasurementProfileID);
                }
                dbMeasProf.deleteProfileByButtonId(buttonId);
                dbButton.deleteButtonByButtonId(buttonId);
            }
            dbArea.deleteArea(area);
        }
        dbDeploy.deleteDeployment(deploymentId);

        dbDeploy.close();
        dbArea.close();
        dbButton.close();
        dbMeasProf.close();
        dbMeas.close();

        UserDialog::warning("Deployment has been deleted.");
        // Reset deployment combo boxes
        initUI();
    }
}

void DeploymentSelectUI::quitApplication()
{
    done(Rejected);
}

