! include( ../../common.pri ) {
    error( "Couldn't find the common.pri file!" )
}

QT       += core gui network

DEFINES += QT_MESSAGELOGCONTEXT

greaterThan(QT_MAJOR_VERSION, 4): QT += printsupport


TARGET = Client
TEMPLATE = app

DEBUG_TARGET = clientd
RELEASE_TARGET = client

CONFIG(debug, debug|release){
    TARGET = $$DEBUG_TARGET
}

CONFIG(release, debug|release){
    TARGET = $$RELEASE_TARGET
#    CONFIG += console
#    DEFINES += QT_NO_DEBUG_OUTPUT
}

CONFIG +=console
CONFIG += c++14

INCLUDEPATH += $${LIBS_PATH}/dfs_proto/include
INCLUDEPATH += $${LIBS_PATH}/dfs_net/include
INCLUDEPATH += $${LIBS_PATH}/dfs_sync/include
INCLUDEPATH += $${LIBS_PATH}/dfs_base/include

INCLUDEPATH += $${LIBS_PATH}/ipp/include
INCLUDEPATH += $${LIBS_PATH}/plot/include
INCLUDEPATH += $${LIBS_PATH}/google/include

LIBS += -L$${LIBS_PATH}/dfs_net/lib -ldfs_net$${LIB_SUFFIX}
LIBS += -L$${LIBS_PATH}/dfs_proto/lib  -ldfs_proto$${LIB_SUFFIX}
LIBS += -L$${LIBS_PATH}/dfs_sync/lib  -ldfs_sync$${LIB_SUFFIX}
LIBS += -L$${LIBS_PATH}/dfs_base/lib  -ldfs_base$${LIB_SUFFIX}

LIBS += -L$${LIBS_PATH}/ipp/lib -lippi -lipps -lippcore -lippvm
LIBS += -L$${LIBS_PATH}/plot/lib -lqcustomplot$${LIB_SUFFIX}2
LIBS += -L$${LIBS_PATH}/google/lib -lprotobuf$${LIB_SUFFIX}

#Если -lcommonlib$${LIB_SUFFIX} не поставить перед -ldfs_net$${LIB_SUFFIX}
#or  -ldfs_proto$${LIB_SUFFIX} то куча ошибок undefine reference
RESOURCES += res.qrc

HEADERS += \
    ui/main_window.h \
    ui/tool_frequency_line_edit.h \
    ui/tool_widgets.h \
    ui/tool_switch_button.h \
    ui/tool_preselector_widget.h \
    ui/client_ds_ui.h \
    ui/client_ds_ui_list.h \
    ui/command_hierarch.h \
    ui/command_factory.h \
    ui/i_device_settings.h \
    client_ds.h \
    client.h \
    client_manager.h \
    ui/plot.h \
    stream_server.h

SOURCES += \
    ui/main_window.cpp \
    ui/tool_widgets.cpp \
    ui/tool_switch_button.cpp \
    ui/tool_preselector_widget.cpp \
    ui/tool_frequency_line_edit.cpp \
    ui/client_ds_ui.cpp \
    ui/client_ds_ui_list.cpp \
    ui/command_hierarch.cpp \
    ui/command_factory.cpp \
    client_ds.cpp \
    client.cpp \
    client_manager.cpp \
    main.cpp \
    ui/plot.cpp \
    stream_server.cpp

FORMS += \
    ui/main_window.ui \
