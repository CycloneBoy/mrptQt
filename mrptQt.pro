#-------------------------------------------------
#
# Project created by QtCreator 2017-12-14T10:49:55
#
#-------------------------------------------------

QT       += core gui widgets opengl


CONFIG   += c++14
#QMAKE_CXXFLAGS += -std=c++1y


TARGET = mrptQt
TEMPLATE = app




SOURCES +=  \
            main.cpp \
    HelpDialog.cpp \
    MainWindow.cpp \
    mrptOne.cpp \
    PropertyEditor.cpp \
    QtGL.cpp




HEADERS  += \
    HelpDialog.hpp \
    MainWindow.hpp \
    mrptOne.h \
    PropertyEditor.hpp \
    QtGL.hpp

FORMS    += \
    MainWindow.ui \
    Propertyeditor.ui \
    Helpdialog.ui


# MRPT : base;gui;opengl;graphs;vision;maps;slam;obs;nav;kinematics)
INCLUDEPATH += /usr/include/mrpt/base/include \
                /usr/include/mrpt/gui/include \
                /usr/include/mrpt/opengl/include \
                /usr/include/mrpt/graphs/include \
                /usr/include/mrpt/vision/include \
                /usr/include/mrpt/maps/include \
                /usr/include/mrpt/obs/include \
                /usr/include/mrpt/nav/include \
                /usr/include/mrpt/slam/include \
                /usr/include/mrpt/kinematics/include \
                /usr/include/mrpt/mrpt-config \
                /usr/include/eigen3 \
                /usr/include/GL


#LIBS += -L/usr/lib/x86_64-linux-gnu -lmrpt-gui -lmrpt-base -lmrpt-opengl -lmrpt-graphs -lmrpt-vision

LIBS += -L/usr/lib/x86_64-linux-gnu -lmrpt-base -lmrpt-gui -lmrpt-opengl -lmrpt-nav -lmrpt-kinematics\
                                    -lmrpt-graphs -lmrpt-vision -lmrpt-maps -lmrpt-obs -lmrpt-slam\
                                    -lglut -lGL -lGLU



RESOURCES += \
    resources.qrc



