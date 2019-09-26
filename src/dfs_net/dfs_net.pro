! include( ../../common.pri ) {
    error( "Couldn't find the common.pri file!" )
}

#PROJECT_ROOT_PATH = $${PWD}/

CONFIG(debug, debug|release) {
    BUILD_FLAG = debug
    LIB_SUFFIX = d
} else {
    BUILD_FLAG = release
}

#LIBS_PATH = $${PROJECT_ROOT_PATH}/lib
QT += core
QT += network
QT -= gui

#documentation.path = $${PWD}/include
#documentation.files = $$files(*.h)
#INSTALLS += documentation

CONFIG += c++14

TARGET = dfs_net
TEMPLATE = lib
CONFIG += staticlib

DESTDIR =$${LIBS_PATH}/dfs_net/lib

INCLUDEPATH += $${LIBS_PATH}/google/include
LIBS +=$${LIBS_PATH}/google/lib

LIBS+= -lprotobuf

INCLUDEPATH += $${LIBS_PATH}/dfs_proto/include
LIBS += $${LIBS_PATH}/dfs_proto/lib

#INCLUDEPATH += $${LIBS_PATH}/dfs_proto/include
#LIBS += $${LIBS_PATH}/dfs_proto/lib


#INCLUDEPATH += "C:\Qt\include\google\protobuf_3_4_1"
#INCLUDEPATH += "C:\Qt\include\google\protobuf_3_7_1"
#LIBS += -L "C:\Qt\lib\google" -lprotobufd
#LIBS += -lprotobuf$${LIB_SUFFIX}
#message("COMMON" $${PROJECT_ROOT_PATH})

#INCLUDEPATH += $${LIBS_PATH}/google/include
#INCLUDEPATH += $${LIBS_PATH}/proto/include
#LIBS += -L$${LIBS_PATH}/google/lib
#LIBS += -L$${LIBS_PATH}/proto/lib

## Добавляем модуль file_copies
#CONFIG += file_copies
## Добавляем переменную, описывающую копируемые файлы
#COPIES += common_incl
## Определяем, с каким расширением нам необохдимо скопировать файлы и по какому пути они расположены
#common_incl.files = $$files($${PWD}/*.h)
## Указываем путь, куда копировать файлы
#common_incl.path = $${LIBS_PATH}/common/include

headers.path = $${LIBS_PATH}/common/include
headers.files   += $$files($${PWD}/*.h)
INSTALLS       += headers

#*************************  COPY INCLUDE *************************
#copytarget.path    = $${LIBS_PATH}/common/include
#copytarget.files  += $$files($${PWD}/*.h)
### wildcard for filename1 filename2 filename3 ...

##message("found files for copytarget: "$$copytarget.files)
#win32: copytarget.files ~= s,/,\\,g

### === copy compiler for makefile ===
#DIR_INCL=include

#DirSep = /
#win32: DirSep = \\

#for(f,copytarget.files) tmp += $${f} ## make absolute paths
#copycompiler.input        = tmp

##message("Input: " $$copycompiler.input)
#isEmpty(DESTDIR):DESTDIR=.
#copycompiler.output       = $$DESTDIR$$DirSep${QMAKE_FILE_BASE}${QMAKE_FILE_EXT}
##copycompiler.output       = $$DESTDIR$$DirSep$$DIR_INCL$$DirSep${QMAKE_FILE_BASE}${QMAKE_FILE_EXT}
#message("copycompiler.output: " $$copycompiler.output)

#copycompiler.commands     = $(COPY_FILE) ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}

#copycompiler.CONFIG       = no_link no_clean
### other CONFIG options are: depends explicit_dependencies target_predeps

#copycompiler.variable_out = QMAKE_DISTCLEAN
#QMAKE_EXTRA_COMPILERS += copycompiler

### == makefile copy target ===
#copyfiles.recurse_target = compiler_copycompiler_make_all
#copyfiles.depends        = $$copyfiles.recurse_target
#copyfiles.CONFIG        += recursive
#QMAKE_EXTRA_TARGETS += copyfiles
#POST_TARGETDEPS     += copyfiles ## copy files after source compilation

#INSTALLS += copytarget


SOURCES += peer_wire_client.cpp \
    channel.cpp \
    channel_client.cpp \
    channel_host.cpp

HEADERS += peer_wire_client.h \
           ring_packet_buffer.h \
    channel.h \
    channel_client.h \
    channel_host.h

DEFINES += QT_DEPRECATED_WARNINGS
