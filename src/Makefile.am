lib_LTLIBRARIES = libinput.la

include_HEADERS =			\
	libinput.h

libinput_la_SOURCES =			\
	libinput.c			\
	libinput.h			\
	libinput-util.c			\
	libinput-util.h			\
	evdev.c				\
	evdev.h				\
	evdev-touchpad.c		\
	filter.c			\
	filter.h			\
	path.h				\
	path.c				\
	udev-seat.c			\
	udev-seat.h

libinput_la_LIBADD = $(MTDEV_LIBS) \
		     $(LIBUDEV_LIBS) \
		     -lm
libinput_la_CFLAGS = $(MTDEV_CFLAGS)	\
		     $(LIBUDEV_CFLAGS)	\
		     $(GCC_CFLAGS)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libinput.pc

AM_CPPFLAGS = $(FFI_CFLAGS)
AM_CFLAGS = $(GCC_CFLAGS)

DISTCLEANFILES = libinput-version.h
EXTRA_DIST = libinput-version.h.in
