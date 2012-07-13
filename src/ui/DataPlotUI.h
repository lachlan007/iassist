#ifndef DATAPLOTUI_H_
#define DATAPLOTUI_H_

#include <QtGui/QDialog>
#include "ui_DataPlotUI.h"
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include "../database/DBMeasurementProfileTable.h"
#include "../database/DBMeasurementTable.h"
#include <QtCore>

#define TEXT_NONE "none"

class DataPlotUI : public QDialog
{
    Q_OBJECT

public:
    /**
     * Default constructor
     */
    DataPlotUI(QWidget *parent = 0);
    /**
     * Default deconstructor
     */
    ~DataPlotUI();


private:
    //! Reference to the ui itself
    Ui::DataPlotUIClass ui;

    void initComboArea();

    QwtPlotCurve *curve1;

private slots:
    /* Regenerate data plot */
    void measurementProfileChanged();

    /* Closes the Window. */
    void closeButtonClicked();

};

#endif /* DATAPLOTUI_H_ */
