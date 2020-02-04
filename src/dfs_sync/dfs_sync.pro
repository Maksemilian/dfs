#-------------------------------------------------
#
# Project created by QtCreator 2019-10-31T10:42:38
#
#-------------------------------------------------
! include( ../../common.pri ) {
    error( "Couldn't find the common.pri file!" )
}

LIB_NAME = dfs_sync

! include( ../../copy_files.pri ) {
    error( "Couldn't find the copy_files.pri file!" )
}

QT       +=core

DEBUG_TARGET = dfs_syncd
RELEASE_TARGET = dfs_sync

CONFIG(debug, debug|release){
    TARGET = $$DEBUG_TARGET
}

CONFIG(release, debug|release){
    TARGET = $$RELEASE_TARGET
#    CONFIG += console
#    DEFINES += QT_NO_DEBUG_OUTPUT
}
TEMPLATE = lib

CONFIG += staticlib
CONFIG += c++14

DESTDIR =$${LIBS_PATH}/dfs_sync/lib

INCLUDEPATH += $${LIBS_PATH}/dfs_base/include
INCLUDEPATH += $${LIBS_PATH}/dfs_proto/include

INCLUDEPATH += $${LIBS_PATH}/ipp/include
INCLUDEPATH += $${LIBS_PATH}/google/include

LIBS += -L$${LIBS_PATH}/dfs_proto/lib  -ldfs_proto$${LIB_SUFFIX}
LIBS += -L$${LIBS_PATH}/ipp/lib -lippi -lipps -lippcore -lippvm
#LIBS += -L$${LIBS_PATH}/google/lib -lprotobuf$${LIB_SUFFIX}

SOURCES += \
    sync_controller.cpp \
    sync_block_equalizer.cpp \
    sync_2d.cpp \
    sync_radio_channel.cpp

HEADERS += \
    sync_controller.h \
    sync_block_equalizer.h \
    sync_2d.h \
    sync_test.h \
    sync_global.h \
    sync_calc_delta_pps.h \
    sync_radio_channel.h

