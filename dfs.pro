TEMPLATE = subdirs


SUBDIRS += \
          src/client  \
          src/dfs_server  \
          src/common \
          src/dfs_proto \
          tests/test_dfs_net



#client.depends = common
#client.depends = dfs_proto

#server.depends = common
#server.depends = dfs_proto

#common.depends = dfs_proto
