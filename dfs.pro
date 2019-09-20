TEMPLATE = subdirs


SUBDIRS += \
          src/client  \
          src/server  \
          src/common \
          src/dfs_proto \
          tests/network



#client.depends = common
#client.depends = dfs_proto

#server.depends = common
#server.depends = dfs_proto

#common.depends = dfs_proto
