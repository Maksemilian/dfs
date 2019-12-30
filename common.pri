! include( format.astylerc){
    error( "Couldn't find the format.astylerc file!" )
}

PROJECT_ROOT_PATH = $${PWD}/


CONFIG(debug, debug|release) {
    BUILD_FLAG = debug
    LIB_SUFFIX = d
} else {
    BUILD_FLAG = release
}

LIBS_PATH = $${PROJECT_ROOT_PATH}/lib
#message(COMMON_T: $${PROJECT_ROOT_PATH}/lib);
BIN_PATH = $${PROJECT_ROOT_PATH}/bin
#INC_PATH = $${PROJECT_ROOT_PATH}/include/
#INCLUDEPATH += $${LIBS_PATH}/proto/include

#INCLUDEPATH += $${LIBS_PATH}/common/include

#*******************

#INCLUDEPATH += $${LIBS_PATH}/proto/include
#INCLUDEPATH += $${LIBS_PATH}/common/include
##при добавлении нижней строки происходит error
##:-1: ошибка: cannot open output file release/ProcessingSignalClient.exe: Permission denied

##INCLUDEPATH += $${PROJECT_ROOT_PATH}/src/dfs_proto
##LIBS += -L$${LIBS_PATH}/

#LIBS += -L$${LIBS_PATH}/common/lib
#LIBS += -L$${LIBS_PATH}/proto/lib

#*******************

#LIBS += -L$${PROJECT_ROOT_PATH}/src/common/lib
#LIBS += -L$${PROJECT_ROOT_PATH}/src/dfs_proto/lib

#INCLUDEPATH += $${LIBS_PATH}/google/include
#LIBS += -L$${LIBS_PATH}/google/lib
#LIBS += -L$${LIBS_PATH}/proto/lib

#INCLUDEPATH += $${LIBS_PATH}/plot/include
#LIBS += -L$${LIBS_PATH}/plot/lib
#INCLUDEPATH += $${LIBS_PATH}/common/include/message
#INCLUDEPATH += $${LIBS_PATH}/ipp/include
#LIBS += -L$${LIBS_PATH}/ipp/lib
#INCLUDEPATH += $${LIBS_PATH}/winradio/include
#LIBS += -L$${LIBS_PATH}/winradio/lib
