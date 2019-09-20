! include( ../../common.pri ) {
    error( "Couldn't find the common.pri file!" )
}

TEMPLATE =app

QT+= network core

INCLUDEPATH += $${LIBS_PATH}/common/include
INCLUDEPATH += $${LIBS_PATH}/google/include
INCLUDEPATH += $${LIBS_PATH}/proto/include

LIBS += -L$${LIBS_PATH}/common/lib -lcommonlib$${LIB_SUFFIX}
LIBS += -L$${LIBS_PATH}/proto/lib -ldfs_proto$${LIB_SUFFIX}
LIBS += -L$${LIBS_PATH}/google/lib -lprotobuf$${LIB_SUFFIX}

DEFINES +=QT_MESSAGELOGCONTEXT

greaterThan(QT_MAJOR_VERSION, 4): QT += printsupport
CONFIG +=console
#LIBS += -L$${LIBS_PATH}/google/lib

#LIBS += -ldfs_proto$${LIB_SUFFIX}
#LIBS += -lcommonlib$${LIB_SUFFIX} #Если не поставить перед protobuf то куча ошибок undefine reference
#LIBS += -lprotobuf$${LIB_SUFFIX}

SOURCES += main.cpp \
    echo_server.cpp

HEADERS += \
    echo_server.h
