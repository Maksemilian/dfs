! include( ../../common.pri ) {
    error( "Couldn't find the common.pri file!" )
}

TEMPLATE =app

QT+= network core

INCLUDEPATH += $${LIBS_PATH}/dfs_net/include
INCLUDEPATH += $${LIBS_PATH}/dfs_proto/include
INCLUDEPATH += $${LIBS_PATH}/google/include

LIBS += -L$${LIBS_PATH}/dfs_net/lib -ldfs_net$${LIB_SUFFIX}
LIBS += -L$${LIBS_PATH}/dfs_proto/lib -ldfs_proto$${LIB_SUFFIX}
LIBS += -L$${LIBS_PATH}/google/lib -lprotobuf$${LIB_SUFFIX}

DEFINES +=QT_MESSAGELOGCONTEXT

greaterThan(QT_MAJOR_VERSION, 4): QT += printsupport
CONFIG +=console

SOURCES += main.cpp \
    echo_server.cpp

HEADERS += \
    echo_server.h
