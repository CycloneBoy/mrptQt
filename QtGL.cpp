#include "QtGL.hpp"

float QtGL::azimuth = 25.f;
float QtGL::elevation = 15.f;
float QtGL::distance = 35.f;

float QtGL::pointx =0.0f;
float QtGL::pointy =0.0f;
float QtGL::pointz =0.0f;

COpenGLScene QtGL::theScene;

QtGL::QtGL(QWidget *parent) :
    QGLWidget(parent)
{
    render = true;
    


   // qtimer = new QTimer(this);
}

void QtGL::initializeGL(){

    QTimer* qtimer = new QTimer(this);
    connect(qtimer, SIGNAL(timeout()), this, SLOT(updateScene()));
    qtimer->start(10);
    glClearColor(0.5, 0.5, 0.5, 1.0);
    AddScene();
    setFocus();
}



void QtGL::resizeGL(int w, int h){

    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, (float)w/h, 0.01, 100);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0,0.0,2.5, 0.0,0.0,0.0, 0,1,0);

}

void QtGL::paintGL(){
    if(render)
        theScene.render();
}

void QtGL::keyPressEvent(QKeyEvent *event){

    if(event->key() == Qt::Key_Right){

        QtGL::azimuth -= 5.f;

    }

    if(event->key() == Qt::Key_Left){

        QtGL::azimuth += 5.f;

    }

    if(event->key() == Qt::Key_Up){

        QtGL::elevation -= 2.f;

    }

    if(event->key() == Qt::Key_Down){

        QtGL::elevation += 2.f;

    }

    if(event->key() == Qt::Key_R){
        if(distance > 2.f)
            QtGL::distance -= 2.f;
    }

    if(event->key() == Qt::Key_T){
        QtGL::distance +=2.f;

    }


}

void QtGL::updateCameraParams(){

    QtGL::theScene.getViewport()->getCamera().setAzimuthDegrees(QtGL::azimuth);
    QtGL::theScene.getViewport()->getCamera().setElevationDegrees(QtGL::elevation);
    QtGL::theScene.getViewport()->getCamera().setZoomDistance(QtGL::distance);
   // QtGL::theScene.getViewport()->getCamera().setPointingAt(QtGL::pointx,QtGL::pointy,QtGL::pointz);
}


void QtGL::mouseMoveEvent(QMouseEvent *event){

    QPoint currentPos = event->pos();

    float x = -mousePosition.x() + currentPos.x();
    float y = -mousePosition.y() + currentPos.y();

    if(event->buttons() == Qt::LeftButton){


        if(x > 0){

            QtGL::azimuth -= 5.f;

        }
        else{

            QtGL::azimuth += 5.f;
        }


    }
    if(event->buttons() == Qt::RightButton){
        if(y > 0){

            QtGL::elevation += 2.f;

        }
        else{

            QtGL::elevation -= 2.f;
        }

//        if(x > 0){
//            QtGL::pointx -=5.0f;
//        }
//        else{
//            QtGL::pointx +=5.0f;
//        }

//        if(y > 0){

//           QtGL::pointy -=5.0f;

//        }
//        else{

//            QtGL::pointy +=5.0f;
//        }
    }
    mousePosition = currentPos;

}

void QtGL::wheelEvent(QWheelEvent *event){
    if(event->delta() > 0){
        if(QtGL::distance > 2.f)
            QtGL::distance -= 2.f;
    }else{
        QtGL::distance += 2.f;
    }
}

/*COpenGLScene* QtGL::getScene(){
    return theScene;
}*/

void QtGL::setAzimuth(float azi){
    QtGL::azimuth = azi;
}

void QtGL::setElevation(float ele){
    QtGL::elevation = ele;
}

void QtGL::setZoomDistance(float dis){
    QtGL::distance = dis;

}

static void setViewPosition(float x,float y ,float z){
    QtGL::pointx=x;
    QtGL::pointy=y;
    QtGL::pointz=z;
}

void QtGL::updateScene(){
    updateGL();
    updateCameraParams();
    qApp->processEvents();
}

void QtGL::AddScene(){
    {
      
        opengl::CGridPlaneXY::Ptr obj = opengl::CGridPlaneXY::Create(-150,150,-150,150,0,1);
        //opengl::CGridPlaneXY::Ptr obj =  mrpt::make_aligned_shared<opengl::CGridPlaneXY>(-150,150,-150,150,0,1);
        obj->setColor(0.8,0.8,0.8);
        obj->setName("gridplane");
        QtGL::theScene.insert( obj );
    }
    {
        opengl::CRenderizable::Ptr obj = mrpt::opengl::stock_objects::CornerXYZ();
        //opengl::CRenderizable::Ptr obj = mrpt::make_aligned_shared<mrpt::opengl::stock_objects>();
        obj->setName("corner");
        QtGL::theScene.insert( obj);
    }
    {
        opengl::CAxis::Ptr obj = opengl::CAxis::Create();
        //opengl::CAxis::Ptr obj = mrpt::make_aligned_shared<opengl::CAxis>();
        obj->setFrequency(5);
        obj->enableTickMarks();
        obj->setAxisLimits(-10,-10,-10, 10,10,10);
        obj->setName("axis");
        QtGL::theScene.insert( obj );
    }
}

void QtGL::setRender(bool b){
    render = b;
}

QtGL::~QtGL(){
    QtGL::theScene.removeObject(QtGL::theScene.getByName("gridmap"));
}


QPoint QtGL:: getMousePosition(){
    return this->mousePosition;
}
