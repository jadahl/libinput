noinst_PROGRAMS = event-debug

AM_CPPFLAGS = -I$(top_srcdir)/include \
              -I$(top_srcdir)/src

event_debug_SOURCES = event-debug.c
event_debug_LDADD = ../src/libinput.la $(LIBUDEV_LIBS)
event_debug_LDFLAGS = -static
event_debug_CFLAGS = $(LIBUDEV_CFLAGS)
