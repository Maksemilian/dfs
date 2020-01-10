! include( ../../common.pri ) {
    error( "Couldn't find the common.pri file!" )
}

LIB_NAME = dfs_base

! include( ../../copy_files.pri ) {
    error( "Couldn't find the copy_files.pri file!" )
}

QT += core
QT -= gui

TARGET = dfs_base

TEMPLATE = aux
CONFIG += c++14

HEADERS += \
    ring_buffer.h \
    device_settings.h \
    i_device_settings.h
