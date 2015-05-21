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
#include <libinput-util.h>
#include <unistd.h>

#include "litest.h"

static int open_restricted(const char *path, int flags, void *data)
{
	int fd = open(path, flags);
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

static struct libevdev_uinput *
create_simple_test_device(const char *name, ...)
{
	va_list args;
	struct libevdev_uinput *uinput;
	struct libevdev *evdev;
	unsigned int type, code;
	int rc;
	struct input_absinfo abs = {
		.value = -1,
		.minimum = 0,
		.maximum = 100,
		.fuzz = 0,
		.flat = 0,
		.resolution = 100,
	};

	evdev = libevdev_new();
	litest_assert_notnull(evdev);
	libevdev_set_name(evdev, name);

	va_start(args, name);

	while ((type = va_arg(args, unsigned int)) != (unsigned int)-1 &&
	       (code = va_arg(args, unsigned int)) != (unsigned int)-1) {
		const struct input_absinfo *a = NULL;
		if (type == EV_ABS)
			a = &abs;
		libevdev_enable_event_code(evdev, type, code, a);
	}

	va_end(args);

	rc = libevdev_uinput_create_from_device(evdev,
						LIBEVDEV_UINPUT_OPEN_MANAGED,
						&uinput);
	litest_assert_int_eq(rc, 0);
	libevdev_free(evdev);

	return uinput;
}

START_TEST(event_conversion_device_notify)
{
	struct libevdev_uinput *uinput;
	struct libinput *li;
	struct libinput_event *event;
	int device_added = 0, device_removed = 0;

	uinput = create_simple_test_device("litest test device",
					   EV_REL, REL_X,
					   EV_REL, REL_Y,
					   EV_KEY, BTN_LEFT,
					   EV_KEY, BTN_MIDDLE,
					   EV_KEY, BTN_LEFT,
					   -1, -1);
	li = libinput_path_create_context(&simple_interface, NULL);
	libinput_path_add_device(li, libevdev_uinput_get_devnode(uinput));

	libinput_dispatch(li);
	libinput_suspend(li);
	libinput_resume(li);

	while ((event = libinput_get_event(li))) {
		enum libinput_event_type type;
		type = libinput_event_get_type(event);

		if (type == LIBINPUT_EVENT_DEVICE_ADDED ||
		    type == LIBINPUT_EVENT_DEVICE_REMOVED) {
			struct libinput_event_device_notify *dn;
			struct libinput_event *base;
			dn = libinput_event_get_device_notify_event(event);
			base = libinput_event_device_notify_get_base_event(dn);
			ck_assert(event == base);

			if (type == LIBINPUT_EVENT_DEVICE_ADDED)
				device_added++;
			else if (type == LIBINPUT_EVENT_DEVICE_REMOVED)
				device_removed++;

			litest_disable_log_handler(li);
			ck_assert(libinput_event_get_pointer_event(event) == NULL);
			ck_assert(libinput_event_get_keyboard_event(event) == NULL);
			ck_assert(libinput_event_get_touch_event(event) == NULL);
			litest_restore_log_handler(li);
		}

		libinput_event_destroy(event);
	}

	libinput_unref(li);
	libevdev_uinput_destroy(uinput);

	ck_assert_int_gt(device_added, 0);
	ck_assert_int_gt(device_removed, 0);
}
END_TEST

START_TEST(event_conversion_pointer)
{
	struct litest_device *dev = litest_current_device();
	struct libinput *li = dev->libinput;
	struct libinput_event *event;
	int motion = 0, button = 0;

	/* Queue at least two relative motion events as the first one may
	 * be absorbed by the pointer acceleration filter. */
	litest_event(dev, EV_REL, REL_X, -1);
	litest_event(dev, EV_REL, REL_Y, -1);
	litest_event(dev, EV_SYN, SYN_REPORT, 0);
	litest_event(dev, EV_REL, REL_X, -1);
	litest_event(dev, EV_REL, REL_Y, -1);
	litest_event(dev, EV_KEY, BTN_LEFT, 1);
	litest_event(dev, EV_SYN, SYN_REPORT, 0);

	libinput_dispatch(li);

	while ((event = libinput_get_event(li))) {
		enum libinput_event_type type;
		type = libinput_event_get_type(event);

		if (type == LIBINPUT_EVENT_POINTER_MOTION ||
		    type == LIBINPUT_EVENT_POINTER_BUTTON) {
			struct libinput_event_pointer *p;
			struct libinput_event *base;
			p = libinput_event_get_pointer_event(event);
			base = libinput_event_pointer_get_base_event(p);
			ck_assert(event == base);

			if (type == LIBINPUT_EVENT_POINTER_MOTION)
				motion++;
			else if (type == LIBINPUT_EVENT_POINTER_BUTTON)
				button++;

			litest_disable_log_handler(li);
			ck_assert(libinput_event_get_device_notify_event(event) == NULL);
			ck_assert(libinput_event_get_keyboard_event(event) == NULL);
			ck_assert(libinput_event_get_touch_event(event) == NULL);
			litest_restore_log_handler(li);
		}
		libinput_event_destroy(event);
	}

	ck_assert_int_gt(motion, 0);
	ck_assert_int_gt(button, 0);
}
END_TEST

START_TEST(event_conversion_pointer_abs)
{
	struct litest_device *dev = litest_current_device();
	struct libinput *li = dev->libinput;
	struct libinput_event *event;
	int motion = 0, button = 0;

	litest_event(dev, EV_ABS, ABS_X, 10);
	litest_event(dev, EV_ABS, ABS_Y, 50);
	litest_event(dev, EV_KEY, BTN_LEFT, 1);
	litest_event(dev, EV_SYN, SYN_REPORT, 0);
	litest_event(dev, EV_ABS, ABS_X, 30);
	litest_event(dev, EV_ABS, ABS_Y, 30);
	litest_event(dev, EV_SYN, SYN_REPORT, 0);

	libinput_dispatch(li);

	while ((event = libinput_get_event(li))) {
		enum libinput_event_type type;
		type = libinput_event_get_type(event);

		if (type == LIBINPUT_EVENT_POINTER_MOTION_ABSOLUTE ||
		    type == LIBINPUT_EVENT_POINTER_BUTTON) {
			struct libinput_event_pointer *p;
			struct libinput_event *base;
			p = libinput_event_get_pointer_event(event);
			base = libinput_event_pointer_get_base_event(p);
			ck_assert(event == base);

			if (type == LIBINPUT_EVENT_POINTER_MOTION_ABSOLUTE)
				motion++;
			else if (type == LIBINPUT_EVENT_POINTER_BUTTON)
				button++;

			litest_disable_log_handler(li);
			ck_assert(libinput_event_get_device_notify_event(event) == NULL);
			ck_assert(libinput_event_get_keyboard_event(event) == NULL);
			ck_assert(libinput_event_get_touch_event(event) == NULL);
			litest_restore_log_handler(li);
		}
		libinput_event_destroy(event);
	}

	ck_assert_int_gt(motion, 0);
	ck_assert_int_gt(button, 0);
}
END_TEST

START_TEST(event_conversion_key)
{
	struct litest_device *dev = litest_current_device();
	struct libinput *li = dev->libinput;
	struct libinput_event *event;
	int key = 0;

	litest_event(dev, EV_KEY, KEY_A, 1);
	litest_event(dev, EV_SYN, SYN_REPORT, 0);
	litest_event(dev, EV_KEY, KEY_A, 0);
	litest_event(dev, EV_SYN, SYN_REPORT, 0);

	libinput_dispatch(li);

	while ((event = libinput_get_event(li))) {
		enum libinput_event_type type;
		type = libinput_event_get_type(event);

		if (type == LIBINPUT_EVENT_KEYBOARD_KEY) {
			struct libinput_event_keyboard *k;
			struct libinput_event *base;
			k = libinput_event_get_keyboard_event(event);
			base = libinput_event_keyboard_get_base_event(k);
			ck_assert(event == base);

			key++;

			litest_disable_log_handler(li);
			ck_assert(libinput_event_get_device_notify_event(event) == NULL);
			ck_assert(libinput_event_get_pointer_event(event) == NULL);
			ck_assert(libinput_event_get_touch_event(event) == NULL);
			litest_restore_log_handler(li);
		}
		libinput_event_destroy(event);
	}

	ck_assert_int_gt(key, 0);
}
END_TEST

START_TEST(event_conversion_touch)
{
	struct litest_device *dev = litest_current_device();
	struct libinput *li = dev->libinput;
	struct libinput_event *event;
	int touch = 0;

	libinput_dispatch(li);

	litest_event(dev, EV_KEY, BTN_TOOL_FINGER, 1);
	litest_event(dev, EV_KEY, BTN_TOUCH, 1);
	litest_event(dev, EV_ABS, ABS_X, 10);
	litest_event(dev, EV_ABS, ABS_Y, 10);
	litest_event(dev, EV_ABS, ABS_MT_SLOT, 0);
	litest_event(dev, EV_ABS, ABS_MT_TRACKING_ID, 1);
	litest_event(dev, EV_ABS, ABS_MT_POSITION_X, 10);
	litest_event(dev, EV_ABS, ABS_MT_POSITION_Y, 10);
	litest_event(dev, EV_SYN, SYN_REPORT, 0);

	libinput_dispatch(li);

	while ((event = libinput_get_event(li))) {
		enum libinput_event_type type;
		type = libinput_event_get_type(event);

		if (type >= LIBINPUT_EVENT_TOUCH_DOWN &&
		    type <= LIBINPUT_EVENT_TOUCH_FRAME) {
			struct libinput_event_touch *t;
			struct libinput_event *base;
			t = libinput_event_get_touch_event(event);
			base = libinput_event_touch_get_base_event(t);
			ck_assert(event == base);

			touch++;

			litest_disable_log_handler(li);
			ck_assert(libinput_event_get_device_notify_event(event) == NULL);
			ck_assert(libinput_event_get_pointer_event(event) == NULL);
			ck_assert(libinput_event_get_keyboard_event(event) == NULL);
			litest_restore_log_handler(li);
		}
		libinput_event_destroy(event);
	}

	ck_assert_int_gt(touch, 0);
}
END_TEST

START_TEST(context_ref_counting)
{
	struct libinput *li;

	/* These tests rely on valgrind to detect memory leak and use after
	 * free errors. */

	li = libinput_path_create_context(&simple_interface, NULL);
	ck_assert_notnull(li);
	ck_assert_ptr_eq(libinput_unref(li), NULL);

	li = libinput_path_create_context(&simple_interface, NULL);
	ck_assert_notnull(li);
	ck_assert_ptr_eq(libinput_ref(li), li);
	ck_assert_ptr_eq(libinput_unref(li), li);
	ck_assert_ptr_eq(libinput_unref(li), NULL);
}
END_TEST

START_TEST(config_status_string)
{
	const char *strs[3];
	const char *invalid;
	size_t i, j;

	strs[0] = libinput_config_status_to_str(LIBINPUT_CONFIG_STATUS_SUCCESS);
	strs[1] = libinput_config_status_to_str(LIBINPUT_CONFIG_STATUS_UNSUPPORTED);
	strs[2] = libinput_config_status_to_str(LIBINPUT_CONFIG_STATUS_INVALID);

	for (i = 0; i < ARRAY_LENGTH(strs) - 1; i++)
		for (j = i + 1; j < ARRAY_LENGTH(strs); j++)
			ck_assert_str_ne(strs[i], strs[j]);

	invalid = libinput_config_status_to_str(LIBINPUT_CONFIG_STATUS_INVALID + 1);
	ck_assert(invalid == NULL);
	invalid = libinput_config_status_to_str(LIBINPUT_CONFIG_STATUS_SUCCESS - 1);
	ck_assert(invalid == NULL);
}
END_TEST

START_TEST(matrix_helpers)
{
	struct matrix m1, m2, m3;
	float f[6] = { 1, 2, 3, 4, 5, 6 };
	int x, y;
	int row, col;

	matrix_init_identity(&m1);

	for (row = 0; row < 3; row++) {
		for (col = 0; col < 3; col++) {
			ck_assert_int_eq(m1.val[row][col],
					 (row == col) ? 1 : 0);
		}
	}
	ck_assert(matrix_is_identity(&m1));

	matrix_from_farray6(&m2, f);
	ck_assert_int_eq(m2.val[0][0], 1);
	ck_assert_int_eq(m2.val[0][1], 2);
	ck_assert_int_eq(m2.val[0][2], 3);
	ck_assert_int_eq(m2.val[1][0], 4);
	ck_assert_int_eq(m2.val[1][1], 5);
	ck_assert_int_eq(m2.val[1][2], 6);
	ck_assert_int_eq(m2.val[2][0], 0);
	ck_assert_int_eq(m2.val[2][1], 0);
	ck_assert_int_eq(m2.val[2][2], 1);

	x = 100;
	y = 5;
	matrix_mult_vec(&m1, &x, &y);
	ck_assert_int_eq(x, 100);
	ck_assert_int_eq(y, 5);

	matrix_mult(&m3, &m1, &m1);
	ck_assert(matrix_is_identity(&m3));

	matrix_init_scale(&m2, 2, 4);
	ck_assert_int_eq(m2.val[0][0], 2);
	ck_assert_int_eq(m2.val[0][1], 0);
	ck_assert_int_eq(m2.val[0][2], 0);
	ck_assert_int_eq(m2.val[1][0], 0);
	ck_assert_int_eq(m2.val[1][1], 4);
	ck_assert_int_eq(m2.val[1][2], 0);
	ck_assert_int_eq(m2.val[2][0], 0);
	ck_assert_int_eq(m2.val[2][1], 0);
	ck_assert_int_eq(m2.val[2][2], 1);

	matrix_mult_vec(&m2, &x, &y);
	ck_assert_int_eq(x, 200);
	ck_assert_int_eq(y, 20);

	matrix_init_translate(&m2, 10, 100);
	ck_assert_int_eq(m2.val[0][0], 1);
	ck_assert_int_eq(m2.val[0][1], 0);
	ck_assert_int_eq(m2.val[0][2], 10);
	ck_assert_int_eq(m2.val[1][0], 0);
	ck_assert_int_eq(m2.val[1][1], 1);
	ck_assert_int_eq(m2.val[1][2], 100);
	ck_assert_int_eq(m2.val[2][0], 0);
	ck_assert_int_eq(m2.val[2][1], 0);
	ck_assert_int_eq(m2.val[2][2], 1);

	matrix_mult_vec(&m2, &x, &y);
	ck_assert_int_eq(x, 210);
	ck_assert_int_eq(y, 120);

	matrix_to_farray6(&m2, f);
	ck_assert_int_eq(f[0], 1);
	ck_assert_int_eq(f[1], 0);
	ck_assert_int_eq(f[2], 10);
	ck_assert_int_eq(f[3], 0);
	ck_assert_int_eq(f[4], 1);
	ck_assert_int_eq(f[5], 100);
}
END_TEST

START_TEST(ratelimit_helpers)
{
	struct ratelimit rl;
	unsigned int i, j;

	/* 10 attempts every 100ms */
	ratelimit_init(&rl, 100, 10);

	for (j = 0; j < 3; ++j) {
		/* a burst of 9 attempts must succeed */
		for (i = 0; i < 9; ++i) {
			ck_assert_int_eq(ratelimit_test(&rl),
					 RATELIMIT_PASS);
		}

		/* the 10th attempt reaches the threshold */
		ck_assert_int_eq(ratelimit_test(&rl), RATELIMIT_THRESHOLD);

		/* ..then further attempts must fail.. */
		ck_assert_int_eq(ratelimit_test(&rl), RATELIMIT_EXCEEDED);

		/* ..regardless of how often we try. */
		for (i = 0; i < 100; ++i) {
			ck_assert_int_eq(ratelimit_test(&rl),
					 RATELIMIT_EXCEEDED);
		}

		/* ..even after waiting 20ms */
		msleep(20);
		for (i = 0; i < 100; ++i) {
			ck_assert_int_eq(ratelimit_test(&rl),
					 RATELIMIT_EXCEEDED);
		}

		/* but after 100ms the counter is reset */
		msleep(90); /* +10ms to account for time drifts */
	}
}
END_TEST

struct parser_test {
	char *tag;
	int expected_value;
};

START_TEST(dpi_parser)
{
	struct parser_test tests[] = {
		{ "450 *1800 3200", 1800 },
		{ "*450 1800 3200", 450 },
		{ "450 1800 *3200", 3200 },
		{ "450 1800 3200", 3200 },
		{ "450 1800 failboat", 0 },
		{ "450 1800 *failboat", 0 },
		{ "0 450 1800 *3200", 0 },
		{ "450@37 1800@12 *3200@6", 3200 },
		{ "450@125 1800@125   *3200@125  ", 3200 },
		{ "450@125 *1800@125  3200@125", 1800 },
		{ "*this @string fails", 0 },
		{ "12@34 *45@", 0 },
		{ "12@a *45@", 0 },
		{ "12@a *45@25", 0 },
		{ "                                      * 12, 450, 800", 0 },
		{ "                                      *12, 450, 800", 12 },
		{ "*12, *450, 800", 12 },
		{ "*-23412, 450, 800", 0 },
		{ "112@125, 450@125, 800@125, 900@-125", 0 },
		{ "", 0 },
		{ "   ", 0 },
		{ "* ", 0 },
		{ NULL, 0 }
	};
	int i, dpi;

	for (i = 0; tests[i].tag != NULL; i++) {
		dpi = parse_mouse_dpi_property(tests[i].tag);
		ck_assert_int_eq(dpi, tests[i].expected_value);
	}
}
END_TEST

START_TEST(wheel_click_parser)
{
	struct parser_test tests[] = {
		{ "1", 1 },
		{ "10", 10 },
		{ "-12", -12 },
		{ "360", 360 },
		{ "66 ", 66 },
		{ "   100 ", 100 },

		{ "0", 0 },
		{ "-0", 0 },
		{ "a", 0 },
		{ "10a", 0 },
		{ "10-", 0 },
		{ "sadfasfd", 0 },
		{ "361", 0 },
		{ NULL, 0 }
	};

	int i, angle;

	for (i = 0; tests[i].tag != NULL; i++) {
		angle = parse_mouse_wheel_click_angle_property(tests[i].tag);
		ck_assert_int_eq(angle, tests[i].expected_value);
	}
}
END_TEST

struct parser_test_float {
	char *tag;
	double expected_value;
};

START_TEST(trackpoint_accel_parser)
{
	struct parser_test_float tests[] = {
		{ "0.5", 0.5 },
		{ "1.0", 1.0 },
		{ "2.0", 2.0 },
		{ "fail1.0", 0.0 },
		{ "1.0fail", 0.0 },
		{ "0,5", 0.0 },
		{ NULL, 0.0 }
	};
	int i;
	double accel;

	for (i = 0; tests[i].tag != NULL; i++) {
		accel = parse_trackpoint_accel_property(tests[i].tag);
		ck_assert(accel == tests[i].expected_value);
	}
}
END_TEST

void
litest_setup_tests(void)
{
	litest_add_no_device("events:conversion", event_conversion_device_notify);
	litest_add_for_device("events:conversion", event_conversion_pointer, LITEST_MOUSE);
	litest_add_for_device("events:conversion", event_conversion_pointer, LITEST_MOUSE);
	litest_add_for_device("events:conversion", event_conversion_pointer_abs, LITEST_XEN_VIRTUAL_POINTER);
	litest_add_for_device("events:conversion", event_conversion_key, LITEST_KEYBOARD);
	litest_add_for_device("events:conversion", event_conversion_touch, LITEST_WACOM_TOUCH);

	litest_add_no_device("context:refcount", context_ref_counting);
	litest_add_no_device("config:status string", config_status_string);

	litest_add_no_device("misc:matrix", matrix_helpers);
	litest_add_no_device("misc:ratelimit", ratelimit_helpers);
	litest_add_no_device("misc:dpi parser", dpi_parser);
	litest_add_no_device("misc:wheel click parser", wheel_click_parser);
	litest_add_no_device("misc:trackpoint accel parser", trackpoint_accel_parser);
}
