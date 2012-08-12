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
#ifndef DATAPLOTUI_H_
#define DATAPLOTUI_H_

#include <QtGui/QDialog>
#include "ui_DataPlotUI.h"
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include "../database/DBMeasurementProfileTable.h"
#include "../database/DBMeasurementTable.h"
#include "../database/DBButtonTable.h"
#include <QtCore>

#define TEXT_NONE "none"

class DataPlotUI : public QDialog
{
    Q_OBJECT

public:
    /**
     * Default constructor
     */
    DataPlotUI(int deploymentId, QWidget *parent = 0);
    /**
     * Default deconstructor
     */
    ~DataPlotUI();


private:
    //! Reference to the ui itself
    Ui::DataPlotUIClass ui;

    void initComboArea();

    QwtPlotCurve *curve1;

    int deploymentId;

private slots:
    /* Regenerate data plot */
    void measurementProfileChanged();

    /* Closes the Window. */
    void closeButtonClicked();

};

#endif /* DATAPLOTUI_H_ */
