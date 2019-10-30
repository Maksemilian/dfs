! include( ../../common.pri ) {
    error( "Couldn't find the common.pri file!" )
}

QT       += core gui
QT       += network
QT       += xml
QT       += sql
QT       += core

DEFINES +=QT_MESSAGELOGCONTEXT

greaterThan(QT_MAJOR_VERSION, 4): QT += printsupport


TEMPLATE = app
TARGET = Client
CONFIG +=console
CONFIG += c++14
INCLUDEPATH += $${LIBS_PATH}/dfs_proto/include
INCLUDEPATH += $${LIBS_PATH}/dfs_net/include

INCLUDEPATH += $${LIBS_PATH}/ipp/include
INCLUDEPATH += $${LIBS_PATH}/plot/include
INCLUDEPATH += $${LIBS_PATH}/google/include

LIBS += -L$${LIBS_PATH}/dfs_net/lib -ldfs_net$${LIB_SUFFIX}
LIBS += -L$${LIBS_PATH}/dfs_proto/lib  -ldfs_proto$${LIB_SUFFIX}

LIBS += -L$${LIBS_PATH}/ipp/lib -lippi -lipps -lippcore -lippvm
LIBS += -L$${LIBS_PATH}/plot/lib -lqcustomplot$${LIB_SUFFIX}2
LIBS += -L$${LIBS_PATH}/google/lib -lprotobuf$${LIB_SUFFIX}

#Если -lcommonlib$${LIB_SUFFIX} не поставить перед -ldfs_net$${LIB_SUFFIX}
#or  -ldfs_proto$${LIB_SUFFIX} то куча ошибок undefine reference

HEADERS += \
    ui/main_window.h \
    ui/plot_elipse.h \
    ui/plot_channel.h \
    ui/tool_frequency_line_edit.h \
    ui/tool_widgets.h \
    ui/tool_switch_button.h \
    ui/tool_preselector_widget.h \
    interface/i_deviceset.h \
    i_device_set_settings.h \
    core/network/signal_sync.h \
    command_hierarch.h \
    sync_pair_channel.h \
    command_factory.h \
    client_ds.h \
    client_ds_stream.h \
    client_ds_stream_controller.h \
    ui/client_ds_ui.h \
    ui/client_ds_ui_list.h \
    ui/plot_monitoring.h

SOURCES += \
    ui/main_window.cpp \
    ui/plot_elipse.cpp \
    ui/plot_channel.cpp \
    ui/tool_widgets.cpp \
    ui/tool_switch_button.cpp \
    ui/tool_preselector_widget.cpp \
    ui/tool_frequency_line_edit.cpp \
    command_hierarch.cpp \
    command_factory.cpp \
    sync_pair_channel.cpp \
    main.cpp \
    client_ds.cpp \
    client_ds_stream.cpp \
    client_ds_stream_controller.cpp \
    ui/client_ds_ui.cpp \
    ui/client_ds_ui_list.cpp \
    ui/plot_monitoring.cpp

FORMS += \
    ui/main_window.ui \

#при добавлении нижней строки происходит error
#:-1: ошибка: cannot open output file release/ProcessingSignalClient.exe: Permission denied

#INCLUDEPATH += $${PROJECT_ROOT_PATH}/src/dfs_proto
#LIBS += -L$${LIBS_PATH}/

#LIBS += -L$${LIBS_PATH}/client/common/lib
#LIBS += -L$${LIBS_PATH}/client/proto/lib
#PRE_TARGETDEPS +=$${LIBS_PATH}/common/lib/libcommonlib.a
#PRE_TARGETDEPS +=$${LIBS_PATH}/proto/lib/libdfs_proto.a

## Добавляем модуль file_copies
#CONFIG += file_copies
## Добавляем переменную, описывающую копируемые файлы
#COPIES += protobuf_incl
## Определяем, с каким расширением нам необохдимо скопировать файлы и по какому пути они расположены
#protobuf_incl.files = $$files($${PWD}/lib/proto/lib/include/*.pb.h)
## Указываем путь, куда копировать файлы
#protobuf_incl.path = $${LIBS_PATH}/client/proto/include

#headers.path    = $${LIBS_PATH}/client/proto/include
#headers.files   += $$files($${LIBS_PATH}/proto/include/*.pb.h)
#INSTALLS       += headers
