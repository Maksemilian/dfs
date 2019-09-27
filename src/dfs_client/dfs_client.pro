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
    interface/i_device_set_settings.h \
    interface/i_stream_reader.h \
    interface/i_sygnal_update.h \
    interface/i_toolbar.h \
    core/network/signal_sync.h \
    core/command/factory_command.h \
    core/command/command_hierarch.h \
    core/db/data_base.h \
    core/sync_pair_channel.h \
    core/sync_manager.h \
    ui/tool/frequency_line_edit.h \
    ui/tool/switch_button.h \
    ui/tool/preselector_widget.h \
    ui/tool/widgets.h \
    ui/db/widget_director.h \
    ui/db/list_station_panel_widget.h \
    ui/db/receiver_station_panel.h \
    ui/db/sync_task_view.h \
    ui/plot/monitoring_signal.h \
    ui/plot/channel_plot.h \
    ui/plot/elips_plot.h \
    ui/main_window.h \
    ui/main_window_settings.h \
    core/network/file_loader.h \
    core/network/stream_clieint.h \
    core/network/device_set_client.h

SOURCES += \
    core/command/factory_command.cpp \
    core/command/command_hierarch.cpp \
    core/sync_pair_channel.cpp \
    core/db/data_base.cpp \
    core/sync_manager.cpp \
    ui/db/sync_task_view.cpp \
    ui/db/list_station_panel_widget.cpp \
    ui/db/receiver_station_panel.cpp \
    ui/db/widget_director.cpp \
    ui/tool/frequency_line_edit.cpp \
    ui/tool/switch_button.cpp \
    ui/tool/preselector_widget.cpp \
    ui/tool/widgets.cpp \
    ui/plot/channel_plot.cpp \
    ui/plot/elips_plot.cpp \
    ui/main_window.cpp \
    interface/interface_toolbar.cpp \
    ui/main_window_settings.cpp \
    main.cpp \
    core/network/file_loader.cpp \
    core/network/stream_clieint.cpp \
    core/network/device_set_client.cpp

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
