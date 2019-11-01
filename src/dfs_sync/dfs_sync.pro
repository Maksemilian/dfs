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

INCLUDEPATH += $${LIBS_PATH}/dfs_proto/include
INCLUDEPATH += $${LIBS_PATH}/dfs_net/include
INCLUDEPATH += $${LIBS_PATH}/google/include

LIBS += -L$${LIBS_PATH}/dfs_net/lib -ldfs_net$${LIB_SUFFIX}
LIBS += -L$${LIBS_PATH}/dfs_proto/lib  -ldfs_proto$${LIB_SUFFIX}
LIBS += -L$${LIBS_PATH}/google/lib -lprotobuf$${LIB_SUFFIX}

SOURCES += \
        sync_pair_channel.cpp \
    sync_block_alinement.cpp \
    sync_shift_finder.cpp

HEADERS += \
        sync_pair_channel.h \
    sync_block_alinement.h \
    sync_base.h \
    sync_shift_finder.h \
    sync_sum_sub_method.h

