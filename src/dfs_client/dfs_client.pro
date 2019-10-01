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
    i_device_set_settings.h \
    i_stream_reader.h \
    i_sygnal_update.h \
    i_toolbar.h \
    core/network/signal_sync.h \
    command_hierarch.h \
    sync_pair_channel.h \
    ui/db/widget_director.h \
    ui/db/list_station_panel_widget.h \
    ui/db/receiver_station_panel.h \
    ui/db/sync_task_view.h \
    ui/main_window.h \
    interface/i_deviceset.h \
    ui/plot_elipse.h \
    ui/plot_channel.h \
    ui/tool_frequency_line_edit.h \
    ui/tool_widgets.h \
    ui/tool_switch_button.h \
    ui/tool_preselector_widget.h \
    command_factory.h \
    db.h \
    client_device_set.h \
    client_file_loader.h \
    client_stream.h \
    ui/client_device_set_widget.h \
    ui/client_device_set_widget_list.h

SOURCES += \
    command_hierarch.cpp \
    command_factory.cpp \
    sync_pair_channel.cpp \
    ui/db/sync_task_view.cpp \
    ui/db/list_station_panel_widget.cpp \
    ui/db/receiver_station_panel.cpp \
    ui/db/widget_director.cpp \
    ui/main_window.cpp \
    ui/plot_elipse.cpp \
    ui/plot_channel.cpp \
    ui/tool_widgets.cpp \
    ui/tool_switch_button.cpp \
    ui/tool_preselector_widget.cpp \
    ui/tool_frequency_line_edit.cpp \
    db.cpp \
    main.cpp \
    ui/client_device_set_widget.cpp \
    ui/client_device_set_widget_list.cpp \
    client_device_set.cpp \
    client_file_loader.cpp \
    client_stream.cpp \
    i_toolbar.cpp

FORMS += \
    ui/main_window.ui \
    ui/db/sync_task_view.ui \
    ui/db/receiver_station_panel.ui \

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
