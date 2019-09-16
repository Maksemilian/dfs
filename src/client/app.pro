! include( ../../../common.pri ) {
    error( "Couldn't find the common.pri file!" )
}

QT       += core gui
QT       += network
QT       += xml
QT       += sql
QT       += core

DEFINES +=QT_MESSAGELOGCONTEXT

greaterThan(QT_MAJOR_VERSION, 4): QT += printsupport

CONFIG +=console
TARGET = ProcessingSignalClient
TEMPLATE = app

LIBS += -lqcustomplot$${LIB_SUFFIX}2
LIBS += -lcommonlib$${LIB_SUFFIX} #Если не поставить перед protobuf то куча ошибок undefine reference
LIBS += -lprotobuf$${LIB_SUFFIX}
LIBS += -lippi -lipps -lippcore -lippvm

#LIBS += -lprotobuf-18
#LIBS += -lMyLib$${LIB_SUFFIX}
#win32 {
#    CONFIG(debug, debug|release): DESTDIR = ../bin/debug
#    CONFIG(release, debug|release): DESTDIR = ../bin/release
#}


HEADERS += \
    interface/i_device_set_settings.h \
    interface/i_stream_reader.h \
    interface/i_sygnal_update.h \
    interface/i_toolbar.h \
    core/network/receiver_station_client.h \
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
    core/network/file_loader.h

SOURCES += \
    core/network/receiver_station_client.cpp \
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
    core/network/file_loader.cpp

FORMS += \
    ui/main_window.ui \
    ui/db/sync_task_view.ui \
    ui/db/receiver_station_panel.ui \


# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
