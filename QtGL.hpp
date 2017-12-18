#ifndef QTGL_H
#define QTGL_H

#include <QGLWidget>
#include <QApplication>
#include <QDebug>
#include <cmath>
#include <mrpt/gui/CDisplayWindow3D.h>
//#include <mrpt/system/threads.h>
#include <mrpt/system/os.h>
#include <mrpt/utils/CTicTac.h>
#include <mrpt/utils/CObserver.h>
#include <mrpt/opengl/gl_utils.h>
#include <mrpt/opengl/stock_objects.h>
#include <mrpt/opengl/CText.h>
#include <mrpt/opengl/CGridPlaneXY.h>
#include <mrpt/opengl/CAxis.h>
#include <mrpt/opengl/CBox.h>
#include <mrpt/opengl/CCylinder.h>
#include <mrpt/opengl/CSphere.h>
#include <mrpt/poses/CPose3D.h>
#include <GL/glut.h>
#include <QTimer>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QWheelEvent>
//#include <QTimer>
#include <mrpt/maps/COccupancyGridMap2D.h>
#include <mrpt/utils/CFileGZInputStream.h>
#include <string.h>
#include <mrpt/opengl/CText3D.h>
#include "MainWindow.hpp"

#include<mrpt/opengl/COpenGLViewport.h>
#include <mrpt/opengl.h>

using namespace std;
using namespace mrpt;
using namespace mrpt::gui;
using namespace mrpt::opengl;
using namespace mrpt::utils;
using namespace mrpt::poses;
using namespace mrpt::maps;

class QtGL : public QGLWidget{
    Q_OBJECT

public:
     QtGL(QWidget *parent = 0);
    ~QtGL();

    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);
   // static COpenGLScene* getScene();
 static   void setAzimuth(float);
 static   void setElevation(float);
 static    void setZoomDistance(float);
    void keyPressEvent(QKeyEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void AddScene();
    void setRender(bool);
    void updateCameraParams();

   static  float azimuth;  //方位
  static  float elevation; //海拔
   static  float distance;  // 距离
  static  COpenGLScene theScene;
  static float pointx,pointy,pointz;
   static void setViewPosition(float x,float y ,float z);
    QPoint getMousePosition();

public slots:
    void updateScene();
private:
    QPoint mousePosition;
    //COpenGLViewportPtr view;
    mrpt::opengl::COpenGLViewport::Ptr view;
    //QTimer* qtimer ;
    bool render;

};

#endif
