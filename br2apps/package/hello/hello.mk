HELLO_VERSION = 1.0
HELLO_SOURCE =
HELLO_LICENSE = MIT
HELLO_LICENSE_FILES =

define HELLO_BUILD_CMDS
    $(TARGET_CC) $(TARGET_CFLAGS) -o $(TARGET_DIR)/usr/bin/hello $(BR2_EXTERNAL_BREADBEE_PATH)/../br2apps/package/hello/hello.c
endef

$(eval $(generic-package))

