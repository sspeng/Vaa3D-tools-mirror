
TEMPLATE	= lib
CONFIG	+= qt plugin warn_off
#CONFIG	+= x86_64
VAA3DPATH = ../../../v3d_main
INCLUDEPATH	+= $$VAA3DPATH/basic_c_fun
INCLUDEPATH     += $$VAA3DPATH/common_lib/include
#LIBS += -L. -lv3dtiff -L$$VAA3DPATH/common_lib/lib


HEADERS	+= histogram_plugin.h
HEADERS	+= histogram_func.h
HEADERS	+= histogram_gui.h
#HEADERS	+= $$VAA3DPATH/basic_c_fun/stackutil.h
#HEADERS	+= $$VAA3DPATH/basic_c_fun/imageio_mylib.h

SOURCES	= histogram_plugin.cpp
SOURCES	+= histogram_func.cpp
SOURCES	+= histogram_gui.cpp

#SOURCES	+= $$VAA3DPATH/basic_c_fun/stackutil.cpp
#SOURCES += $$VAA3DPATH/basic_c_fun/basic_4dimage.cpp
SOURCES	+= $$VAA3DPATH/basic_c_fun/v3d_message.cpp
#SOURCES	+= $$VAA3DPATH/basic_c_fun/mg_utilities.cpp
#SOURCES	+= $$VAA3DPATH/basic_c_fun/mg_image_lib.cpp
#SOURCES	+= $$VAA3DPATH/basic_c_fun/imageio_mylib.cpp


TARGET	= $$qtLibraryTarget(histogram)
#DESTDIR	= ../../v3d/plugins/histogram/
DESTDIR	= $$VAA3DPATH/../bin/plugins/histogram/
