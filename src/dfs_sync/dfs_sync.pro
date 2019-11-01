#-------------------------------------------------
#
# Project created by QtCreator 2019-10-31T10:42:38
#
#-------------------------------------------------
! include( ../../common.pri ) {
    error( "Couldn't find the common.pri file!" )
}

headers.path = $${LIBS_PATH}/dfs_sync/include
headers.files   += $$files($${PWD}/*.h)
INSTALLS       += headers

QT       +=core

TARGET = dfs_sync
TEMPLATE = lib

CONFIG += staticlib
CONFIG += c++14

DESTDIR =$${LIBS_PATH}/dfs_sync/lib

INCLUDEPATH += $${LIBS_PATH}/ipp/include
LIBS += -L$${LIBS_PATH}/ipp/lib -lippi -lipps -lippcore -lippvm

INCLUDEPATH += $${LIBS_PATH}/dfs_base/include

INCLUDEPATH += $${LIBS_PATH}/dfs_proto/include
INCLUDEPATH += $${LIBS_PATH}/google/include

LIBS += -L$${LIBS_PATH}/dfs_proto/lib  -ldfs_proto$${LIB_SUFFIX}
LIBS += -L$${LIBS_PATH}/google/lib -lprotobuf$${LIB_SUFFIX}

SOURCES += \
    sync_process.cpp \
    sync_controller.cpp \
    sync_block_equalizer.cpp

HEADERS += \
    sync_process.h \
    sync_controller.h \
    sync_block_equalizer.h

