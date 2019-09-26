! include( ../../common.pri ) {
    error( "Couldn't find the common.pri file!" )
}

QT +=   core
QT +=   network
QT +=   serialport


TARGET = ReceiveStationServer

CONFIG += console c++14
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH += $${LIBS_PATH}/winradio/include
INCLUDEPATH += $${LIBS_PATH}/google/include

#INCLUDEPATH += $${LIBS_PATH}/proto/include
#INCLUDEPATH += $${LIBS_PATH}/common/include
#при добавлении нижней строки происходит error
#:-1: ошибка: cannot open output file release/ProcessingSignalClient.exe: Permission denied

#******************
#PRE_TARGETDEPS +=$${LIBS_PATH}/common/lib/libcommonlib.a
#PRE_TARGETDEPS +=$${LIBS_PATH}/proto/lib/libdfs_proto.a


INCLUDEPATH += $${LIBS_PATH}/proto/include
#DEPENDPATH += $${LIBS_PATH}/proto/include
INCLUDEPATH += $${LIBS_PATH}/common/include

LIBS += -L$${LIBS_PATH}/common/lib
LIBS += -L$${LIBS_PATH}/proto/lib

#LIBS += -L$${LIBS_PATH}/server/proto/lib

#LIBS += -L$${LIBS_PATH}/server/common/lib

#LIBS += -L$${LIBS_PATH}/common/lib
#LIBS += -L$${LIBS_PATH}/proto/lib
#******************

##******************
#INCLUDEPATH += $${PROJECT_ROOT_PATH}/src/dfs_proto
#LIBS += -L$${PROJECT_ROOT_PATH}/src/dfs_proto/lib

#INCLUDEPATH += $${PROJECT_ROOT_PATH}/src/common
#LIBS += -L$${PROJECT_ROOT_PATH}/src/common/lib

##LIBS += -L$${LIBS_PATH}/common/lib
##LIBS += -L$${LIBS_PATH}/proto/lib
##******************

LIBS += -L$${LIBS_PATH}/winradio/lib
LIBS += -L$${LIBS_PATH}/google/lib

LIBS += -lcommonlib$${LIB_SUFFIX}
LIBS += -ldfs_proto$${LIB_SUFFIX}
LIBS += -lprotobuf$${LIB_SUFFIX}

#LIBS += -lprotobuf-18


#LIBS += -llibprotobuf -lcommonlib

#QMAKE_CFLAGS += -static
#INCLUDEPATH += "C:\Qt\include\google\protobuf_3_4_1"
#LIBS += -L"C:\Qt\lib\google" -lprotobuf-14
#LIBS += -L"C:\Qt\QtProject\ReceiveStationServer\\lib\\win32\\google" -lprotobuf-14
#LIBS += -lws2_32

#INCLUDEPATH += "C:\Qt\QtProject\common"
#INCLUDEPATH += "C:\Qt\QtProject\common\message"

#LIBS += -L"C:\Qt\QtProject\build-common-Desktop_Qt_5_9_1_MinGW_32bit-Debug\debug" -lcommonlib

HEADERS += receiver/device_set_selector.h \
    tsip/parser.h \
    tsip/time_reader.h \
    network/receiver_station_client.h \
    receiver/coh_g35_device_set.h \
    receiver/signal_file_writer.h \
    network/stream_ddc1.h \
    network/stream_file.h \
    network/stream_server.h

SOURCES += main.cpp \
    receiver/device_set_selector.cpp \
    tsip/parser.cpp \
    tsip/time_reader.cpp \
    network/receiver_station_client.cpp \
    receiver/coh_g35_device_set.cpp \
    receiver/signal_file_writer.cpp \
    network/stream_ddc1.cpp \
    network/stream_file.cpp \
    network/stream_server.cpp

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
