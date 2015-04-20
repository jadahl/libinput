udevdir=$(UDEV_DIR)
udev_PROGRAMS = libinput-device-group

libinput_device_group_SOURCES = libinput-device-group.c
libinput_device_group_CFLAGS = $(LIBUDEV_CFLAGS) $(GCC_CFLAGS)
libinput_device_group_LDADD = $(LIBUDEV_LIBS)

udev_rulesdir=$(UDEV_DIR)/rules.d
dist_udev_rules_DATA = \
	80-libinput-device-groups.rules \
	90-libinput-model-quirks.rules

udev_hwdbdir=$(UDEV_DIR)/hwdb.d
dist_udev_hwdb_DATA = \
	90-libinput-model-quirks.hwdb
