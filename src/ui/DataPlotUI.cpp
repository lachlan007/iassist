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
#include "DataPlotUI.h"


DataPlotUI::DataPlotUI(int deploymentId, QWidget *parent)
: QDialog(parent) {
    ui.setupUi(this);

    this->deploymentId = deploymentId;

    // init combo boxes
    initComboArea();

    // connect signals and slots
    connect(ui.cbMeasurementProfileID, SIGNAL(currentIndexChanged(QString)), this, SLOT(measurementProfileChanged()));
    connect(ui.btnClose, SIGNAL(clicked()), this, SLOT(closeButtonClicked()));

    //ui.plData->setTitle("Data");
    ui.plData->setAxisTitle(QwtPlot::xBottom, "Time");
    ui.plData->setAxisTitle(QwtPlot::yLeft, "Temperature [Celsius]");
    ui.plData->setCanvasBackground(Qt::white);

    curve1 = new QwtPlotCurve("Measurements");
    curve1->attach(ui.plData);
    curve1->setPen(QPen(Qt::black, 1, Qt::SolidLine));

    ui.plData->replot();
}

DataPlotUI::~DataPlotUI() {
    delete curve1;
}

void DataPlotUI::initComboArea()
{
    ui.cbMeasurementProfileID->clear();

    DBButtonTable dbBut(deploymentId);
    DBMeasurementProfileTable dbMeas;
    dbMeas.open();
    dbBut.open();
    QStringList listProfiles;
    QStringList listNone;
    QVector<MeasurementProfile> profileVector = dbMeas.getFinishedMeasurementProfiles();
    listNone.append(TEXT_NONE);

    for (int i = 0; i < profileVector.size(); ++i) {
        ButtonData b = dbBut.getButtonByButtonId(profileVector.at(i).ButtonId);
        if(b.ButtonNr == "")
        {
            // Button belongs to other deployment
            continue;
        }
        listProfiles.append("ProfileID: " + QString::number(profileVector.at(i).MeasurementProfileID)
                        + ", ButtonNr: " + b.ButtonNr
                        + ", SessionNr: " + QString::number(profileVector.at(i).SessionNr));
    }
    ui.cbMeasurementProfileID->addItems(listNone + listProfiles);
    dbMeas.close();
    dbBut.close();

}

void DataPlotUI::closeButtonClicked()
{
    this->close();
}

void DataPlotUI::measurementProfileChanged()
{
    DBMeasurementTable db;
    double *y;

    if(ui.cbMeasurementProfileID->currentText() == TEXT_NONE) {
        return;
    }

    db.open();

    QStringList tmp = ui.cbMeasurementProfileID->currentText().split(' ');
    QString measurementProfileID = tmp.at(1);
    measurementProfileID.chop(1);

    int numValues = db.getMeasurementsByProfileID(measurementProfileID, &y);

    double *x = new double[numValues];
    for(int i = 0; i < numValues; i++) {
        x[i] = i;
    }

    curve1->setSamples(x, y, numValues);
    ui.plData->replot();

    delete x;
    delete y;

}
