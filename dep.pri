SONAME = a
for(dep, DEPENDENCY_LIBRARIES) {
    #message($$TARGET depends on $$dep ($${DESTDIR}/$${dep}.$${SONAME}))
#    LIBS += $${DESTDIR}/lib$${dep}.$${SONAME}
    PRE_TARGETDEPS += $${DESTDIR}/lib$${dep}.$${SONAME}
}
#PRE_TARGETDEPS += \
#$${LIBS_PATH}/dfs_net/lib/$${LIBPREFIX}dfs_net$${LIB_SUFFIX}.a \
#$${LIBS_PATH}/dfs_proto/lib/$${LIBPREFIX}dfs_proto$${LIB_SUFFIX}.a \
#$${LIBS_PATH}/dfs_receiver/lib/$${LIBPREFIX}dfs_receiver$${LIB_SUFFIX}.a

#DEPENDENCY_LIBRARIES = dfs_net dfs_proto dfs_receiver

#SONAME = a
#for(dep, DEPENDENCY_LIBRARIES) {
#    #message($$TARGET depends on $$dep ($${DESTDIR}/$${dep}.$${SONAME}))
##    LIBS += $${DESTDIR}/lib$${dep}.$${SONAME}
#    DEPENDPATH +=
#    PRE_TARGETDEPS += $${LIBS_PATH}/$${dep}/lib/lib$${dep}$${LIB_SUFFIX}.$${SONAME}
#}

#! include( ../../dep.pri ) {
#    error( "Couldn't find the dep.pri file!" )
#}
