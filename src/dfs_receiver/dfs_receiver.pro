! include( ../../common.pri ) {
    error( "Couldn't find the common.pri file!" )
}

LIB_NAME = dfs_receiver

! include( ../../copy_files.pri ) {
    error( "Couldn't find the copy_files.pri file!" )
}

QT += core
QT += serialport

TEMPLATE = lib

DEBUG_TARGET = dfs_receiverd
RELEASE_TARGET = dfs_receiver

CONFIG(debug, debug|release){
    TARGET = $$DEBUG_TARGET
}

CONFIG(release, debug|release){
    TARGET = $$RELEASE_TARGET
}

CONFIG += staticlib
CONFIG += c++14
DESTDIR =$${LIBS_PATH}/dfs_receiver/lib


INCLUDEPATH += $${LIBS_PATH}/google/include
INCLUDEPATH += $${LIBS_PATH}/winradio/include

INCLUDEPATH += $${LIBS_PATH}/dfs_proto/include
INCLUDEPATH += $${LIBS_PATH}/dfs_base/include

LIBS += -L$${LIBS_PATH}/dfs_proto/lib -ldfs_proto$${LIB_SUFFIX}

HEADERS += \
    wrd_coh_g35_ds.h \
    trmbl_tsip_parser.h \
    trmbl_tsip_reader.h \
    wrd_device_interface.h \
    wrd_coh_callback_g35.h

SOURCES += \
    trmbl_tsip_parser.cpp \
    trmbl_tsip_reader.cpp \
    wrd_coh_callback_g35.cpp \
    wrd_coh_g35_ds.cpp
