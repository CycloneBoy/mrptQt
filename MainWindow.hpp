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
#include "HelpDialog.hpp"
#include "PropertyEditor.hpp"
#include "QtGL.hpp"

// add for nav
#include <mrpt/nav.h>
#include <mrpt/opengl/CSetOfObjects.h>
#include <mrpt/opengl/CDisk.h>
#include <mrpt/opengl/CSimpleLine.h>
#include <mrpt/opengl/CSetOfLines.h>
#include <mrpt/opengl/CPointCloud.h>
#include <mrpt/opengl/CPlanarLaserScan.h>  // It's in the lib mrpt-maps
#include <mrpt/maps/COccupancyGridMap2D.h>
#include <memory>  // unique_ptr
#include <QTimer>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QWheelEvent>


#include <mrpt/slam/CMetricMapBuilderRBPF.h>
#include <mrpt/obs/CActionRobotMovement2D.h>
#include <mrpt/obs/CActionRobotMovement3D.h>
#include <mrpt/obs/CRawlog.h>
#include <mrpt/utils/CFileGZInputStream.h>
#include <mrpt/utils/CFileGZOutputStream.h>
#include <mrpt/utils/CFileOutputStream.h>
#include <mrpt/utils/CConfigFile.h>
#include <mrpt/gui/CDisplayWindow3D.h>
#include <mrpt/random.h>
#include <mrpt/system/filesystem.h>
#include <mrpt/system/os.h>
#include <mrpt/poses/CPosePDFGaussian.h>

#include <mrpt/opengl/CSetOfLines.h>
#include <mrpt/opengl/CGridPlaneXY.h>
#include <mrpt/opengl/CEllipsoid.h>
#include <mrpt/opengl/stock_objects.h>


#include <QtDebug>

using namespace mrpt;
using namespace mrpt::slam;
using namespace mrpt::maps;
using namespace mrpt::obs;
using namespace mrpt::opengl;
using namespace mrpt::gui;
using namespace mrpt::math;
using namespace mrpt::utils;
using namespace mrpt::system;
using namespace mrpt::random;
using namespace mrpt::poses;
using namespace mrpt::nav;
using namespace std;



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

 //   QtGL m_plotScan;
//    QtGL m_plot3D;

public:
        void initNavitionParmas();
        QTimer* timRunSimul;


         /* Methods: */
        void updateMap3DView();
        void reinitSimulator();  // Create navigator object & load params from GUI
        void simulateOneStep(double time_step);
        void updateViewsDynamicObjects();  // Update 3D object positions and refresh
        // views.

        void Onplot3DMouseClick(QMouseEvent *event);
        void Onplot3DMouseMove(QMouseEvent *event);


        void keyPressEvent(QKeyEvent *event);
        void mouseMoveEvent(QMouseEvent *event);
        void mousePressEvent(QMouseEvent *event);

        /* Vars: */
        struct TOptions : public mrpt::utils::CLoadableOptions
        {
            double ROBOT_MAX_SPEED;
            double MAX_SENSOR_RADIUS;
            uint64_t SENSOR_NUM_RANGES;
            double SENSOR_RANGE_NOISE_STD;

            TOptions();
            void loadFromConfigFile(
                const mrpt::utils::CConfigFileBase& source,
                const std::string& section) override;  // See base docs
            void saveToConfigFile(
                mrpt::utils::CConfigFileBase& source,
                const std::string& section) const override;  // See base docs
        };

        TOptions m_simul_options;

        /**  The state of the cursor onto the 3D view:
          */
        enum TCursorPickState
        {
            cpsNone = 0,
            cpsPickTarget,
            cpsPlaceRobot
        };

        std::unique_ptr<mrpt::nav::CAbstractHolonomicReactiveMethod>
            m_holonomicMethod;
        mrpt::maps::COccupancyGridMap2D m_gridMap;
        mrpt::math::TPoint2D m_targetPoint;
        mrpt::math::TPose2D m_robotPose;

        mrpt::utils::CTicTac m_runtime;
        /** Of the cursor on the 3D view (in world coordinates at Z=0) */
        mrpt::math::TPoint2D m_curCursorPos;
        /** The state of the cursor onto the 3D view: */
        TCursorPickState m_cursorPickState;

        // ========= Opengl View: Map & robot  =======
        mrpt::opengl::CSetOfObjects::Ptr gl_grid;
        mrpt::opengl::CSetOfObjects::Ptr gl_robot, gl_target;
        mrpt::opengl::CSetOfObjects::Ptr m_gl_placing_nav_target;
        mrpt::opengl::CSetOfObjects::Ptr m_gl_placing_robot;
        mrpt::opengl::CDisk::Ptr gl_robot_sensor_range;
        mrpt::opengl::CSetOfLines::Ptr gl_robot_path;
        mrpt::opengl::CPlanarLaserScan::Ptr gl_scan3D, gl_scan2D;
        mrpt::opengl::CPointCloud::Ptr gl_path;

        // ========= Opengl View: Local view (holonomic)  =======
        mrpt::opengl::CSimpleLine::Ptr gl_line_direction;
        mrpt::opengl::CPointCloud::Ptr gl_rel_target;
        mrpt::opengl::CSetOfLines::Ptr gl_nd_gaps;

public slots:
    void OntimRunSimulTrigger();

public :
         // =============  RBPF-SLAM   =====================
        void initMapBuildingParams();
        void MapBuilding_RBPF();

        /*****************************************************
                    Config params
         *****************************************************/
        std::string INI_FILENAME;
        std::string RAWLOG_FILE;
        unsigned int rawlog_offset;
        std::string OUT_DIR_STD;
        const char* OUT_DIR;
        int LOG_FREQUENCY;
        bool GENERATE_LOG_JOINT_H;
        bool GENERATE_LOG_INFO;
        bool SAVE_POSE_LOG;
        bool SAVE_MAP_IMAGES;
        bool SAVE_3D_SCENE;
        bool CAMERA_3DSCENE_FOLLOWS_ROBOT;

        bool SHOW_PROGRESS_IN_WINDOW;
        int SHOW_PROGRESS_IN_WINDOW_DELAY_MS;
        int PROGRESS_WINDOW_WIDTH = 600, PROGRESS_WINDOW_HEIGHT = 500;

        std::string METRIC_MAP_CONTINUATION_GRIDMAP_FILE;  // .gridmap file
        mrpt::math::TPose2D METRIC_MAP_CONTINUATION_START_POSE;
        std::string SIMPLEMAP_CONTINUATION;  // .simplemap file




public slots:

private slots:

    void on_actionOpen_triggered();

    void on_actionExit_triggered();

    void on_actionFull_Screen_triggered();

    void on_actionHelp_triggered();

    void on_actionProperty_Editor_triggered();

    void on_actionCapture_Image_triggered();


    void on_btnNavStartPoint_clicked();

    void on_btnNavEndPoint_clicked();

    void on_btnNavStart_clicked();


    void on_btnNavEnd_clicked();

    void on_btnSlamStart_clicked();

    void on_btnSlamEnd_clicked();

    void on_btnTest_clicked();

    void on_btnNavLoadMap_clicked();


    void on_btnNavMap_clicked();

    void on_btnSetRobotNavX_clicked();

    void on_btnSetRobotNavY_clicked();

private:
    Ui::MainWindow *ui;

    QPoint mousePosition;
};

#endif // MAINWINDOW_H
