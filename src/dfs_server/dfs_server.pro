! include( ../../common.pri ) {
    error( "Couldn't find the common.pri file!" )
}

QT +=   core serialport
QT +=   network

DEBUG_TARGET = serverd
RELEASE_TARGET = server

CONFIG(debug, debug|release){
    TARGET = $$DEBUG_TARGET
}

CONFIG(release, debug|release){
    TARGET = $$RELEASE_TARGET
}

CONFIG += console c++14
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH += $${LIBS_PATH}/winradio/include
INCLUDEPATH += $${LIBS_PATH}/google/include

INCLUDEPATH += $${LIBS_PATH}/dfs_proto/include

INCLUDEPATH += $${LIBS_PATH}/dfs_base/include
INCLUDEPATH += $${LIBS_PATH}/dfs_receiver/include
INCLUDEPATH += $${LIBS_PATH}/dfs_net/include

LIBS += -L$${LIBS_PATH}/dfs_net/lib -ldfs_net$${LIB_SUFFIX}
LIBS += -L$${LIBS_PATH}/dfs_proto/lib -ldfs_proto$${LIB_SUFFIX}
LIBS += -L$${LIBS_PATH}/dfs_receiver/lib -ldfs_receiver$${LIB_SUFFIX}
LIBS += -L$${LIBS_PATH}/google/lib -lprotobuf$${LIB_SUFFIX}

HEADERS += device_set_selector.h \
    host_server.h \
    host_ds.h

SOURCES += main.cpp \
    host_server.cpp \
    host_ds.cpp

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

