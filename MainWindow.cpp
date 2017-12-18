
#include "MainWindow.hpp"
//#include "/usr/include/x86_64-linux-gnu/qt5/QtGui/qguiapplication.h"
//#include "/home/muhammad/Desktop/MRPT/samples/build-qt_integration-Desktop-Default/ui_MainWindow.h"
//#include "../build-mrptQt-Desktop_Qt_5_10_0_GCC_64bit-Debug/ui_MainWindow.h"
#include "../build-mrptQt-Desktop_Qt_5_10_0_GCC_64bit-Debug/ui_MainWindow.h"

//#include <qt5/QtGui/QGuiApplication>
#include "QtGL.hpp"

#include <mrpt/gui/about_box.h>
#include "DEFAULT_GRIDMAP_DATA.h"
#include <mrpt/nav.h>
#include <mrpt/gui.h>
#include <mrpt/opengl.h>
#include <mrpt/system/filesystem.h>
#include <QDebug>




MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_gridMap(),
    m_targetPoint(-5, -7),
    m_robotPose(0, 0, 0),
    m_curCursorPos(0, 0),
    m_cursorPickState(cpsNone)
{
    ui->setupUi(this);
    ui->statusBar->showMessage("Ready");
    ui->statusBar->setToolTip("Click the open button to open a gridmap file");
    ui->centralWidget->installEventFilter(this);

    ui->actionExit->setShortcut(Qt::Key_F4 | Qt::Key_Alt);
    ui->actionFull_Screen->setShortcut(Qt::Key_F11);
    ui->actionHelp->setShortcut(Qt::Key_F1);
    ui->actionOpen->setShortcut(Qt::Key_O | Qt::CTRL);
    ui->actionCapture_Image->setShortcut(Qt::Key_I | Qt::CTRL);
    ui->actionProperty_Editor->setShortcut(Qt::Key_P | Qt::CTRL);

    ui->centralWidget->setMouseTracking(true);
    this->setMouseTracking(true);

    timRunSimul = new QTimer(this);
    connect(timRunSimul, SIGNAL(timeout()), this, SLOT(OntimRunSimulTrigger()));
}


MainWindow::~MainWindow()
{
    delete ui;
}


// ------------------------------------------------------
//					UI init and some
// ------------------------------------------------------
void MainWindow::on_actionOpen_triggered()
{
    QString name = QFileDialog::getOpenFileName(this, "Select gridmap file",tr("All files (*.*);;GRIDMAP(*.gridmap)") ,"GRIDMAP (*.gridmap)");

    if(name.isEmpty()){

        return;
    }
    openFile(name);

}

void MainWindow::openFile(QString filename){

    COccupancyGridMap2D gridmap;
    CFileGZInputStream in_s(filename.toStdString());
    in_s >> gridmap;
    CSetOfObjects::Ptr gl_grid;
    if (!gl_grid)
        gl_grid = CSetOfObjects::Create();

    gl_grid->clear();
    gridmap.getAs3DObject(gl_grid);
    gl_grid->setName("gridmap");

   opengl::CRenderizable::Ptr obj =QtGL::theScene.getByName("gridmap");

    if(obj != NULL){

        QtGL::theScene.removeObject(obj);
    }


     QtGL::theScene.insert(gl_grid);
    
   // ui->widget->setRender(true);
    ui->statusBar->showMessage(filename);

}

void MainWindow::on_actionExit_triggered()
{
    QCoreApplication::quit();
}

bool MainWindow::eventFilter(QObject *target, QEvent *event) {
    if (event->type() == QEvent::KeyRelease) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if(keyEvent->key() == Qt::Key_Escape){
            normalScreen();
        }

        return QObject::eventFilter(target, event);
    }
}

void MainWindow::on_actionFull_Screen_triggered()
{
    std::system("notify-send 'MRPT - Gridmap visualizer' 'Please use escape to exit fullscreen'");
    this->showFullScreen();
    ui->toolBar->hide();
    ui->statusBar->hide();

}

void MainWindow::normalScreen(){

    this->showNormal();
    ui->toolBar->show();
    ui->statusBar->show();
    this->showMaximized();

}

void MainWindow::on_actionHelp_triggered()
{
   HelpDialog dialog(this);
    dialog.setWindowTitle("Help");
    dialog.exec();
}

void MainWindow::on_actionProperty_Editor_triggered()
{
    PropertyEditor dialog(this);
    dialog.setWindowTitle("Property Editor");
    dialog.exec();
}

void MainWindow::on_actionCapture_Image_triggered()
{

    //   QtGL* w = ui->centralWidget->findChild<QtGL*>("widget");

    //    QScreen *screen = QGuiApplication::primaryScreen();
    //    //QScreen *screen1 = QGuiApplication::primaryScreen();
    //    QPixmap pixmap;
    //    if (screen){
    //       pixmap = screen->grabWindow(w->winId());
    //       //pixmap = screen->pixmapDataFactory();

    //    }
    //     QApplication::clipboard()->setPixmap(pixmap);
     
     QMessageBox::information(this,"Capture image", "Successfully copied the image to the clipboard!");
}


void MainWindow::keyPressEvent(QKeyEvent *event){

    if(event->key() == Qt::Key_Right){


    }

    if(event->key() == Qt::Key_Left){



    }

    if(event->key() == Qt::Key_Up){


    }

    if(event->key() == Qt::Key_Down){



    }

    if(event->key() == Qt::Key_R){

    }

    if(event->key() == Qt::Key_T){


    }


}

void MainWindow::mouseMoveEvent(QMouseEvent *event){

    QPoint currentPos = event->pos();

    /*float x = -mousePosition.x() + currentPos.x();
    float y = -mousePosition.y() + currentPos.y();

    if(event->buttons() == Qt::LeftButton){
        if(x > 0){

        }
        else{

        }
    }
    if(event->buttons() == Qt::RightButton){
        if(y > 0){

        }
        else{

        }
    }*/

    int X, Y;
    X = currentPos.x();
    Y = currentPos.y();
    qDebug("nav target point currentPos X=%.03f Y=%.04f Z=0", currentPos.x(), currentPos.y());
    qDebug("QtGL currentPos X=%.03f Y=%.04f Z=0", ui->widget->getMousePosition().x(),  ui->widget->getMousePosition().y());

    // Intersection of 3D ray with ground plane ====================
    TLine3D ray;
    QtGL::theScene.getViewport()->get3DRayForPixelCoord( X, Y, ray);

    // Create a 3D plane, e.g. Z=0
    const TPlane ground_plane(
                TPoint3D(0, 0, 0), TPoint3D(1, 0, 0), TPoint3D(0, 1, 0));
    // Intersection of the line with the plane:
    TObject3D inters;
    intersect(ray, ground_plane, inters);
    // Interpret the intersection as a point, if there is an intersection:
    TPoint3D inters_pt;
    if (inters.getPoint(inters_pt))
    {
        m_curCursorPos.x = inters_pt.x;
        m_curCursorPos.y = inters_pt.y;
        qDebug("nav target point  X=%.03f Y=%.04f Z=0", inters_pt.x, inters_pt.y);
        if (m_cursorPickState == cpsPickTarget)
        {
            m_gl_placing_nav_target->setVisibility(true);
            m_gl_placing_nav_target->setLocation(
                        m_curCursorPos.x, m_curCursorPos.y, 0.05);

            qDebug("nav target point  X=%.03f Y=%.04f Z=0", m_curCursorPos.x, m_curCursorPos.y);
        }
        else if (m_cursorPickState == cpsPlaceRobot)
        {
            m_gl_placing_robot->setVisibility(true);
            m_gl_placing_robot->setLocation(
                        m_curCursorPos.x, m_curCursorPos.y, 0.05);

             qDebug("nav robot point  X=%.03f Y=%.04f Z=0", m_curCursorPos.x, m_curCursorPos.y);
        }

       // qDebug("X=%.03f Y=%.04f Z=0", m_curCursorPos.x, m_curCursorPos.y);
    }


}

void MainWindow::mousePressEvent(QMouseEvent *event){
    switch (m_cursorPickState)
        {
            case cpsPickTarget:
            {
                m_targetPoint = m_curCursorPos;

                ui->btnNavEndPoint->setEnabled(false);
                ui->btnNavEndPoint->update();
                m_gl_placing_nav_target->setVisibility(false);
                qDebug("place the robot target point : %.03f %.03f",m_targetPoint.x,m_targetPoint.y);
                break;
            }
            case cpsPlaceRobot:
            {
                m_robotPose.x = m_curCursorPos.x;
                m_robotPose.y = m_curCursorPos.y;

                ui->btnNavStartPoint->setEnabled(false);
                ui->btnNavStartPoint->update();
                m_gl_placing_robot->setVisibility(false);
                 qDebug("place the robot start point : %.03f %.03f",m_targetPoint.x,m_targetPoint.y);
                break;
            }
            default:
                break;
        }

        ui->widget->setCursor(Qt::ArrowCursor);
        //m_plot3D->SetCursor(*wxSTANDARD_CURSOR);  // End of cross cursor
        m_cursorPickState = cpsNone;  // end of mode


}


void MainWindow::on_btnTest_clicked()
{
       std::cout<< "on_btnTest_clicked"<<std::endl;


}




// ------------------------------------------------------
//					RBPF-SLAM  MapBuilding RBPF
// ------------------------------------------------------
/**
 *  rbpf-slam 参数设置
 * @brief MainWindow::initMapBuildingParams
 */
void MainWindow::initMapBuildingParams(){

       qDebug()<<"移动机器人同步建图与导航V1.0  \n";
        qDebug()<< " MRPT C++ Library: "<<MRPT_getVersion().c_str() <<  "- Sources timestamp: " << MRPT_getCompilationDate().c_str();


        std::string intConfigFileName = string("/usr/share/mrpt/config_files/rbpf-slam/gridmapping_RBPF_grid_ICPbased_malaga.ini");
        //std::string intConfigFileName = string("/home/sl/projects/Qt/gridmapping_RBPF_grid_ICPbased_malaga.ini");

        INI_FILENAME = std::string(intConfigFileName);

        ASSERT_FILE_EXISTS_(INI_FILENAME)


        string override_rawlog_file;
        override_rawlog_file = string("/usr/share/mrpt/datasets/2006-01ENE-21-SENA_Telecom Faculty_one_loop_only.rawlog");

        CConfigFile iniFile(INI_FILENAME);

        // ------------------------------------------
        //			Load config from file:
        // ------------------------------------------
        RAWLOG_FILE = !override_rawlog_file.empty()
                          ? override_rawlog_file
                          : iniFile.read_string(
                                "MappingApplication", "rawlog_file", "",
                                /*Force existence:*/ true);
        rawlog_offset =
            iniFile.read_int("MappingApplication", "rawlog_offset", 0);
        OUT_DIR_STD = iniFile.read_string(
            "MappingApplication", "logOutput_dir", "log_out",
            /*Force existence:*/ true);
        LOG_FREQUENCY = iniFile.read_int(
            "MappingApplication", "LOG_FREQUENCY", 5,
            /*Force existence:*/ true);
        GENERATE_LOG_JOINT_H = iniFile.read_bool(
            "MappingApplication", "GENERATE_LOG_JOINT_H", false);
        GENERATE_LOG_INFO =
            iniFile.read_bool("MappingApplication", "GENERATE_LOG_INFO", false);
        SAVE_POSE_LOG =
            iniFile.read_bool("MappingApplication", "SAVE_POSE_LOG", false);
        SAVE_MAP_IMAGES =
            iniFile.read_bool("MappingApplication", "SAVE_MAP_IMAGES", false);
        SAVE_3D_SCENE =
            iniFile.read_bool("MappingApplication", "SAVE_3D_SCENE", false);
        CAMERA_3DSCENE_FOLLOWS_ROBOT = iniFile.read_bool(
            "MappingApplication", "CAMERA_3DSCENE_FOLLOWS_ROBOT", true);
        SHOW_PROGRESS_IN_WINDOW = iniFile.read_bool(
            "MappingApplication", "SHOW_PROGRESS_IN_WINDOW", false);
        SHOW_PROGRESS_IN_WINDOW_DELAY_MS = iniFile.read_int(
            "MappingApplication", "SHOW_PROGRESS_IN_WINDOW_DELAY_MS", 1);
        METRIC_MAP_CONTINUATION_GRIDMAP_FILE = iniFile.read_string(
            "MappingApplication", "METRIC_MAP_CONTINUATION_GRIDMAP_FILE", "");
        MRPT_LOAD_CONFIG_VAR(
            SIMPLEMAP_CONTINUATION, string, iniFile, "MappingApplication");

        METRIC_MAP_CONTINUATION_START_POSE.x = iniFile.read_double(
            "MappingApplication", "METRIC_MAP_CONTINUATION_START_POSE_X", .0);
        METRIC_MAP_CONTINUATION_START_POSE.y = iniFile.read_double(
            "MappingApplication", "METRIC_MAP_CONTINUATION_START_POSE_Y", .0);
        METRIC_MAP_CONTINUATION_START_POSE.phi = DEG2RAD(
            iniFile.read_double(
                "MappingApplication",
                "METRIC_MAP_CONTINUATION_START_POSE_PHI_DEG", .0));

        MRPT_LOAD_CONFIG_VAR(
            PROGRESS_WINDOW_WIDTH, int, iniFile, "MappingApplication");
        MRPT_LOAD_CONFIG_VAR(
            PROGRESS_WINDOW_HEIGHT, int, iniFile, "MappingApplication");

        // easier!
        OUT_DIR = OUT_DIR_STD.c_str();

        // Print params:
        qDebug("Running with the following parameters:\n");
        qDebug(" RAWLOG file:'%s'\n", RAWLOG_FILE.c_str());
        qDebug(" Output directory:\t\t\t'%s'\n", OUT_DIR);
        qDebug(" Log record freq:\t\t\t%u\n", LOG_FREQUENCY);
        qDebug("  GENERATE_LOG_JOINT_H:\t\t\t%c\n", GENERATE_LOG_JOINT_H ? 'Y' : 'N');
        qDebug("  GENERATE_LOG_INFO:\t\t\t%c\n", GENERATE_LOG_INFO ? 'Y' : 'N');
        qDebug("  SAVE_MAP_IMAGES:\t\t\t%c\n", SAVE_MAP_IMAGES ? 'Y' : 'N');
        qDebug("  SAVE_3D_SCENE:\t\t\t%c\n", SAVE_3D_SCENE ? 'Y' : 'N');
        qDebug("  SAVE_POSE_LOG:\t\t\t%c\n", SAVE_POSE_LOG ? 'Y' : 'N');
        qDebug( "  CAMERA_3DSCENE_FOLLOWS_ROBOT:\t%c\n", CAMERA_3DSCENE_FOLLOWS_ROBOT ? 'Y' : 'N');
        qDebug( "  SHOW_PROGRESS_IN_WINDOW:\t%c\n",SHOW_PROGRESS_IN_WINDOW ? 'Y' : 'N');


        // Checks:
        ASSERT_(RAWLOG_FILE.size() > 0);
        ASSERT_FILE_EXISTS_(RAWLOG_FILE)

        // Set relative path for externally-stored images in rawlogs:
        string rawlog_images_path = extractFileDirectory(RAWLOG_FILE);
        rawlog_images_path += "/Images";
        CImage::setImagesPathBase(rawlog_images_path);  // Set it.

        // Run:qDebug
      // MapBuilding_RBPF();


}

// MapBuilding RBPF
void MainWindow::MapBuilding_RBPF()
{
    qDebug()<<"Build map ";

    MRPT_START

    CTicTac tictac, tictacGlobal, tictac_JH;
    int step = 0;
    CSimpleMap finalMap;
    float t_exec;
    COccupancyGridMap2D::TEntropyInfo entropy;

    char strFil[1000];

    size_t rawlogEntry = 0;
    CFileGZInputStream rawlogFile(RAWLOG_FILE);

    // ---------------------------------
    //		MapPDF opts
    // ---------------------------------
    CMetricMapBuilderRBPF::TConstructionOptions rbpfMappingOptions;

    rbpfMappingOptions.loadFromConfigFile(
        CConfigFile(INI_FILENAME), "MappingApplication");
    rbpfMappingOptions.dumpToConsole();

    // ---------------------------------
    //		Constructor
    // ---------------------------------
    CMetricMapBuilderRBPF mapBuilder(rbpfMappingOptions);

    // handle the case of metric map continuation
    if (!METRIC_MAP_CONTINUATION_GRIDMAP_FILE.empty())
    {
        CSimpleMap dummySimpleMap;
        CPosePDFGaussian startPose;

        startPose.mean.x(METRIC_MAP_CONTINUATION_START_POSE.x);
        startPose.mean.y(METRIC_MAP_CONTINUATION_START_POSE.y);
        startPose.mean.phi(METRIC_MAP_CONTINUATION_START_POSE.phi);
        startPose.cov.setZero();

        mrpt::maps::COccupancyGridMap2D gridmap;
        {
            mrpt::utils::CFileGZInputStream f(
                METRIC_MAP_CONTINUATION_GRIDMAP_FILE);
            f >> gridmap;
        }

        mapBuilder.initialize(dummySimpleMap, &startPose);

        for (CMultiMetricMapPDF::CParticleList::iterator it =
                 mapBuilder.mapPDF.m_particles.begin();
             it != mapBuilder.mapPDF.m_particles.end(); ++it)
        {
            CRBPFParticleData* part_d = it->d.get();
            CMultiMetricMap& mmap = part_d->mapTillNow;
            mrpt::maps::COccupancyGridMap2D::Ptr it_grid =
                mmap.getMapByClass<mrpt::maps::COccupancyGridMap2D>();
            ASSERTMSG_(
                it_grid,
                "No gridmap in multimetric map definition, but metric map "
                "continuation was set (!)");
            it_grid->copyMapContentFrom(gridmap);
        }
    }
    if (!SIMPLEMAP_CONTINUATION.empty())
    {
        mrpt::maps::CSimpleMap init_map;
        mrpt::utils::CFileGZInputStream(SIMPLEMAP_CONTINUATION) >> init_map;
        mapBuilder.initialize(init_map);
    }

    // ---------------------------------
    //   CMetricMapBuilder::TOptions
    // ---------------------------------
    // mapBuilder.setVerbosityLevel(  mrpt::utils::LVL_DEBUG );  // default
    // value: as loaded from config file
    mapBuilder.options.enableMapUpdating = true;
    mapBuilder.options.debugForceInsertion = false;

    getRandomGenerator().randomize();

    // Prepare output directory:
    // --------------------------------
    // os::sprintf(strFil,1000,"%s/*.*",OUT_DIR);
    deleteFilesInDirectory(OUT_DIR);
    createDirectory(OUT_DIR);

    string OUT_DIR_MAPS = format("%s/maps", OUT_DIR);
    string OUT_DIR_3D = format("%s/3D", OUT_DIR);

    deleteFilesInDirectory(OUT_DIR_MAPS);
    createDirectory(OUT_DIR_MAPS);

    deleteFilesInDirectory(OUT_DIR_3D);
    createDirectory(OUT_DIR_3D);

    // Open log files:
    // ----------------------------------
    CFileOutputStream f_log(format("%s/log_times.txt", OUT_DIR));
    CFileOutputStream f_info(format("%s/log_info.txt", OUT_DIR));
    CFileOutputStream f_jinfo(format("%s/log_jinfo.txt", OUT_DIR));
    CFileOutputStream f_path(format("%s/log_estimated_path.txt", OUT_DIR));
    CFileOutputStream f_pathOdo(format("%s/log_odometry_path.txt", OUT_DIR));
    CFileOutputStream f_partStats(format("%s/log_ParticlesStats.txt", OUT_DIR));

    f_log.printf(
        "%% time_step  execution_time(ms)  map_size(#frames)  frame_inserted? "
        "\n"
        "%%-------------------------------------------------------------------"
        "\n");

    f_info.printf(
        "%% EMI    H    EMMI    effecMappedArea  effecMappedCells \n"
        "%%-------------------------------------------------------\n");

    f_pathOdo.printf(
        "%% time_step  x  y z yaw pitch roll timestamp \n"
        "%%--------------------------------------------\n");

    f_pathOdo.printf(
        "%% time_step  x  y z yaw pitch roll \n"
        "%%----------------------------------\n");

    f_partStats.printf(
        "%% time_step   #particles   ESS \n"
        "%%------------------------------\n");

    // ----------------------------------------------------------
    //						Map Building
    // ----------------------------------------------------------
    CActionCollection::Ptr action ;
    CSensoryFrame::Ptr observations ;
    //std::deque<CObservationGasSensors::Ptr> gasObservations =nullptr;
    //std::deque<CObservationWirelessPower::Ptr> wifiObservations = nullptr;
    CPose3D odoPose(0, 0, 0);

//   mrpt::gui:: CDisplayWindow3D win3D("RBPF-SLAM @ MRPT C++ Library", PROGRESS_WINDOW_WIDTH, PROGRESS_WINDOW_HEIGHT);
//    win3D.setCameraZoom(40);
//    win3D.setCameraAzimuthDeg(-50);
//    win3D.setCameraElevationDeg(70);

//    if (SHOW_PROGRESS_IN_WINDOW)
//    {
//        win3D = new CDisplayWindow3D(
//            "RBPF-SLAM @ MRPT C++ Library", PROGRESS_WINDOW_WIDTH,
//            PROGRESS_WINDOW_HEIGHT);
//        win3D.setCameraZoom(40);
//        win3D.setCameraAzimuthDeg(-50);
//        win3D.setCameraElevationDeg(70);
//    }

  tictacGlobal.Tic();
    for (;;)
    {
        if (os::kbhit())
        {
            char c = os::getch();
            if (c == 27) break;
        }

        // Load action/observation pair from the rawlog:
        // --------------------------------------------------
        if (!CRawlog::readActionObservationPair(
                rawlogFile, action, observations, rawlogEntry))
            break;  // file EOF

        if (rawlogEntry >= rawlog_offset)
        {
            // Update odometry:
            {
                CActionRobotMovement2D::Ptr act =
                    action->getBestMovementEstimation();
                if (act)
                    odoPose = odoPose + CPose3D(act->poseChange->getMeanVal());
                else
                {
                    CActionRobotMovement3D::Ptr act3D =
                        action->getActionByClass<CActionRobotMovement3D>();
                    if (act3D) odoPose = odoPose + act3D->poseChange.mean;
                }
            }

            double observations_timestamp_double = 0;
            if (observations && observations->size() > 0)
                observations_timestamp_double = mrpt::system::timestampTotime_t(
                    (*observations->begin())->timestamp);

            // Execute:
            // ----------------------------------------
            tictac.Tic();
            mapBuilder.processActionObservation(*action, *observations);
            t_exec = tictac.Tac();
            qDebug("Map building executed in %.03fms\n", 1000.0f * t_exec);

            // Info log:
            // -----------
            f_log.printf(
                "%u %f %i %i\n", static_cast<unsigned int>(step),
                1000.0f * t_exec, mapBuilder.getCurrentlyBuiltMapSize(),
                mapBuilder.m_statsLastIteration.observationsInserted ? int(1)
                                                                     : int(0));

            CPose3DPDF::Ptr curPDFptr = mapBuilder.getCurrentPoseEstimation();
            CPose3DPDFParticles curPDF;

            if (IS_CLASS(curPDFptr, CPose3DPDFParticles))
            {
                CPose3DPDFParticles::Ptr pp =
                    std::dynamic_pointer_cast<CPose3DPDFParticles>(curPDFptr);
                curPDF = *pp;
            }

            if (0 == (step % LOG_FREQUENCY))
            {
                const CMultiMetricMap* mostLikMap =
                    mapBuilder.mapPDF.getCurrentMostLikelyMetricMap();

                if (GENERATE_LOG_INFO)
                {
                    qDebug("Saving info log information...");

                    tictac_JH.Tic();

                    const CMultiMetricMap* avrMap =
                        mapBuilder.mapPDF.getAveragedMetricMapEstimation();
                    ASSERT_(avrMap->m_gridMaps.size() > 0);
                    COccupancyGridMap2D::Ptr grid = avrMap->m_gridMaps[0];
                    grid->computeEntropy(entropy);

                    grid->saveAsBitmapFile(
                        format("%s/EMMI_gridmap_%03u.bmp", OUT_DIR, step));

                    f_info.printf(
                        "%f %f %f %f %lu\n", entropy.I, entropy.H,
                        entropy.mean_I, entropy.effectiveMappedArea,
                        entropy.effectiveMappedCells);
                    qDebug(
                        "Ok\n EMI = %.04f    EMMI=%.04f (in %.03fms)\n",
                        entropy.I, entropy.mean_I, 1000.0f * tictac_JH.Tac());
                }

                // Pose log:
                // -------------
                if (SAVE_POSE_LOG)
                {
                    qDebug("Saving pose log information...");
                    curPDF.saveToTextFile(
                        format("%s/mapbuild_posepdf_%03u.txt", OUT_DIR, step));
                    qDebug("Ok\n");
                }

                // Map images:
                // -------------
                if (SAVE_MAP_IMAGES)
                {
                    qDebug("Saving map images to files...");

                    //  Most likely maps:
                    // ----------------------------------------
                    mostLikMap->saveMetricMapRepresentationToFile(
                        format(
                            "%s/mapbuilt_%05u_", OUT_DIR_MAPS.c_str(), step));

                    if (mostLikMap->m_gridMaps.size() > 0)
                    {
                        CImage img;
                        mapBuilder.drawCurrentEstimationToImage(&img);
                        img.saveToFile(
                            format("%s/mapping_%05u.png", OUT_DIR, step));
                    }

                    qDebug("Ok!\n");
                }

                // Save a 3D scene view of the mapping process:
                COpenGLScene::Ptr scene;
                if (SAVE_3D_SCENE || SHOW_PROGRESS_IN_WINDOW)
                {
                    scene = mrpt::make_aligned_shared<COpenGLScene>();

                    // The ground:
                    mrpt::opengl::CGridPlaneXY::Ptr groundPlane =
                        mrpt::make_aligned_shared<mrpt::opengl::CGridPlaneXY>(
                            -500, 500, -500, 500, 0, 1); //-150,150,-150,150,0,1)
                    groundPlane->setColor(0.4, 0.4, 0.4);
                    scene->insert(groundPlane);

                    // The camera pointing to the current robot pose:
                    if (CAMERA_3DSCENE_FOLLOWS_ROBOT)
                    {
                        mrpt::opengl::CCamera::Ptr objCam =
                            mrpt::make_aligned_shared<mrpt::opengl::CCamera>();
                        CPose3D robotPose;
                        curPDF.getMean(robotPose);

                        objCam->setPointingAt(robotPose);
                        //objCam->setAzimuthDegrees(-30);
                        //objCam->setElevationDegrees(10); //30
                        //objCam->setZoomDistance(15);  // add by test
                        scene->insert(objCam);
                    }
                    // Draw the map(s):
                    mrpt::opengl::CSetOfObjects::Ptr objs =
                        mrpt::make_aligned_shared<
                            mrpt::opengl::CSetOfObjects>();
                    mostLikMap->getAs3DObject(objs);
                    scene->insert(objs);

                    // Draw the robot particles:
                    size_t M = mapBuilder.mapPDF.particlesCount();
                    mrpt::opengl::CSetOfLines::Ptr objLines =
                        mrpt::make_aligned_shared<mrpt::opengl::CSetOfLines>();
                    objLines->setColor(0, 1, 1);
                    for (size_t i = 0; i < M; i++)
                    {
                        std::deque<TPose3D> path;
                        mapBuilder.mapPDF.getPath(i, path);

                        float x0 = 0, y0 = 0, z0 = 0;
                        for (size_t k = 0; k < path.size(); k++)
                        {
                            objLines->appendLine(
                                x0, y0, z0 + 0.001, path[k].x, path[k].y,
                                path[k].z + 0.001);
                            x0 = path[k].x;
                            y0 = path[k].y;
                            z0 = path[k].z;
                        }
                    }
                    scene->insert(objLines);

                    // An ellipsoid:
                    CPose3D lastMeanPose;
                    float minDistBtwPoses = -1;
                    std::deque<TPose3D> dummyPath;
                    mapBuilder.mapPDF.getPath(0, dummyPath);
                    for (int k = (int)dummyPath.size() - 1; k >= 0; k--)
                    {
                        CPose3DPDFParticles poseParts;
                        mapBuilder.mapPDF.getEstimatedPosePDFAtTime(
                            k, poseParts);

                        CPose3D meanPose;
                        CMatrixDouble66 COV;
                        poseParts.getCovarianceAndMean(COV, meanPose);

                        if (meanPose.distanceTo(lastMeanPose) > minDistBtwPoses)
                        {
                            CMatrixDouble33 COV3 = COV.block(0, 0, 3, 3);

                            minDistBtwPoses = 6 * sqrt(COV3(0, 0) + COV3(1, 1));

                            opengl::CEllipsoid::Ptr objEllip =
                                mrpt::make_aligned_shared<opengl::CEllipsoid>();
                            objEllip->setLocation(
                                meanPose.x(), meanPose.y(),
                                meanPose.z() + 0.001);
                            objEllip->setCovMatrix(
                                COV3, COV3(2, 2) == 0 ? 2 : 3);

                            objEllip->setColor(0, 0, 1);
                            objEllip->enableDrawSolid3D(false);
                            scene->insert(objEllip);

                            lastMeanPose = meanPose;
                        }
                    }
                }  // end if show or save 3D scene->

                if (SAVE_3D_SCENE)
                {  // Save as file:
                    CFileGZOutputStream(
                        format(
                            "%s/buildingmap_%05u.3Dscene", OUT_DIR_3D.c_str(),
                            step))
                        << *scene;
                }

                if (SHOW_PROGRESS_IN_WINDOW)
                {
//                    QtGL::azimuth = 25.0f;
//                    QtGL::elevation = -15.0f;
//                    QtGL::distance= 5.0f;


//                    // QtGL::theScene.getViewPort()->getCamera().
//                    QtGL::theScene.getViewport()->getCamera().setAzimuthDegrees(QtGL::azimuth);
//                    QtGL::theScene.getViewport()->getCamera().setElevationDegrees(QtGL::elevation);
//                    QtGL::theScene.getViewport()->getCamera().setZoomDistance(QtGL::distance);


                    COpenGLScene tmpScene(*scene);

                    QtGL::theScene = tmpScene;

//                    QtGL::azimuth = 25.0f;
//                    QtGL::elevation = -15.0f;
//                    QtGL::distance= 35.0f;


                    ui->widget->updateScene();

                //    ui->widget->repaint();
                   //QtGL::updateScene();
//                    COpenGLScene::Ptr& scenePtr = win3D->get3DSceneAndLock();
//                    scenePtr = scene;



//                    win3D->unlockAccess3DScene();

//                    win3D->forceRepaint();
//                    int add_delay =
//                        SHOW_PROGRESS_IN_WINDOW_DELAY_MS - t_exec * 1000;
//                    if (add_delay > 0)
//                        std::this_thread::sleep_for(
//                            std::chrono::milliseconds(add_delay));
                }


                // Save the weighted entropy of each map:
                // ----------------------------------------
                if (GENERATE_LOG_JOINT_H)
                {
                    qDebug("Saving joint H...");
                    tictac_JH.Tic();

                    double H_joint = mapBuilder.getCurrentJointEntropy();
                    double H_path =
                        mapBuilder.mapPDF.getCurrentEntropyOfPaths();
                    f_jinfo.printf("%e %e\n", H_joint, H_path);
                    qDebug(
                        "Ok\t joint-H=%f\t(in %.03fms)\n", H_joint,
                        1000.0f * tictac_JH.Tac());
                }

            }  // end of LOG_FREQ

            // Save the memory usage:
            // ------------------------------------------------------------------
            {
                qDebug("Saving memory usage...");
                unsigned long memUsage = getMemoryUsage();
                FILE* f = os::fopen(
                    format("%s/log_MemoryUsage.txt", OUT_DIR).c_str(), "at");
                if (f)
                {
                    os::fprintf(f, "%u\t%lu\n", step, memUsage);
                    os::fclose(f);
                }
                qDebug("Ok! (%.04fMb)\n", ((float)memUsage) / (1024 * 1024));
            }

            // Save the parts stats:
            f_partStats.printf(
                "%u %u %f\n", (unsigned int)step, (unsigned int)curPDF.size(),
                curPDF.ESS());

            // Save the robot estimated pose for each step:
            CPose3D meanPose;
            mapBuilder.getCurrentPoseEstimation()->getMean(meanPose);

            f_path.printf(
                "%i %f %f %f %f %f %f %f\n", (int)rawlogEntry, meanPose.x(),
                meanPose.y(), meanPose.z(), meanPose.yaw(), meanPose.pitch(),
                meanPose.roll(), observations_timestamp_double);

            f_pathOdo.printf(
                "%i\t%f\t%f\t%f\t%f\t%f\t%f\n", step, odoPose.x(), odoPose.y(),
                odoPose.z(), odoPose.yaw(), odoPose.pitch(), odoPose.roll());

        }  // end of if "rawlog_offset"...

        step++;
        qDebug(
            "\n---------------- STEP %u | RAWLOG ENTRY %u ----------------\n",
            step, (unsigned)rawlogEntry);

        // Free memory:
        action.reset();
        observations.reset();
    };  // end while

    qDebug(
        "\n---------------- END!! (total time: %.03f sec) ----------------\n",
        tictacGlobal.Tac());

    // Save map:
    mapBuilder.getCurrentlyBuiltMap(finalMap);

    CFileOutputStream filOut(format("%s/_finalmap_.simplemap", OUT_DIR));
    filOut << finalMap;

    // Save gridmap extend (if exists):
    const CMultiMetricMap* mostLikMap =
        mapBuilder.mapPDF.getCurrentMostLikelyMetricMap();
    if (mostLikMap->m_gridMaps.size() > 0)
    {
        CMatrix auxMat(1, 4);
        auxMat(0, 0) = mostLikMap->m_gridMaps[0]->getXMin();
        auxMat(0, 1) = mostLikMap->m_gridMaps[0]->getXMax();
        auxMat(0, 2) = mostLikMap->m_gridMaps[0]->getYMin();
        auxMat(0, 3) = mostLikMap->m_gridMaps[0]->getYMax();
        auxMat.saveToTextFile(
            format("%s/finalGridmapSize.txt", OUT_DIR), MATRIX_FORMAT_FIXED);
    }

    // Save the most likely path of the particle set
    FILE* f_pathPart;

    os::sprintf(strFil, 1000, "%s/most_likely_path.txt", OUT_DIR);
    f_pathPart = os::fopen(strFil, "wt");

    ASSERT_(f_pathPart != nullptr);

    std::deque<TPose3D> outPath;
    std::deque<TPose3D>::iterator itPath;

    mapBuilder.getCurrentMostLikelyPath(outPath);

    for (itPath = outPath.begin(); itPath != outPath.end(); itPath++)
        os::fprintf(
            f_pathPart, "%.3f %.3f %.3f\n", itPath->x, itPath->y, itPath->yaw);

    os::fclose(f_pathPart);

    // Free gas readings memory (if any):
    //gasObservations.clear();

    // Free wifi readings memory (if any):
    //wifiObservations.clear();

//     Close 3D window, if any:
//    if (win3D)
//    {
//        mrpt::system::pause();
//        delete win3D;
//        win3D = nullptr;
//    }

    MRPT_END
}

void MainWindow::on_btnSlamStart_clicked()
{
    qDebug()<<" on_btnSlamStart_clicked debug";
    ui->statusBar->showMessage( mrpt::format("重新开始创建地图... ")
                                   .c_str());

    initMapBuildingParams();
    MapBuilding_RBPF();
}

void MainWindow::on_btnSlamEnd_clicked()
{

   qDebug()<<"end build map ";

}


// ------------------------------------------------------
//					Navigaion map
// ------------------------------------------------------
// ==== holonomic_navigator_demoFrame::TOptions ======
MainWindow::TOptions::TOptions()
    : ROBOT_MAX_SPEED(4.0),
      MAX_SENSOR_RADIUS(5.0),
      SENSOR_NUM_RANGES(181),
      SENSOR_RANGE_NOISE_STD(0.02)
{
}

void MainWindow::TOptions::loadFromConfigFile(
    const mrpt::utils::CConfigFileBase& source, const std::string& section)
{
    MRPT_START

    // Load from config text:
    MRPT_LOAD_CONFIG_VAR(ROBOT_MAX_SPEED, double, source, section);
    MRPT_LOAD_CONFIG_VAR(MAX_SENSOR_RADIUS, double, source, section);
    MRPT_LOAD_CONFIG_VAR(SENSOR_NUM_RANGES, uint64_t, source, section);
    MRPT_LOAD_CONFIG_VAR(SENSOR_RANGE_NOISE_STD, double, source, section);

    MRPT_END
}

void MainWindow::TOptions::saveToConfigFile(
    mrpt::utils::CConfigFileBase& cfg, const std::string& section) const
{
    MRPT_START
    const int WN = 40, WV = 20;

    cfg.write(
        section, "ROBOT_MAX_SPEED", ROBOT_MAX_SPEED, WN, WV,
        "Maximum speed for the robot (m/s)");
    cfg.write(
        section, "MAX_SENSOR_RADIUS", MAX_SENSOR_RADIUS, WN, WV,
        "Maximum range of the 360deg sensor (meters)");
    cfg.write(
        section, "SENSOR_NUM_RANGES", SENSOR_NUM_RANGES, WN, WV,
        "Number of ranges in the 360deg sensor FOV");
    cfg.write(
        section, "SENSOR_RANGE_NOISE_STD", SENSOR_RANGE_NOISE_STD, WN, WV,
        "Sensor noise (one sigma, in meters)");

    MRPT_END
}


void MainWindow::initNavitionParmas(){



    COpenGLScene m_plot3D = QtGL::theScene;
    //m_plot3D.clear();

    QtGL::theScene.clear();
    // Initialize gridmap:
    // -------------------------------
    CMemoryStream s(DEFAULT_GRIDMAP_DATA, sizeof(DEFAULT_GRIDMAP_DATA));
    s >> m_gridMap;

    // auto openGLSceneRef = &m_plot3D;
    // Populate 3D views:
    // -------------------------------
    {
        mrpt::opengl::CGridPlaneXY::Ptr obj =
                mrpt::make_aligned_shared<mrpt::opengl::CGridPlaneXY>(
                    -50, 50, -50, 50, 0, 1);
        obj->setColor_u8(TColor(30, 30, 30, 50));
        //openGLSceneRef->insert(obj);
        // m_plot3D.insert(obj);
        QtGL::theScene.insert(obj);
    }

    gl_grid = mrpt::make_aligned_shared<mrpt::opengl::CSetOfObjects>();
    QtGL::theScene.insert(gl_grid);
    this->updateMap3DView();

    gl_robot = mrpt::make_aligned_shared<mrpt::opengl::CSetOfObjects>();
    {
        mrpt::opengl::CCylinder::Ptr obj =
                mrpt::make_aligned_shared<mrpt::opengl::CCylinder>(
                    0.2f, 0.1f, 0.9f);
        obj->setColor_u8(TColor::red());
        gl_robot->insert(obj);
    }
    QtGL::theScene.insert(gl_robot);
    qDebug()<<" insert an robot";

    gl_scan3D = mrpt::make_aligned_shared<mrpt::opengl::CPlanarLaserScan>();
    gl_scan3D->enableLine(false);
    gl_scan3D->setPointsWidth(3.0);
    gl_robot->insert(gl_scan3D);

    gl_robot_sensor_range =
            mrpt::make_aligned_shared<mrpt::opengl::CDisk>(0, 0);
    gl_robot_sensor_range->setColor_u8(TColor(0, 0, 255, 90));
    gl_robot_sensor_range->setLocation(0, 0, 0.01);
    gl_robot->insert(gl_robot_sensor_range);

    gl_robot_path = mrpt::make_aligned_shared<mrpt::opengl::CSetOfLines>();
    gl_robot_path->setLineWidth(1);
    gl_robot_path->setColor_u8(TColor(40, 40, 40, 200));
    QtGL::theScene.insert(gl_robot_path);


    gl_target = mrpt::make_aligned_shared<mrpt::opengl::CSetOfObjects>();
    {
        mrpt::opengl::CArrow::Ptr obj;
        obj = mrpt::make_aligned_shared<mrpt::opengl::CArrow>(
                    1, 0, 0, 0.2f, 0, 0, 0.4f, 0.05f, 0.15f);
        obj->setColor_u8(TColor(0, 0, 255));
        gl_target->insert(obj);
        obj = mrpt::make_aligned_shared<mrpt::opengl::CArrow>(
                    -1, 0, 0, -0.2f, 0, 0, 0.4f, 0.05f, 0.15f);
        obj->setColor_u8(TColor(0, 0, 255));
        gl_target->insert(obj);
        obj = mrpt::make_aligned_shared<mrpt::opengl::CArrow>(
                    0, 1, 0, 0, 0.2f, 0, 0.4f, 0.05f, 0.15f);
        obj->setColor_u8(TColor(0, 0, 255));
        gl_target->insert(obj);
        obj = mrpt::make_aligned_shared<mrpt::opengl::CArrow>(
                    0, -1, 0, 0, -0.2f, 0, 0.4f, 0.05f, 0.15f);
        obj->setColor_u8(TColor(0, 0, 255));
        gl_target->insert(obj);
        QtGL::theScene.insert(gl_target);
    }

    {  // Sign of "picking a navigation target":
        m_gl_placing_nav_target =
                mrpt::make_aligned_shared<opengl::CSetOfObjects>();

        mrpt::opengl::CArrow::Ptr obj;
        obj = mrpt::make_aligned_shared<mrpt::opengl::CArrow>(
                    1, 0, 0, 0.2f, 0, 0, 0.4f, 0.05f, 0.15f);
        obj->setColor_u8(TColor(0, 0, 255));
        m_gl_placing_nav_target->insert(obj);
        obj = mrpt::make_aligned_shared<mrpt::opengl::CArrow>(
                    -1, 0, 0, -0.2f, 0, 0, 0.4f, 0.05f, 0.15f);
        obj->setColor_u8(TColor(0, 0, 255));
        m_gl_placing_nav_target->insert(obj);
        obj = mrpt::make_aligned_shared<mrpt::opengl::CArrow>(
                    0, 1, 0, 0, 0.2f, 0, 0.4f, 0.05f, 0.15f);
        obj->setColor_u8(TColor(0, 0, 255));
        m_gl_placing_nav_target->insert(obj);
        obj = mrpt::make_aligned_shared<mrpt::opengl::CArrow>(
                    0, -1, 0, 0, -0.2f, 0, 0.4f, 0.05f, 0.15f);
        obj->setColor_u8(TColor(0, 0, 255));
        m_gl_placing_nav_target->insert(obj);
        m_gl_placing_nav_target->setVisibility(false);  // Start invisible.
        QtGL::theScene.insert(m_gl_placing_nav_target);
    }


    {  // Sign of "replacing the robot":
        m_gl_placing_robot = mrpt::make_aligned_shared<opengl::CSetOfObjects>();
        mrpt::opengl::CCylinder::Ptr obj =
                mrpt::make_aligned_shared<mrpt::opengl::CCylinder>(
                    0.2f, 0.1f, 0.9f);
        obj->setColor_u8(TColor(255, 0, 0, 120));
        m_gl_placing_robot->insert(obj);

        m_gl_placing_robot->setVisibility(false);  // Start invisible.
        QtGL::theScene.insert(m_gl_placing_robot);
    }

    QtGL::theScene.insert(mrpt::opengl::stock_objects::CornerXYZ(1));


    QtGL::theScene.getViewport()->getCamera().setLocation(0.0, 0.0, 0.0);
    QtGL::theScene.getViewport()->getCamera().setZoomDistance(40.0f);
    QtGL::theScene.getViewport()->getCamera().setElevationDegrees(70.0f);
    QtGL::theScene.getViewport()->getCamera().setAzimuthDegrees(-100.0f);
    QtGL::theScene.getViewport()->getCamera().setProjectiveModel(true);

    ui->widget->updateScene();


    // 2D view ==============
    // auto openGLScanRef = m_plotScan->getOpenGLSceneRef();
    {
        mrpt::opengl::CGridPlaneXY::Ptr obj =
                mrpt::make_aligned_shared<mrpt::opengl::CGridPlaneXY>(
                    -1, 1.001f, -1, 1.001f, 0, 1);
        obj->setColor_u8(TColor(30, 30, 30, 50));
        QtGL::theScene.insert(obj);
    }

    gl_scan2D = mrpt::make_aligned_shared<mrpt::opengl::CPlanarLaserScan>();
    gl_scan2D->enableLine(false);
    gl_scan2D->enableSurface(false);
    gl_scan2D->setPointsWidth(3.0);
    QtGL::theScene.insert(gl_scan2D);

    gl_line_direction = mrpt::make_aligned_shared<mrpt::opengl::CSimpleLine>();
    gl_line_direction->setLineWidth(4);
    gl_line_direction->setColor_u8(TColor(0, 0, 0));
    QtGL::theScene.insert(gl_line_direction);

    gl_rel_target = mrpt::make_aligned_shared<mrpt::opengl::CPointCloud>();
    gl_rel_target->setPointSize(7);
    gl_rel_target->setColor_u8(TColor(0, 0, 255));
    gl_rel_target->insertPoint(0, 0, 0);
    QtGL::theScene.insert(gl_rel_target);

    QtGL::theScene.insert(mrpt::opengl::stock_objects::CornerXYSimple(0.1f, 2));

    gl_nd_gaps = mrpt::make_aligned_shared<mrpt::opengl::CSetOfLines>();
    gl_nd_gaps->setLineWidth(2);
    gl_nd_gaps->setColor_u8(TColor(204, 102, 51));
    QtGL::theScene.insert(gl_nd_gaps);


    ui->widget->updateScene();

    QtGL::theScene.getViewport()->getCamera().setLocation(0.0, 0.0, 0.0);
    QtGL::theScene.getViewport()->getCamera().setZoomDistance(2.2f);
    QtGL::theScene.getViewport()->getCamera().setElevationDegrees(90.0f);
    QtGL::theScene.getViewport()->getCamera().setAzimuthDegrees(-90.0f);
    QtGL::theScene.getViewport()->getCamera().setProjectiveModel(false);

    // Update positions of stuff:
    this->updateViewsDynamicObjects();
   // ui->widget->updateScene();

    // Retrieve default parameters for holonomic methods:
        // ------------------------------------------------------
        {
            mrpt::utils::CConfigFileMemory cfg;

            m_simul_options.saveToConfigFile(cfg, "SIMULATOR");

            mrpt::nav::CHolonomicVFF holo_VFF;
            holo_VFF.options.saveToConfigFile(cfg, "VFF_CONFIG");

            mrpt::nav::CHolonomicND holo_ND;
            holo_ND.options.saveToConfigFile(cfg, "ND_CONFIG");


            ui->etConfig->setText(cfg.getContent().c_str());

        }


}


// Run simulator (when "running"):
void MainWindow::OntimRunSimulTrigger()
{
   qDebug()<<" OntimRunSimulTrigger :  " ;
    try
    {
        if (ui->btnNavEnd->isEnabled())
        {
            simulateOneStep(1000 * 1e-3);
            qDebug()<<"timer 1 trigger: once OntimRunSimulTrigger :  " ;
        }
        updateViewsDynamicObjects();
    }
    catch (std::exception& e)
    {
        qDebug()<<e.what()<<" exception ontimRunSimulTrigger";
        on_btnNavEnd_clicked();
    }
}

// Create navigator object & load params from GUI:
void MainWindow::reinitSimulator()
{


    // Delete old & build new navigator:
    m_holonomicMethod.reset();
    int set =0;
    switch (set)
    {
        case 0:
            m_holonomicMethod.reset(new mrpt::nav::CHolonomicVFF);
            break;
        case 1:
            m_holonomicMethod.reset(new mrpt::nav::CHolonomicND);
            break;
        default:
            throw std::runtime_error("Invalid holonomic method selected!");
    };

    // Load params:
    {
        CConfigFileMemory cfg;
        cfg.setContent(ui->etConfig->toPlainText().toStdString());
        m_holonomicMethod->initialize(cfg);

        m_simul_options.loadFromConfigFile(cfg, "SIMULATOR");
    }

    // Update GUI stuff:
    gl_robot_sensor_range->setDiskRadius(
        m_simul_options.MAX_SENSOR_RADIUS * 1.01,
        m_simul_options.MAX_SENSOR_RADIUS * 0.99);

}


void MainWindow::updateMap3DView()
{
    gl_grid->clear();
    m_gridMap.getAs3DObject(gl_grid);
}


void MainWindow::simulateOneStep(double time_step)
{
    // Simulate 360deg range scan:
    CObservation2DRangeScan simulatedScan;

    simulatedScan.aperture = M_2PIf;
    simulatedScan.rightToLeft = true;
    simulatedScan.maxRange = m_simul_options.MAX_SENSOR_RADIUS;
    simulatedScan.sensorPose = CPose3D();

    m_gridMap.laserScanSimulator(
        simulatedScan, mrpt::poses::CPose2D(m_robotPose), 0.5,
        m_simul_options.SENSOR_NUM_RANGES,
        m_simul_options.SENSOR_RANGE_NOISE_STD);

    gl_scan3D->setScan(simulatedScan);  // Draw real scan in 3D view

    // Normalize:
    for (size_t j = 0; j < simulatedScan.scan.size(); j++)
        simulatedScan.setScanRange(
            j, simulatedScan.getScanRange(j) / simulatedScan.maxRange);

    gl_scan2D->setScan(simulatedScan);  // Draw scaled scan in right-hand view

    // Navigate:
    mrpt::math::TPoint2D relTargetPose = mrpt::math::TPoint2D(
        mrpt::poses::CPoint2D(m_targetPoint) -
        mrpt::poses::CPose2D(m_robotPose));
    relTargetPose *=
        1.0 / simulatedScan.maxRange;  // Normalized relative target:

    // tictac.Tic();
    CAbstractHolonomicReactiveMethod::NavOutput no;
    CAbstractHolonomicReactiveMethod::NavInput ni;
    ni.targets.push_back(relTargetPose);

    ni.obstacles.resize(simulatedScan.getScanSize());
    for (unsigned int i = 0; i < ni.obstacles.size(); i++)
        ni.obstacles[i] = simulatedScan.getScanRange(i);

    ni.maxRobotSpeed = m_simul_options.ROBOT_MAX_SPEED;
    ni.maxObstacleDist = m_simul_options.MAX_SENSOR_RADIUS;

    this->m_holonomicMethod->navigate(ni, no);

    mrpt::nav::CHolonomicLogFileRecord::Ptr out_log = no.logRecord;

    // Move robot:
    m_robotPose.x += cos(no.desiredDirection) * no.desiredSpeed * time_step;
    m_robotPose.y += sin(no.desiredDirection) * no.desiredSpeed * time_step;

    qDebug("simulateOneStep: Robot: (%.03f,%.03f) ->  Target: (%.03f,%.03f) :fangzheng: (%.03f,%.03f) ",
            m_robotPose.x, m_robotPose.y,m_targetPoint.x, m_targetPoint.y,no.desiredDirection,no.desiredSpeed);


    // Update path graph:
    const TPoint3D cur_pt(m_robotPose.x, m_robotPose.y, 0.01);

    if (gl_robot_path->empty())
        gl_robot_path->appendLine(cur_pt, cur_pt);
    else
        gl_robot_path->appendLineStrip(cur_pt);

    gl_rel_target->setLocation(relTargetPose.x, relTargetPose.y, 0);

    // Clear stuff which will be updated if used below:
    //edInfoLocalView->Clear();
    gl_nd_gaps->clear();

    // Update 2D view graphs:
    if (out_log && IS_CLASS(out_log, CLogFileRecord_ND))
    {
        CLogFileRecord_ND::Ptr log =
            std::dynamic_pointer_cast<CLogFileRecord_ND>(out_log);
        const size_t nGaps = log->gaps_ini.size();

        const string sSitu =
            mrpt::utils::TEnumType<CHolonomicND::TSituations>::value2name(
                log->situation);

        string sLog = mrpt::format("ND situation : %s\n", sSitu.c_str());
        sLog += mrpt::format(
            "Gap count    : %u\n", static_cast<unsigned int>(nGaps));

       // edInfoLocalView->SetValue(_U(sLog.c_str()));

        gl_nd_gaps->appendLine(0, 0, 0, 0, 0, 0);

        for (size_t i = 0; i < nGaps; i++)
        {
            const size_t N_STEPS = 20;
            for (size_t j = 0; j < N_STEPS; j++)
            {
                const double sec = log->gaps_ini[i] +
                                   j * (log->gaps_end[i] - log->gaps_ini[i]) /
                                       static_cast<double>(N_STEPS - 1);
                const double ang =
                    M_PI * (-1 + 2 * sec / ((float)simulatedScan.scan.size()));
                const double d = simulatedScan.scan[sec] - 0.05;
                gl_nd_gaps->appendLineStrip(d * cos(ang), d * sin(ang), 0);
            }
            gl_nd_gaps->appendLineStrip(0, 0, 0);
        }
    }

    // Movement direction:
    const double d = no.desiredSpeed / m_simul_options.ROBOT_MAX_SPEED;
    gl_line_direction->setLineCoords(
        0, 0, 0, cos(no.desiredDirection) * d, sin(no.desiredDirection) * d, 0);
}

void MainWindow::updateViewsDynamicObjects()
{
    gl_robot->setLocation(m_robotPose.x, m_robotPose.y, 0);

    QString tmpStrX = QString("%1").arg(m_robotPose.x,0,'f',3);
    QString tmpStrY = QString("%1").arg(m_robotPose.y,0,'f',3);

    ui->lineEditStartPointX->setText(tmpStrX);
    ui->lineEditStartPointY->setText(tmpStrY);

    // animate target: //
    {
        const double TARGET_BOUNCE_MIN = 0.7;
        const double TARGET_BOUNCE_MAX = 1;

        const double TARGET_BOUNCE_PERIOD = 1.0;
        const double t =
                fmod(m_runtime.Tac(), TARGET_BOUNCE_PERIOD) / TARGET_BOUNCE_PERIOD;

        // Parabolic path
        const double s =
                4 * t * (TARGET_BOUNCE_MAX - TARGET_BOUNCE_MIN) * (1 - t) +
                TARGET_BOUNCE_MIN;

        gl_target->setLocation(m_targetPoint.x, m_targetPoint.y, 0);
        gl_target->setScale(s);
    }

    ui->statusBar->showMessage( mrpt::format("Robot: (%.03f,%.03f)", m_robotPose.x, m_robotPose.y)
                                .c_str());

    qDebug("updateViewsDynamicObjects: Robot: (%.03f,%.03f) ->  Target: (%.03f,%.03f) ",
            m_robotPose.x, m_robotPose.y,m_targetPoint.x, m_targetPoint.y);


    ui->statusBar->showMessage(mrpt::format("Target: (%.03f,%.03f)", m_targetPoint.x, m_targetPoint.y)
                               .c_str());

    ui->widget->updateScene();
   // ui->widget->repaint();

    // Show/hide:
     gl_robot_sensor_range->setVisibility(true);
     gl_robot_path->setVisibility(true);
}

void MainWindow::on_btnNavLoadMap_clicked(){
    initNavitionParmas();
    qDebug()<<"init Navition params";
}


void MainWindow::on_btnNavStart_clicked()
{
    timRunSimul->start(1000);
    qDebug()<<"start navigation";
   reinitSimulator();

   ui->btnNavStart->setEnabled(false);
   ui->btnNavStart->update();
   ui->btnNavEnd->setEnabled(true);
   ui->btnNavEnd->update();

}

void MainWindow::on_btnNavEnd_clicked()
{
    timRunSimul->stop();

    ui->btnNavStart->setEnabled(true);
    ui->btnNavStart->update();
    ui->btnNavEnd->setEnabled(false);
    ui->btnNavEnd->update();
    qDebug()<<"end navigation";

}

void MainWindow::on_btnNavStartPoint_clicked()
{
    ui->btnNavEndPoint->setEnabled(true);
    ui->btnNavEndPoint->update();

    if (m_cursorPickState != cpsPlaceRobot)
        {
            m_cursorPickState = cpsPlaceRobot;
            //QtGL::theScene.getViewport()->getCamera();
            ui->widget->setCursor(Qt::CrossCursor);

            //m_plot3D->SetCursor(*wxCROSS_CURSOR);
        }
        else
        {  // Cancel:
            m_cursorPickState = cpsNone;
            //m_plot3D->SetCursor(*wxSTANDARD_CURSOR);
            ui->widget->setCursor(Qt::ArrowCursor);
            m_gl_placing_robot->setVisibility(false);
        }
}

void MainWindow::on_btnNavEndPoint_clicked()
{
    ui->btnNavStartPoint->setEnabled(true);
    ui->btnNavStartPoint->update();

    if (m_cursorPickState != cpsPickTarget)
        {
            m_cursorPickState = cpsPickTarget;
             ui->widget->setCursor(Qt::CrossCursor);
            //m_plot3D->SetCursor(*wxCROSS_CURSOR);
        }
        else
        {  // Cancel:
            m_cursorPickState = cpsNone;
           // m_plot3D->SetCursor(*wxSTANDARD_CURSOR);
            ui->widget->setCursor(Qt::ArrowCursor);
            m_gl_placing_nav_target->setVisibility(false);
        }
}


void MainWindow::on_btnNavMap_clicked()
{
    // tr("All files (*.*);;GRIDMAP(*.gridmap)") ,"GRIDMAP (*.gridmap)");
    QString name = QFileDialog::getOpenFileName(this, "选择栅格地图打开","/home",tr("Images (*.png *.jpg *.gif *.png *.gif);;GRIDMAP(*.gridmap)"));
    qDebug("load map from : %s",name);

    if(name.isEmpty())    {
        return;
    }



        const std::string fil = std::string(name.toStdString());

        const std::string fil_ext = mrpt::system::extractFileExtension(fil, true);

        if (mrpt::system::lowerCase(fil_ext) == "gridmap")
        {
            CFileGZInputStream f(fil);
            f >> m_gridMap;
        }
        else
        {
            // Try loading the image:
            CImage img;
            if (!img.loadFromFile(fil, 0 /* force grayscale */))
            {
                QMessageBox::warning(this,tr("Error"),tr("Can't load the image file (check its format)."));

            }
            else
            {
                // We also need the size of each pixel:
                double cx = -1;
                double cy = -1;
                double cell_size = 0.05;

                bool ok;


                    if (!m_gridMap.loadFromBitmap(img, cell_size, cx, cy)){
                      QMessageBox::warning(this,tr("Error"),tr("Can't load the image file into the gridmap..."));
                    }


            }
        }

        updateMap3DView();
        ui->widget->updateScene();

}

void MainWindow::on_btnSetRobotNavX_clicked()
{
    float robotStartPointX = ui->lineEditStartPointX->text().toFloat();
    float robotStartPointY = ui->lineEditStartPointY->text().toFloat();

    m_robotPose.x = robotStartPointX;
    m_robotPose.y = robotStartPointY;
    m_gl_placing_robot->setVisibility(true);
    m_gl_placing_robot->setLocation(
                robotStartPointX, robotStartPointY, 0.05);
    qDebug("nav robot point  X=%.03f Y=%.04f Z=0", robotStartPointX, robotStartPointY);
}

void MainWindow::on_btnSetRobotNavY_clicked()
{
    float robotEndPointX = ui->lineEditEndPointX->text().toFloat();
    float robotEndPointY = ui->lineEditEndPointY->text().toFloat();

    qDebug("place the robot target point : %.03f %.03f",robotEndPointX,robotEndPointY);

    m_targetPoint.x = robotEndPointX;
    m_targetPoint.y = robotEndPointY;

    m_gl_placing_nav_target->setVisibility(true);
    m_gl_placing_nav_target->setLocation(
                m_targetPoint.x, m_targetPoint.y, 0.05);
    qDebug("nav target point  X=%.03f Y=%.04f Z=0",  m_targetPoint.x, m_targetPoint.y);

}
