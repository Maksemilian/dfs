TEMPLATE = subdirs

# включать только при полной сборке
# и выключать при запусках client and server
#CONFIG += ordered

SUBDIRS += \
          src/dfs_base \
# dfs_proto нужно собирать один раз
# а потом иключать из SUBDIRS
# иначе он будет собираться при каждом запуске client and server
#          src/dfs_proto \ #пересобирать только при смене версии сборки
          src/dfs_net \
          src/dfs_receiver \
          src/dfs_sync \
          src/dfs_client  \
          src/dfs_server  \
          tests/test_dfs_net

#dfs_client.depends = src/dfs_base src/dfs_proto src/dfs_net src/dfs_receiver

#dfs_net.depends = src/dfs_proto

#dfs_receiver.depends = src/dfs_base src/dfs_proto

#dfs_sync.depends = src/dfs_base src/dfs_proto

#dfs_server.depends = src/dfs_base src/dfs_proto src/dfs_net  src/dfs_receiver

#test_dfs_net.depends = src/dfs_proto src/dfs_net
