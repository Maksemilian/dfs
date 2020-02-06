! include( ../../common.pri ) {
    error( "Couldn't find the common.pri file!" )
}

LIB_NAME = dfs_base

! include( ../../copy_files.pri ) {
    error( "Couldn't find the copy_files.pri file!" )
}

QT += core
QT -= gui

DEBUG_TARGET = dfs_based
RELEASE_TARGET = dfs_base

CONFIG(debug, debug|release){
    TARGET = $$DEBUG_TARGET
}

CONFIG(release, debug|release){
    TARGET = $$RELEASE_TARGET
}

TEMPLATE = lib

CONFIG += staticlib
CONFIG += c++14

DESTDIR =$${LIBS_PATH}/dfs_base/lib

INCLUDEPATH += $${LIBS_PATH}/google/include
INCLUDEPATH += $${LIBS_PATH}/dfs_proto/include

LIBS += $${LIBS_PATH}/dfs_proto/lib -ldfs_proto
HEADERS += \
    ring_buffer.h \
    device_settings.h \
    i_device_settings.h \
#    sync_global.h
    radio_channel.h \
    custom_thread.h

SOURCES += \
    radio_channel.cpp \
    custom_thread.cpp
