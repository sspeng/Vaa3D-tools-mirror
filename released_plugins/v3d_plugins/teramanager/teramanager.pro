#last edit: by PHC, 20130722. make sure the static link of opencv works

#generic set up

TEMPLATE    = lib
CONFIG  += qt plugin warn_off
QT += opengl
CONFIG += use_static_libs
CONFIG += use_experimental_features
DEFINES += QT_NO_DEBUG_STREAM
DEFINES += QT_NO_DEBUG_OUTPUT
DEFINES += QT_NO_WARNING_OUTPUT
#DEFINES += _USE_QT_DIALOGS
#DEFINES += terafly_enable_debug_max_level

#QMAKE_CXXFLAGS += -Wall
#QMAKE_CXXFLAGS += -pedantic
#QMAKE_CXXFLAGS += -Werror


#set up Vaa3D and Qt source path
win32{
    V3DMAINPATH =  ../../../../v3d_external/v3d_main
}
else{
    V3DMAINPATH =  ../../../v3d_main
}
QT_PATH = $$dirname(QMAKE_QMAKE)/..


#set up third party libraries under MacOS and Linux
use_static_libs{
    INCLUDEPATH += ./include/opencv
    INCLUDEPATH += ./include
    mac{
        LIBS += -L"$$_PRO_FILE_PWD_/lib/opencv/mac_x86_64"
        LIBS += -L"$$_PRO_FILE_PWD_/lib/opencv/mac_x86_64/3rdparty"
    }
    unix:!mac{
        LIBS += -L"$$_PRO_FILE_PWD_/lib/opencv/unix_x86_64"
        LIBS += -L"$$_PRO_FILE_PWD_/lib/opencv/unix_x86_64/3rdparty"
    }
    win32{
        #WARNING! These precompiled libraries must match with your VC (vc9, vc10 or vc11) compiler!
        LIBS += -L"$$_PRO_FILE_PWD_/lib/opencv/win64/vc9"
        LIBS += -L"$$_PRO_FILE_PWD_/lib/opencv/win64/vc9/3rdparty"

        #on VC, OpenCV precompiled libraries have further dependencies
        LIBS+=-llibpng
        LIBS+=-ladvapi32
        LIBS+=-lcomctl32
    }
} else{
    #OpenCV headers and library folders
    INCLUDEPATH += /usr/local/include/opencv
    LIBS+= -L/usr/local/lib
}
LIBS+= -lopencv_core -lopencv_imgproc -lopencv_highgui \
 -lzlib \
 -llibtiff \
 -llibjpeg \
 -lIlmImf \
 -llibjasper 


#set up Vaa3D stuff needed by the plugin
win32{

    #the directory where files like "ui_template_matching_cellseg.h", "moc*.cxx" and similar were generated by CMake/QMake + Visual Studio
    V3DBUILDPATH = $$V3DMAINPATH/../../build_cmake_vc9/v3d_main
    INCLUDEPATH += $$V3DBUILDPATH/v3d
    INCLUDEPATH += $$V3DBUILDPATH/v3dbase

    #libtiff API
    INCLUDEPATH += $$V3DMAINPATH/common_lib/src_packages/tiff-4.0.1/libtiff

    #disabling experimental features if having troubles with "moc_landmark_property_dialog.cpp" and similar or with an endless sea of unresolved external symbols
    CONFIG -= use_experimental_features

    #Vaa3D and OpenCV libraries are compiled using the multithread-DLL version of the run-time library
    CONFIG(debug, debug|release) {
        QMAKE_CXXFLAGS += /MDd
        QMAKE_CXXFLAGS += /NODEFAULTLIB:MSVCRT
    }
    CONFIG(release, debug|release) {
        QMAKE_CXXFLAGS += /MD
    }

    #this solves undefined symbols like "Read_LSM_Stack" and similar
    SOURCES += $$V3DMAINPATH/basic_c_fun/mg_image_lib.cpp

    #on Windows 7+ the best option is to use native fonts
    DEFINES += _USE_NATIVE_FONTS
}

#Vaa3D headers and sources needed by the plugin
INCLUDEPATH+= $$QT_PATH/demos/shared
INCLUDEPATH += $$V3DMAINPATH/basic_c_fun
INCLUDEPATH += $$V3DMAINPATH/3drenderer
INCLUDEPATH += $$V3DMAINPATH/common_lib/include
INCLUDEPATH += $$V3DMAINPATH/basic_c_fun/customary_structs
SOURCES += $$V3DMAINPATH/basic_c_fun/v3d_message.cpp
SOURCES += $$V3DMAINPATH/basic_c_fun/stackutil.cpp
SOURCES += $$V3DMAINPATH/basic_c_fun/mg_utilities.cpp
SOURCES += $$V3DMAINPATH/basic_c_fun/basic_memory.cpp
SOURCES += $$V3DMAINPATH/basic_c_fun/basic_4dimage.cpp
SOURCES += $$V3DMAINPATH/basic_c_fun/basic_surf_objs.cpp

#Vaa3D sources needed to use experimental features
use_experimental_features{
    DEFINES += USE_EXPERIMENTAL_FEATURES
    INCLUDEPATH += $$V3DMAINPATH
    INCLUDEPATH += $$V3DMAINPATH/v3d
    SOURCES += $$V3DMAINPATH/3drenderer/renderer.cpp
    SOURCES += $$V3DMAINPATH/3drenderer/GLee_r.c
    SOURCES += $$V3DMAINPATH/3drenderer/renderer_hit.cpp
    SOURCES += $$V3DMAINPATH/v3d/landmark_property_dialog.cpp
    SOURCES += $$V3DMAINPATH/v3d/surfaceobj_geometry_dialog.cpp
    SOURCES += $$V3DMAINPATH/jba/newmat11/newmat1.cpp
    SOURCES += $$V3DMAINPATH/jba/newmat11/newmat2.cpp
    SOURCES += $$V3DMAINPATH/jba/newmat11/newmat3.cpp
    SOURCES += $$V3DMAINPATH/jba/newmat11/newmat4.cpp
    SOURCES += $$V3DMAINPATH/jba/newmat11/newmat5.cpp
    SOURCES += $$V3DMAINPATH/jba/newmat11/newmat6.cpp
    SOURCES += $$V3DMAINPATH/jba/newmat11/newmat7.cpp
    SOURCES += $$V3DMAINPATH/jba/newmat11/newmat8.cpp
    SOURCES += $$V3DMAINPATH/jba/newmat11/newmat9.cpp
    SOURCES += $$V3DMAINPATH/jba/newmat11/newmatex.cpp
    SOURCES += $$V3DMAINPATH/jba/newmat11/bandmat.cpp       # @ADDED by Alessandro on 2014-03-29: fixed "Failed instantiation" error on Ubuntu 13.10
    SOURCES += $$V3DMAINPATH/jba/newmat11/myexcept.cpp      # @ADDED by Alessandro on 2014-03-29: fixed "Failed instantiation" error on Ubuntu 13.10
    SOURCES += $$V3DMAINPATH/3drenderer/v3dr_glwidget.cpp

    #WARNING! win32 moc*.cxx files locations might be different on your machine!
    win32{
        SOURCES += $$V3DBUILDPATH/3drenderer/moc_v3dr_glwidget.cxx
        SOURCES += $$V3DBUILDPATH/v3d/moc_landmark_property_dialog.cxx
        SOURCES += $$V3DBUILDPATH/v3dbase/__/v3d/moc_surfaceobj_geometry_dialog.cxx
        SOURCES += $$V3DBUILDPATH/v3dbase/__/v3d/moc_surfaceobj_annotation_dialog.cxx
    }
    else{
        SOURCES += $$V3DMAINPATH/v3d/moc_v3dr_glwidget.cpp
        SOURCES += $$V3DMAINPATH/v3d/moc_landmark_property_dialog.cpp
        SOURCES += $$V3DMAINPATH/v3d/moc_surfaceobj_geometry_dialog.cpp
        SOURCES += $$V3DMAINPATH/v3d/moc_surfaceobj_annotation_dialog.cpp
    }

    #undefined symbols referenced from Vaa3D objects are ignored and checked dynamically at execution time
    mac{
        LIBS += -undefined dynamic_lookup
    }
}


#set up plugin
DEFINES += _VAA3D_TERAFLY_PLUGIN_MODE
RESOURCES += icons.qrc
HEADERS += ./src/control/V3Dsubclasses.h
HEADERS += ./src/control/CAnnotations.h
HEADERS += ./src/control/CConverter.h
HEADERS += ./src/control/CExplorerWindow.h
HEADERS += ./src/control/CImport.h
HEADERS += ./src/control/CPlugin.h
HEADERS += ./src/control/CSettings.h
HEADERS += ./src/control/CVolume.h
HEADERS += ./src/control/CImageUtils.h
HEADERS += ./src/control/V3Dsubclasses.h
HEADERS += ./src/presentation/PConverter.h
HEADERS += ./src/presentation/PDialogImport.h
HEADERS += ./src/presentation/PDialogProofreading.h
HEADERS += ./src/presentation/PMain.h
HEADERS += ./src/presentation/QArrowButton.h
HEADERS += ./src/presentation/QGradientBar.h
HEADERS += ./src/presentation/QHelpBox.h
HEADERS += ./src/presentation/QGLRefSys.h
HEADERS += ./src/presentation/QPixmapToolTip.h
HEADERS += ./src/presentation/PAbout.h
HEADERS += ./src/presentation/PLog.h
HEADERS += ./src/presentation/PAnoToolBar.h
HEADERS += ./src/core/imagemanager/Block.h
HEADERS += ./src/core/imagemanager/dirent_win.h
HEADERS += ./src/core/imagemanager/IM_config.h
HEADERS += ./src/core/imagemanager/ProgressBar.h
HEADERS += ./src/core/imagemanager/RawFmtMngr.h
HEADERS += ./src/core/imagemanager/RawVolume.h
HEADERS += ./src/core/imagemanager/SimpleVolume.h
HEADERS += ./src/core/imagemanager/SimpleVolumeRaw.h
HEADERS += ./src/core/imagemanager/Stack.h
HEADERS += ./src/core/imagemanager/StackRaw.h
HEADERS += ./src/core/imagemanager/StackedVolume.h
HEADERS += ./src/core/imagemanager/Tiff3DMngr.h
HEADERS += ./src/core/imagemanager/TiledMCVolume.h
HEADERS += ./src/core/imagemanager/TiledVolume.h
HEADERS += ./src/core/imagemanager/TimeSeries.h
HEADERS += ./src/core/imagemanager/VirtualFmtMngr.h
HEADERS += ./src/core/imagemanager/VirtualVolume.h
HEADERS += ./src/core/VolumeConverter/VolumeConverter.h
HEADERS += ./src/core/VolumeConverter/resumer.h
HEADERS += ./src/control/QUndoMarkerCreate.h
HEADERS += ./src/control/QUndoMarkerDelete.h
HEADERS += ./src/control/QUndoMarkerDeleteROI.h

SOURCES += ./src/control/CAnnotations.cpp
SOURCES += ./src/control/CConverter.cpp
SOURCES += ./src/control/CExplorerWindow.cpp
SOURCES += ./src/control/CImport.cpp
SOURCES += ./src/control/CPlugin.cpp
SOURCES += ./src/control/CSettings.cpp
SOURCES += ./src/control/CVolume.cpp
SOURCES += ./src/control/CImageUtils.cpp
SOURCES += ./src/control/V3Dsubclasses.cpp
SOURCES += ./src/presentation/PConverter.cpp
SOURCES += ./src/presentation/PDialogImport.cpp
SOURCES += ./src/presentation/PDialogProofreading.cpp
SOURCES += ./src/presentation/PMain.cpp
SOURCES += ./src/presentation/PAbout.cpp
SOURCES += ./src/presentation/PLog.cpp
SOURCES += ./src/presentation/PAnoToolBar.cpp
SOURCES += ./src/presentation/QPixmapToolTip.cpp
SOURCES += ./src/presentation/QArrowButton.cpp
SOURCES += ./src/presentation/QGradientBar.cpp
SOURCES += ./src/presentation/QHelpBox.cpp
SOURCES += ./src/presentation/QGLRefSys.cpp
SOURCES += ./src/core/imagemanager/Block.cpp
SOURCES += ./src/core/imagemanager/IM_config.cpp
SOURCES += ./src/core/imagemanager/imProgressBar.cpp
SOURCES += ./src/core/imagemanager/RawFmtMngr.cpp
SOURCES += ./src/core/imagemanager/RawVolume.cpp
SOURCES += ./src/core/imagemanager/SimpleVolume.cpp
SOURCES += ./src/core/imagemanager/SimpleVolumeRaw.cpp
SOURCES += ./src/core/imagemanager/Stack.cpp
SOURCES += ./src/core/imagemanager/StackRaw.cpp
SOURCES += ./src/core/imagemanager/StackedVolume.cpp
SOURCES += ./src/core/imagemanager/Tiff3DMngr.cpp
SOURCES += ./src/core/imagemanager/TiledMCVolume.cpp
SOURCES += ./src/core/imagemanager/TiledVolume.cpp
SOURCES += ./src/core/imagemanager/TimeSeries.cpp
SOURCES += ./src/core/imagemanager/VirtualFmtMngr.cpp
SOURCES += ./src/core/imagemanager/VirtualVolume.cpp
SOURCES += ./src/core/VolumeConverter/VolumeConverter.cpp
SOURCES += ./src/core/VolumeConverter/resumer.cpp
SOURCES += ./src/control/QUndoMarkerCreate.cpp
SOURCES += ./src/control/QUndoMarkerDelete.cpp
SOURCES += ./src/control/QUndoMarkerDeleteROI.cpp

#set up your own target
TARGET	= $$qtLibraryTarget(teramanagerplugin)
#DESTDIR = $$V3DBUILDPATH/../v3d/Windows_MSVC_64/plugins/teramanager
DESTDIR	= ../../../bin/plugins/teramanager
