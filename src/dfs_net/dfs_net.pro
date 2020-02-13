! include( ../../common.pri ) {
    error( "Couldn't find the common.pri file!" )
}

LIB_NAME = dfs_net

! include( ../../copy_files.pri ) {
    error( "Couldn't find the copy_files.pri file!" )
}

QT += core
QT += network
QT -= gui

DEBUG_TARGET = dfs_netd
RELEASE_TARGET = dfs_net

CONFIG(debug, debug|release){
    TARGET = $$DEBUG_TARGET
}

CONFIG(release, debug|release){
    TARGET = $$RELEASE_TARGET
}

TEMPLATE = lib

CONFIG += staticlib
CONFIG += c++14

DESTDIR =$${LIBS_PATH}/dfs_net/lib

INCLUDEPATH += $${LIBS_PATH}/google/include
INCLUDEPATH += $${LIBS_PATH}/dfs_proto/include

LIBS += $${LIBS_PATH}/dfs_proto/lib -ldfs_proto
#LIBS += $${LIBS_PATH}/google/lib -lprotobuf

HEADERS += \
    channel.h \
    channel_client.h \
    channel_host.h \
    server.h

SOURCES += \
    channel.cpp \
    channel_client.cpp \
    channel_host.cpp \
    server.cpp

DEFINES += QT_DEPRECATED_WARNINGS
