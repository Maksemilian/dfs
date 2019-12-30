! include( ../../common.pri ) {
    error( "Couldn't find the common.pri file!" )
}


##****************** copy 2 ВО ВРЕМЯ ЗАПУСКА APP КОМИТИТЬ

headers.path = $${LIBS_PATH}/dfs_base/include
headers.files   += $$files($${PWD}/*.h)

INSTALLS       += headers

QT += core
QT -= gui

TARGET = dfs_base
TEMPLATE = lib
CONFIG += c++14
CONFIG += staticlib

HEADERS += \
    ring_buffer.h \
    device_settings.h \
    i_device_settings.h
