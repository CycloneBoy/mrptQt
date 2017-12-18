#ifndef PROPERTYEDITOR_H
#define PROPERTYEDITOR_H

#include <QDialog>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <iostream>
#include <QDebug>
#include <QDir>
#include <QColorDialog>
#include <iostream>
#include "QtGL.hpp"
#include <mrpt/utils/TColor.h>
#include <mrpt/math/lightweight_geom_data.h>

namespace Ui {
class PropertyEditor;
}

class PropertyEditor : public QDialog
{
    Q_OBJECT

public:
    explicit PropertyEditor(QWidget *parent = 0);
    ~PropertyEditor();
    void setColor(QLabel *label, QColor c);
    void enableApplyButton(bool);

private slots:
    void on_azimuthbox_valueChanged(double);

    void on_elevationbox_valueChanged(double arg1);

    void on_zoombox_valueChanged(double arg1);

    void on_xmingridplane_valueChanged(double arg1);

    void on_xmaxgridplane_valueChanged(double arg1);

    void on_ymingridplane_valueChanged(double arg1);

    void on_ymaxgridplane_valueChanged(double arg1);

    void on_selectcolorgridplane_clicked();

    void on_colorgridmap_clicked();

    void on_buttonBox_accepted();

    void on_buttonBox_clicked(QAbstractButton *button);

    void on_linewidthaxis_valueChanged(double arg1);

    void on_xminaxis_valueChanged(double arg1);

    void on_xmaxaxis_valueChanged(double arg1);

    void on_yminaxis_valueChanged(double arg1);

    void on_ymaxasis_valueChanged(double arg1);

    void on_zminazis_valueChanged(double arg1);

    void on_zmaxaxis_valueChanged(double arg1);

    void on_axisselectcolor_clicked();

    void on_scalegridmap_valueChanged(double arg1);

    void on_sizecornerxyz_valueChanged(double arg1);

private:

    Ui::PropertyEditor *ui;
    mrpt::utils::TColorf gridPlaneColor;
    mrpt::utils::TColorf gridMapColor;
    mrpt::utils::TColorf axisColor;
    mrpt::utils::TColorf objectColor;

};

#endif
