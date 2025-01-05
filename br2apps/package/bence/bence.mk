BENCE_VERSION = 2.3
BENCE_SOURCE = 
BENCE_LICENSE = MIT
BENCE_LICENSE_FILES =
MY_OVERLAY_FILES= $(BR2_EXTERNAL_APPS_PATH)/package/bence/overlay
#Config: $(BR2_EXTERNAL_APPS_PATH)/package/bence/Config.in
# A build parancsok megadása: buildelés a helyi fájlok alapján
define BENCE_BUILD_CMDS
    $(TARGET_CC) $(TARGET_CFLAGS) -o $(TARGET_DIR)/usr/bin/hello $(BR2_EXTERNAL_APPS_PATH)/package/bence/raw/hello.c
    $(TARGET_CC) $(TARGET_CFLAGS) -o $(TARGET_DIR)/usr/bin/christmastree $(BR2_EXTERNAL_APPS_PATH)/package/bence/raw/christmastree.c
    $(TARGET_CC) $(TARGET_CFLAGS) -o $(TARGET_DIR)/usr/bin/tog_gpio $(BR2_EXTERNAL_APPS_PATH)/package/bence/raw/tog_gpio.c -lgpiod
    $(TARGET_CC) $(TARGET_CFLAGS) -o $(TARGET_DIR)/usr/bin/shift_gpio $(BR2_EXTERNAL_APPS_PATH)/package/bence/raw/shift.c -lgpiod
    $(TARGET_CC) $(TARGET_CFLAGS) -o $(TARGET_DIR)/usr/bin/teszt_cube $(BR2_EXTERNAL_APPS_PATH)/package/bence/raw/teszt_cube.c -lgpiod
    $(TARGET_CC) $(TARGET_CFLAGS) -o $(TARGET_DIR)/usr/bin/clock_teszt $(BR2_EXTERNAL_APPS_PATH)/package/bence/raw/clock_teszt.c -lgpiod
    cp -r $(MY_OVERLAY_FILES)/* $(TARGET_DIR)/
endef
define BENCE_INSTALL_TARGET_CMDS
    @echo "Installing overlay files..."
    cp -r $(MY_OVERLAY_FILES)/* $(TARGET_DIR)/
    @echo "Overlay files installed successfully!"
endef

$(eval $(generic-package))

