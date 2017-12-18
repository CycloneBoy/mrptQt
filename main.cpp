#include <iostream>
#include "MainWindow.hpp"
#include <QApplication>
#include <iostream>
#include <GL/glut.h>

#include "mrptOne.h"
#include <QString>

using namespace std;



int main(int argc, char *argv[]){

    glutInit(&argc, argv);
    QApplication a(argc, argv);
    MainWindow w;
   // const QString  title =  new QString("移动机器人同步建图与定位V1.0");
    //w.setWindowTitle(&title);
    w.showMaximized();
    //HelpDialog h1;
    //h1.showMaximized();

    return a.exec();

    return 0;
}

int main1(int argc, char** argv)
{
    QApplication app(argc, argv);
    mrptOne mrptone;
    mrptone.show();
    return app.exec();
}

