! include( ../../common.pri ) {
    error( "Couldn't find the common.pri file!" )
}

TARGET = dfs_proto
TEMPLATE = aux

#TEMPLATE = lib
#CONFIG += staticlib
#CONFIG -= debug_and_release debug_and_release_target
#CONFIG +=no_link
PROTOS = $$files(*.proto)
#P_T = $$files(*.cpp)
#P_H = $$files(*.pb.h)
#P_CC = $$files(*.pb.cc)

PROTOBUF_DIR = C:/third_party/protobuf
GPP_DIR =C:/msys64/mingw32
PROTO_INCL=$${LIBS_PATH}/proto/include

INCLUDEPATH += $${PROTOBUF_DIR}/include
LIBS += -L$${PROTOBUF_DIR}/lib -llibprotobuf

PROTOC = $${PROTOBUF_DIR}/bin/protoc.exe
GPP =$${GPP_DIR}/bin/g++.exe

OBJECTS_DIR = obj
#PROTO_INCLUDE = include
#PROTO_SOURCE = source

protobuf_decl.name = protobuf headers
protobuf_decl.input = PROTOS
protobuf_decl.output = ${QMAKE_FILE_BASE}.pb.h
#protobuf_impl.depends  = $${PROTOBUF_DIR}/include
protobuf_decl.commands = $${PROTOC} --cpp_out=${QMAKE_FILE_IN_PATH} --proto_path=${QMAKE_FILE_IN_PATH} ${QMAKE_FILE_NAME}
#protobuf_decl.commands = $${PROTOC} --cpp_out=$${LIBS_PATH}/proto/include --proto_path=${QMAKE_FILE_IN_PATH} ${QMAKE_FILE_NAME}
protobuf_decl.variable_out = HEADERS
#если убрать эту строку копируется только файл command.pb.h
protobuf_decl.CONFIG = target_predeps
QMAKE_EXTRA_COMPILERS += protobuf_decl

protobuf_impl.name = protobuf sources
protobuf_impl.input = PROTOS
protobuf_impl.output = ${QMAKE_FILE_BASE}.pb.cc
protobuf_impl.depends  = ${QMAKE_FILE_BASE}.pb.h
protobuf_impl.commands = $$escape_expand(\\n)
#protobuf_decl.commands = $${PROTOC} --cpp_out=${QMAKE_FILE_IN_PATH} --proto_path=${QMAKE_FILE_IN_PATH} ${QMAKE_FILE_NAME}
protobuf_impl.variable_out = G_SOURCES
protobuf_impl.CONFIG = target_predeps
QMAKE_EXTRA_COMPILERS += protobuf_impl

protobuf_obj.name= proto obj
protobuf_obj.input = G_SOURCES
protobuf_obj.output = $${OBJECTS_DIR}/${QMAKE_FILE_BASE}.o
protobuf_obj.commands = g++ -c ${QMAKE_FILE_IN} -o ${QMAKE_FILE_OUT}
protobuf_obj.variable_out = OBJECTS
protobuf_obj.CONFIG = target_predeps
QMAKE_EXTRA_COMPILERS +=protobuf_obj

protobuf_lib.name=proto lib
protobuf_lib.input = OBJECTS
protobuf_lib.output = $${LIBS_PATH}/proto/lib/libdfs_proto.a
protobuf_lib.commands = ar -cr ${QMAKE_FILE_OUT} ${QMAKE_FILE_IN}
#без этой строки
protobuf_lib.CONFIG += combine target_predeps
#ошибка если не использоать combine , target_predeps не нужен
#ar -cr libprotomessages.a libprotomessages.a
#ar: невозможно переименовать <libprotomessages.a>; причина: File exists
#mingw32-make: *** [Makefile.Release:119: libprotomessages.a] Error 1

QMAKE_EXTRA_COMPILERS +=protobuf_lib

#****************** copy 2 ВО ВРЕМЯ ЗАПУСКА APP КОМИТИТЬ
#headers.path = $${LIBS_PATH}/proto/include
#headers.files   += $$files($${PWD}/*.pb.h)
#INSTALLS       += headers

HEADERS += \
    command.pb.h \
    device_set_info.pb.h \
    packet.pb.h
#    TEST.pb.h

SOURCES += \
    command.pb.cc \
    device_set_info.pb.cc \
    packet.pb.cc
#    TEST.pb.cc

#****************** copy 1

## Добавляем модуль file_copies
#CONFIG += file_copies
## Добавляем переменную, описывающую копируемые файлы
#COPIES += protobuf_incl
## Определяем, с каким расширением нам необохдимо скопировать файлы и по какому пути они расположены
#protobuf_incl.files = $$files($${PWD}/*.pb.h)
## Указываем путь, куда копировать файлы
#protobuf_incl.path = $${LIBS_PATH}/proto/include



##*********************4 QMAKE_EXTRA_COMPILERS ПРЕПЯТСВУЕТ СОЗДАНИЮ PROTO ФАЙЛОВ
#copytarget.path    = $${LIBS_PATH}/proto/include
#copytarget.files  += $$files($${PWD}/*.pb.h)
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

##copycompiler.CONFIG       = no_link no_clean
### other CONFIG options are: depends explicit_dependencies target_predeps

#copycompiler.variable_out = QMAKE_DISTCLEAN
#QMAKE_EXTRA_COMPILERS += copycompiler

## == makefile copy target ===
#copyfiles.recurse_target = compiler_copycompiler_make_all
#copyfiles.depends        = $$copyfiles.recurse_target
#copyfiles.CONFIG        += recursive
#QMAKE_EXTRA_TARGETS += copyfiles
#POST_TARGETDEPS     += copyfiles ## copy files after source compilation

#INSTALLS += copytarget

#****************************XCODE
#protobuf_incl_cp.name=proto cp
#protobuf_incl_cp.input = HEADERS
##protobuf_incl_cp.output = $${PWD}/include/${QMAKE_FILE_IN}.pb.h
#protobuf_incl_cp.output =C:\Qt\QtProject\dfs\src\dfs_proto\include\${QMAKE_FILE_IN}
##protobuf_impl.depends  = ${QMAKE_FILE_BASE}.pb.h
#protobuf_incl_cp.commands = XCOPY C:\Qt\QtProject\dfs\src\dfs_proto\${QMAKE_FILE_IN} C:\Qt\QtProject\dfs\src\dfs_proto\include
##protobuf_incl_cp.commands = cp ${QMAKE_FILE_IN} $${PWD}/include/
##без этой строки
##protobuf_incl_cp.CONFIG += combine target_predeps
#QMAKE_EXTRA_COMPILERS +=protobuf_incl_cp

#$${LIBS_PATH}/proto/include
#headers.files   += $$files($${PWD}/*.pb.h)
#*******************CP
#protobuf_incl_cp.name= proto cp
#protobuf_incl_cp.input = HEADERS
##protobuf_incl_cp.output = $${PWD}/include/${QMAKE_FILE_IN}.pb.h
#protobuf_incl_cp.output = $${PWD}/include/${QMAKE_FILE_BASE}.${QMAKE_FILE_EXT}
##protobuf_impl.depends  = ${QMAKE_FILE_BASE}.pb.h
#protobuf_incl_cp.commands = $(COPY_FILE) ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
##protobuf_incl_cp.commands = cp ${QMAKE_FILE_IN} $${PWD}/include/
##без этой строки
#protobuf_incl_cp.CONFIG += no_link no_clean
#QMAKE_EXTRA_COMPILERS +=protobuf_incl_cp

#DISTFILES += \
#    command.proto \
#    device_set_info.proto \
#    packet.proto

##message(OBJECTS);
#g_lib.input = P_T
#g_lib.command = g++ -c main.cpp -o main.o
#g_lib.command = g++ -c command.pb.cc -o command.pb.o
#g_lib.output = C:/Qt/QtProject/dfs_proto/command.pb.o

#*****************************
#TARGET = dfs_proto
#TEMPLATE = lib
##TEMPLATE = aux
#CONFIG += staticlib

#PROTOS =  $$files(*.proto)
##T = $$files(*.cpp)

#PROTOBUF_DIR = C:/third_party/protobuf
#GPP_DIR =C:/msys64/mingw32

#INCLUDEPATH += $${PROTOBUF_DIR}/include
#LIBS += -L$${PROTOBUF_DIR}/lib -llibprotobuf
#PROTOC = $${PROTOBUF_DIR}/bin/protoc.exe
#GPP =$${GPP_DIR}/bin/g++.exe

#protobuf_decl.name = protobuf headers
#protobuf_decl.input = PROTOS
#protobuf_decl.output = ${QMAKE_FILE_BASE}.pb.h
#protobuf_decl.commands = $${PROTOC} --cpp_out=${QMAKE_FILE_IN_PATH} --proto_path=${QMAKE_FILE_IN_PATH} ${QMAKE_FILE_NAME}
#protobuf_decl.variable_out = HEADERS
#protobuf_decl.CONFIG = target_predeps
#QMAKE_EXTRA_COMPILERS += protobuf_decl

#protobuf_impl.name = protobuf sources
#protobuf_impl.input = PROTOS
#protobuf_impl.output = ${QMAKE_FILE_BASE}.pb.cc
#protobuf_impl.depends  = ${QMAKE_FILE_BASE}.pb.h
#protobuf_impl.commands = $$escape_expand(\\n)
#protobuf_impl.variable_out = PRE_TARGETDEPS
#protobuf_impl.CONFIG = target_predeps
#QMAKE_EXTRA_COMPILERS += protobuf_impl

##libt.name= proto obj
##libt.input = T
##libt.command = $${GPP} -c T - o main.o
##libt.output = C:/Qt/QtProject/dfs/src/dfs_proto/__MAIN.o
##libt.CONFIG = target_predeps

###libt.CONFIG += combine
###libt.CONFIG += no link
###lib.variable_out = PRE_TARGETDEPS
##QMAKE_EXTRA_COMPILERS +=libt

##PROTOS_OBJ += command.pb.o device_set_info.pb.o packet.pb.o

##libt.CONFIG += combine
###libt.CONFIG += no link
##libt.name= proto lib
##libt.input = PROTOS_OBJ
##libt.command = ar -qsc PROTOS_OBJ
###libt.output = dfs_proto.a
###libt.output = C:/Qt/QtProject/dfs/build/dfs_proto/release/dfs_7proto.a
###C:/Qt/QtProject/dfs/src/dfs_proto
###lib.variable_out = C:/Qt/QtProject/dfs/build/dfs_proto/release/libdfs_proto.a
###lib.variable_out = C:/Qt/QtProject/dfs/src/dfs_proto/libdfs_proto_777.a
##libt.CONFIG = target_predeps
###lib.variable_out = PRE_TARGETDEPS
##QMAKE_EXTRA_COMPILERS +=libt


