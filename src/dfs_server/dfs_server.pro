! include( ../../common.pri ) {
    error( "Couldn't find the common.pri file!" )
}

QT +=   core
QT +=   network
QT +=   serialport

TARGET = Server

CONFIG += console c++14
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH += $${LIBS_PATH}/winradio/include
INCLUDEPATH += $${LIBS_PATH}/google/include

INCLUDEPATH += $${LIBS_PATH}/dfs_receiver/include
INCLUDEPATH += $${LIBS_PATH}/dfs_proto/include
INCLUDEPATH += $${LIBS_PATH}/dfs_net/include

LIBS += -L$${LIBS_PATH}/dfs_receiver/lib -ldfs_receiver$${LIB_SUFFIX}
LIBS += -L$${LIBS_PATH}/dfs_net/lib -ldfs_net$${LIB_SUFFIX}
LIBS += -L$${LIBS_PATH}/dfs_proto/lib -ldfs_proto$${LIB_SUFFIX}

LIBS += -L$${LIBS_PATH}/winradio/lib
LIBS += -L$${LIBS_PATH}/google/lib -lprotobuf$${LIB_SUFFIX}

#LIBS += -ldfs_net$${LIB_SUFFIX}
#LIBS += -ldfs_proto$${LIB_SUFFIX}
#LIBS += -lprotobuf$${LIB_SUFFIX}

HEADERS += device_set_selector.h \
    host_server.h \
    host_ds.h \
    host_ds_stream.h

SOURCES += main.cpp \
    host_server.cpp \
    host_ds.cpp \
    host_ds_stream.cpp

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#INCLUDEPATH += $${LIBS_PATH}/proto/include
#INCLUDEPATH += $${LIBS_PATH}/common/include
#при добавлении нижней строки происходит error
#:-1: ошибка: cannot open output file release/ProcessingSignalClient.exe: Permission denied

#******************
#PRE_TARGETDEPS +=$${LIBS_PATH}/common/lib/libcommonlib.a
#PRE_TARGETDEPS +=$${LIBS_PATH}/proto/lib/libdfs_proto.a
