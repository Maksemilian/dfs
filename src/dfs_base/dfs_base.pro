! include( ../../common.pri ) {
    error( "Couldn't find the common.pri file!" )
}

LIB_NAME = dfs_base

! include( ../../copy_files.pri ) {
    error( "Couldn't find the copy_files.pri file!" )
}


## Добавляем модуль file_copies
#CONFIG += file_copies
## Добавляем переменную, описывающую копируемые файлы
#COPIES += headers
## Определяем, с каким расширением нам необохдимо скопировать файлы и по какому пути они расположены
#headers.files = $$files($${PWD}/*.h)
## Указываем путь, куда копировать файлы
#headers.path = $${LIBS_PATH}/$${LIB_NAME}/include

#message( $${headers.path})
QT += core
QT -= gui

TARGET = dfs_base

TEMPLATE = aux
CONFIG += c++14
#CONFIG += debug_and_release
#CONFIG += staticlib

HEADERS += \
    ring_buffer.h \
    device_settings.h \
    i_device_settings.h
