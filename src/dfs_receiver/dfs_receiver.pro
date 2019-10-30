! include( ../../common.pri ) {
    error( "Couldn't find the common.pri file!" )
}

headers.path = $${LIBS_PATH}/dfs_receiver/include
headers.files   += $$files($${PWD}/*.h)
INSTALLS       += headers

QT += core
QT += serialport

TARGET =dfs_receiver
TEMPLATE = lib

CONFIG += staticlib
CONFIG += c++14
DESTDIR =$${LIBS_PATH}/dfs_receiver/lib

INCLUDEPATH += $${LIBS_PATH}/winradio/include

INCLUDEPATH += $${LIBS_PATH}/google/include

INCLUDEPATH += $${LIBS_PATH}/dfs_proto/include
INCLUDEPATH += $${LIBS_PATH}/dfs_net/include

LIBS += -L$${LIBS_PATH}/winradio/lib

LIBS += -L$${LIBS_PATH}/dfs_proto/lib -ldfs_proto$${LIB_SUFFIX}

LIBS += -L$${LIBS_PATH}/dfs_receiver/lib -ldfs_receiver$${LIB_SUFFIX}
LIBS += -L$${LIBS_PATH}/dfs_net/lib -ldfs_net$${LIB_SUFFIX}
#LIBS += -L$${LIBS_PATH}/winradio/lib
LIBS += -L$${LIBS_PATH}/google/lib -lprotobuf$${LIB_SUFFIX}

HEADERS += \
    wrd_coh_g35_ds.h \
    wrd_ds_selector.h \
    trmbl_tsip_parser.h \
    trmbl_tsip_reader.h

SOURCES += \
    wrd_coh_g35_ds.cpp \
    wrd_ds_selector.cpp \
    trmbl_tsip_parser.cpp \
    trmbl_tsip_reader.cpp
