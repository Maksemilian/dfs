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
    i_device_set_settings.h \
    ring_buffer.h \
    coh_g35_ds_settings.h
