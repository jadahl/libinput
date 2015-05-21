/*
 * Copyright © 2014 Red Hat, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of the copyright holders not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  The copyright holders make
 * no representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 * THE COPYRIGHT HOLDERS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <config.h>

#include <check.h>
#include <errno.h>
#include <fcntl.h>
#include <libinput.h>
#include <unistd.h>

#include "litest.h"

static int log_handler_called;
static struct libinput *log_handler_context;

static int open_restricted(const char *path, int flags, void *data)
{
	int fd;
	fd = open(path, flags);
	return fd < 0 ? -errno : fd;
}
static void close_restricted(int fd, void *data)
{
	close(fd);
}

const struct libinput_interface simple_interface = {
	.open_restricted = open_restricted,
	.close_restricted = close_restricted,
};

static void
simple_log_handler(struct libinput *libinput,
		   enum libinput_log_priority priority,
		   const char *format,
		   va_list args)
{
	log_handler_called++;
	if (log_handler_context)
		litest_assert_ptr_eq(libinput, log_handler_context);
	litest_assert_notnull(format);
}

START_TEST(log_default_priority)
{
	enum libinput_log_priority pri;
	struct libinput *li;

	li = libinput_path_create_context(&simple_interface, NULL);
	pri = libinput_log_get_priority(li);

	ck_assert_int_eq(pri, LIBINPUT_LOG_PRIORITY_ERROR);

	libinput_unref(li);
}
END_TEST

START_TEST(log_handler_invoked)
{
	struct libinput *li;

	li = libinput_path_create_context(&simple_interface, NULL);

	libinput_log_set_priority(li, LIBINPUT_LOG_PRIORITY_DEBUG);
	libinput_log_set_handler(li, simple_log_handler);
	log_handler_context = li;

	libinput_path_add_device(li, "/tmp");

	ck_assert_int_gt(log_handler_called, 0);
	log_handler_called = 0;

	libinput_unref(li);

	log_handler_context = NULL;
}
END_TEST

START_TEST(log_handler_NULL)
{
	struct libinput *li;

	li = libinput_path_create_context(&simple_interface, NULL);
	libinput_log_set_priority(li, LIBINPUT_LOG_PRIORITY_DEBUG);
	libinput_log_set_handler(li, NULL);

	libinput_path_add_device(li, "/tmp");

	ck_assert_int_eq(log_handler_called, 0);
	log_handler_called = 0;

	libinput_unref(li);
}
END_TEST

START_TEST(log_priority)
{
	struct libinput *li;

	li = libinput_path_create_context(&simple_interface, NULL);
	libinput_log_set_priority(li, LIBINPUT_LOG_PRIORITY_ERROR);
	libinput_log_set_handler(li, simple_log_handler);
	log_handler_context = li;

	libinput_path_add_device(li, "/tmp");

	ck_assert_int_eq(log_handler_called, 1);

	libinput_log_set_priority(li, LIBINPUT_LOG_PRIORITY_INFO);
	/* event0 is usually Lid Switch which prints an info that
	   we don't handle it */
	libinput_path_add_device(li, "/dev/input/event0");
	ck_assert_int_gt(log_handler_called, 1);

	log_handler_called = 0;

	libinput_unref(li);
	log_handler_context = NULL;
}
END_TEST

void
litest_setup_tests(void)
{
	litest_add_no_device("log:defaults", log_default_priority);
	litest_add_no_device("log:logging", log_handler_invoked);
	litest_add_no_device("log:logging", log_handler_NULL);
	litest_add_no_device("log:logging", log_priority);
}
