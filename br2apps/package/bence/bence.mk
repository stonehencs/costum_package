BENCE_VERSION = 2.0
BENCE_SOURCE = 
BENCE_LICENSE = MIT
BENCE_LICENSE_FILES =

# A build parancsok megadása: buildelés a helyi fájlok alapján
define BENCE_BUILD_CMDS
    $(TARGET_CC) $(TARGET_CFLAGS) -o $(TARGET_DIR)/usr/bin/hello $(BR2_EXTERNAL_BREADBEE_PATH)/../br2apps/package/bence/hello.c
    $(TARGET_CC) $(TARGET_CFLAGS) -o $(TARGET_DIR)/usr/bin/christmastree $(BR2_EXTERNAL_BREADBEE_PATH)/../br2apps/package/bence/christmastree.c
    $(TARGET_CC) $(TARGET_CFLAGS) -o $(TARGET_DIR)/usr/bin/tog_gpio $(BR2_EXTERNAL_BREADBEE_PATH)/../br2apps/package/bence/tog_gpio.c -lgpiod
    $(TARGET_CC) $(TARGET_CFLAGS) -o $(TARGET_DIR)/usr/bin/shift_gpio $(BR2_EXTERNAL_BREADBEE_PATH)/../br2apps/package/bence/shift.c -lgpiod
endef

$(eval $(generic-package))

