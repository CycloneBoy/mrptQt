#include "PropertyEditor.hpp"
#include <mrpt/opengl/gl_utils.h>
#include <mrpt/opengl/stock_objects.h>
#include <mrpt/opengl/CText.h>
#include <mrpt/opengl/CGridPlaneXY.h>
#include <mrpt/opengl/CAxis.h>
#include <mrpt/opengl/CBox.h>
#include <mrpt/opengl/CCylinder.h>
#include <mrpt/opengl/CSphere.h>

#include "../build-mrptQt-Desktop_Qt_5_10_0_GCC_64bit-Debug/ui_Propertyeditor.h"

using namespace std;
using namespace mrpt;
using namespace mrpt::opengl;

TColorf toTColorf(QColor c){

    TColorf color;
    color.A = c.alphaF();
    color.B = c.blueF();
    color.G = c.greenF();
    color.R = c.redF();
    return color;
}

QColor toQColor(TColorf c){

    QColor color;
    color.setRedF(c.R);
    color.setGreenF(c.G);
    color.setBlueF(c.B);
    color.setAlphaF(c.A);

    return color;
}

PropertyEditor::PropertyEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PropertyEditor)
{
    ui->setupUi(this);

    /*for camera settings*/
    ui->azimuthbox->setValue(QtGL::azimuth);
    ui->elevationbox->setValue(QtGL::elevation);
    ui->zoombox->setValue(QtGL::distance);
 
    /*for grid plane*/
    opengl::CRenderizable::Ptr obj = QtGL:: theScene.getByName("gridplane");
    //std::shared_ptr<mrpt::opengl::CGridPlaneXY> obj = QtGL:: theScene.getByName("gridplane").get();
    float xmin, xmax, ymin, ymax;
    
  
    //obj->getPlaneLimits(xmin,xmax, ymin, ymax); 				//*******************
    gridPlaneColor = obj->getColor();

    setColor(ui->gridcolorlabel, toQColor(gridPlaneColor));
    ui->xmingridplane->setValue(xmin);
    ui->xmaxgridplane->setValue(xmax);
    ui->ymingridplane->setValue(ymin);
    ui->ymaxgridplane->setValue(ymax);

    /*for corner*/

    ui->sizecornerxyz->setValue(QtGL::theScene.getByName("corner")->getScaleX());

    /*for axis*/
    //opengl::CRenderizable::Ptr axis = QtGL::theScene.getByName("axis");    //------------
     opengl::CRenderizable::Ptr axis = QtGL::theScene.getByName("axis");
    
     // std::shared_ptr<mrpt::opengl::CAxis>  axis = QtGL::theScene.getByName("axis").get();
      
    mrpt::math::TPoint3D p1,p2;
    axis->getBoundingBox(p1,p2);
    ui->xminaxis->setValue(p1.x);
    ui->xmaxaxis->setValue(p2.x);
    ui->yminaxis->setValue(p1.y);
    ui->ymaxasis->setValue(p2.y);
    ui->zminazis->setValue(p1.z);
    ui->zmaxaxis->setValue(p2.z);
   // ui->linewidthaxis->setValue( axis->getLineWidth());                         //*******************
    axisColor = axis->getColor();
    setColor(ui->coloraxislabel, toQColor(axisColor));

    /*for gridmap object*/
    opengl::CRenderizable::Ptr gridobj = QtGL::theScene.getByName("gridmap");

    if(gridobj != NULL){

        ui->scalegridmap->setEnabled(true);
        ui->colorgridmap->setEnabled(true);

        float scale = gridobj->getScaleX();
        ui->scalegridmap->setValue(scale);
        gridMapColor = gridobj->getColor();
        setColor(ui->colorlabelgridmap, toQColor(gridMapColor));

    }
    else{

        ui->scalegridmap->setEnabled(false);
        ui->colorgridmap->setEnabled(false);
    }

    enableApplyButton(false);
}

void PropertyEditor::on_azimuthbox_valueChanged(double value)
{
    QtGL::azimuth = value;

}

void PropertyEditor::on_elevationbox_valueChanged(double arg1)
{
    QtGL::elevation = arg1;

}

void PropertyEditor::on_zoombox_valueChanged(double arg1)
{
    QtGL::distance = arg1;
}


void PropertyEditor::on_xmingridplane_valueChanged(double arg1)
{

    ui->xmaxgridplane->setMinimum(arg1);
    //opengl::CGridPlaneXY::Ptr
    opengl::CRenderizable::Ptr gridplane =  QtGL::theScene.getByName("gridplane");   //------------
    //gridplane->setPlaneLimits(ui->xmingridplane->value(),ui->xmaxgridplane->value(),ui->ymingridplane->value(),ui->ymaxgridplane->value());   //*******************

}

void PropertyEditor::on_xmaxgridplane_valueChanged(double arg1)
{
    ui->xmingridplane->setMaximum(arg1);
     opengl::CRenderizable::Ptr gridplane =  QtGL::theScene.getByName("gridplane");   //------------
    //gridplane->setPlaneLimits(ui->xmingridplane->value(),ui->xmaxgridplane->value(),ui->ymingridplane->value(),ui->ymaxgridplane->value());  //*******************
}

void PropertyEditor::on_ymingridplane_valueChanged(double arg1)
{
    ui->ymaxgridplane->setMinimum(arg1);
    opengl::CRenderizable::Ptr gridplane =  QtGL::theScene.getByName("gridplane");   //------------
    //gridplane->setPlaneLimits(ui->xmingridplane->value(),ui->xmaxgridplane->value(),ui->ymingridplane->value(),ui->ymaxgridplane->value());  //*******************
}

void PropertyEditor::on_ymaxgridplane_valueChanged(double arg1)
{
    ui->ymingridplane->setMaximum(arg1);
     opengl::CRenderizable::Ptr gridplane =  QtGL::theScene.getByName("gridplane");   //------------
    //gridplane->setPlaneLimits(ui->xmingridplane->value(),ui->xmaxgridplane->value(),ui->ymingridplane->value(),ui->ymaxgridplane->value());  //*******************
}

void PropertyEditor::on_selectcolorgridplane_clicked()
{
    QColor color = QColorDialog::getColor(toQColor(gridPlaneColor));

    if( !color.isValid() ){
        return;
    }

    setColor(ui->gridcolorlabel, color);
    gridPlaneColor = toTColorf(color);

    opengl::CRenderizable::Ptr gridplane =  QtGL::theScene.getByName("gridplane");    //------------
    gridplane->setColor(gridPlaneColor);

}

void PropertyEditor::setColor(QLabel* label, QColor c){

    QPixmap pixmap(16, 16);
    pixmap.fill(c);
    label->setPixmap(pixmap);
}

void PropertyEditor::on_colorgridmap_clicked()
{
    QColor color = QColorDialog::getColor(toQColor(gridMapColor));

    if( !color.isValid() ){
        return;
    }

    setColor(ui->colorlabelgridmap, color);
    gridMapColor = toTColorf(color);

    opengl::CRenderizable::Ptr obj = QtGL::theScene.getByName("gridmap");

    if(obj != NULL){

        obj->setColor(gridMapColor);
    }

}

void PropertyEditor::on_buttonBox_accepted()
{
    if(ui->buttonBox->button(QDialogButtonBox::Apply)->isEnabled()){
        ui->buttonBox->button(QDialogButtonBox::Apply)->click();
    }
    return;
}

void PropertyEditor::enableApplyButton(bool b){

    ui->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(b);
}

void PropertyEditor::on_buttonBox_clicked(QAbstractButton *button)
{
    if(button == ui->buttonBox->button(QDialogButtonBox::Apply)){
        {


            QtGL::theScene.removeObject(QtGL::theScene.getByName("axis"));

            opengl::CAxis::Ptr obj = opengl::CAxis::Create();
            obj->setFrequency(ui->xmaxaxis->value()/2);
            obj->enableTickMarks();
            obj->setAxisLimits(ui->xminaxis->value(),
                               ui->yminaxis->value(),
                               ui->zminazis->value(),
                               ui->xmaxaxis->value(),
                               ui->ymaxasis->value(),
                               ui->zmaxaxis->value());
            obj->setName("axis");
            obj->setColor(axisColor);
            obj->setLineWidth(ui->linewidthaxis->value());
            QtGL::theScene.insert( obj );
        }



        ui->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(false);
    }

}

void PropertyEditor::on_linewidthaxis_valueChanged(double arg1)
{

    opengl::CRenderizable::Ptr axis = QtGL::theScene.getByName("axis");              //------------
  //  axis->setLineWidth(arg1);  								//*******************


}

void PropertyEditor::on_xminaxis_valueChanged(double arg1)
{
    ui->xmaxaxis->setMinimum(arg1);
    enableApplyButton(true);
}

void PropertyEditor::on_xmaxaxis_valueChanged(double arg1)
{
    ui->xminaxis->setMaximum(arg1);
    enableApplyButton(true);
}

void PropertyEditor::on_yminaxis_valueChanged(double arg1)
{
    ui->ymaxasis->setMinimum(arg1);
    enableApplyButton(true);
}

void PropertyEditor::on_ymaxasis_valueChanged(double arg1)
{
    ui->yminaxis->setMaximum(arg1);
    enableApplyButton(true);
}

void PropertyEditor::on_zminazis_valueChanged(double arg1)
{
    ui->zmaxaxis->setMinimum(arg1);
    enableApplyButton(true);
}

void PropertyEditor::on_zmaxaxis_valueChanged(double arg1)
{
    ui->zminazis->setMaximum(arg1);
    enableApplyButton(true);
}

PropertyEditor::~PropertyEditor()
{
    delete ui;
}





void PropertyEditor::on_axisselectcolor_clicked()
{
    QColor color = QColorDialog::getColor(toQColor(axisColor));

    if( !color.isValid() ){
        return;
    }

    setColor(ui->coloraxislabel, color);
    axisColor = toTColorf(color);

    opengl::CRenderizable::Ptr axis =  QtGL::theScene.getByName("axis");   //------------
    axis->setColor(axisColor);

}

void PropertyEditor::on_scalegridmap_valueChanged(double arg1)
{

    opengl::CRenderizable::Ptr obj = QtGL::theScene.getByName("gridmap");

    if(obj != NULL){

        obj->setScale(arg1);
    }

}

void PropertyEditor::on_sizecornerxyz_valueChanged(double arg1)
{
    QtGL::theScene.getByName("corner")->setScale(arg1);
}
