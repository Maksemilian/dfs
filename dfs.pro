TEMPLATE = subdirs


SUBDIRS += \
          src/dfs_client  \
          src/dfs_server  \
          src/dfs_net \
          src/dfs_proto \
          src/dfs_receiver \
          tests/test_dfs_net



#client.depends = common
#client.depends = dfs_proto

#server.depends = common
#server.depends = dfs_proto

#common.depends = dfs_proto
