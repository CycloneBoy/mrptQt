#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <iostream>
#include <QProgressBar>
#include <QFileDialog>
#include <QProcess>
#include <QKeyEvent>
#include <QScreen>
#include <QClipboard>
#include <vector>
#include <map>
#include <mrpt/gui/CDisplayWindow3D.h>
//#include <mrpt/system/threads.h>
#include <mrpt/system/os.h>
#include <mrpt/utils/CTicTac.h>
#include <mrpt/utils/CObserver.h>
#include <mrpt/utils/CFileGZInputStream.h>
#include <mrpt/opengl/gl_utils.h>
#include <mrpt/opengl/stock_objects.h>
#include <mrpt/opengl/CText.h>
#include <mrpt/opengl/CGridPlaneXY.h>
#include <mrpt/opengl/CAxis.h>
#include <mrpt/opengl/CBox.h>
#include <mrpt/opengl/CCylinder.h>
#include <mrpt/opengl/CSphere.h>
#include <mrpt/poses/CPose3D.h>
#include <mrpt/maps/COccupancyGridMap2D.h>
#include <mrpt/maps/CSimplePointsMap.h>
#include <QProcess>
#include "../headers/HelpDialog.hpp"
#include "../headers/PropertyEditor.hpp"

using namespace std;
using namespace mrpt;
using namespace mrpt::gui;
using namespace mrpt::opengl;
using namespace mrpt::utils;
using namespace mrpt::poses;
using namespace mrpt::maps;


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
   // Q_OBJECTasdas
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void openFile(QString filename);
    void normalScreen();
    bool eventFilter(QObject *target, QEvent *event);

public slots:

private slots:

    void on_actionOpen_triggered();

    void on_actionExit_triggered();

    void on_actionFull_Screen_triggered();

    void on_actionHelp_triggered();

    void on_actionProperty_Editor_triggered();

    void on_actionCapture_Image_triggered();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
