/*
 * Copyright © 2013 Red Hat, Inc.
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
#include <libudev.h>
#include <stdio.h>
#include <unistd.h>

#include "litest.h"

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

static int
new_device(struct libinput_device *device, void *user_data)
{
	return 0;
}

const struct libinput_interface simple_interface = {
	.open_restricted = open_restricted,
	.close_restricted = close_restricted,
	.new_device = new_device,
};


START_TEST(udev_create_NULL)
{
	struct libinput *li;
	const struct libinput_interface interface;
	struct udev *udev = (struct udev*)0xdeadbeef;
	const char *seat = (const char*)0xdeaddead;

	li = libinput_create_from_udev(NULL, NULL, NULL, NULL);
	ck_assert(li == NULL);

	li = libinput_create_from_udev(&interface, NULL, NULL, NULL);
	ck_assert(li == NULL);
	li = libinput_create_from_udev(NULL, NULL, udev, NULL);
	ck_assert(li == NULL);
	li = libinput_create_from_udev(NULL, NULL, NULL, seat);
	ck_assert(li == NULL);

	li = libinput_create_from_udev(&interface, NULL, udev, NULL);
	ck_assert(li == NULL);
	li = libinput_create_from_udev(NULL, NULL, udev, seat);
	ck_assert(li == NULL);

	li = libinput_create_from_udev(&interface, NULL, NULL, seat);
	ck_assert(li == NULL);
}
END_TEST

START_TEST(udev_create_seat0)
{
	struct libinput *li;
	struct libinput_event *event;
	struct udev *udev;
	int fd;

	udev = udev_new();
	ck_assert(udev != NULL);

	li = libinput_create_from_udev(&simple_interface, NULL, udev, "seat0");
	ck_assert(li != NULL);

	fd = libinput_get_fd(li);
	ck_assert_int_ge(fd, 0);

	/* expect at least one event */
	libinput_dispatch(li);
	event = libinput_get_event(li);
	ck_assert(event != NULL);

	libinput_event_destroy(event);
	libinput_destroy(li);
	udev_unref(udev);
}
END_TEST

START_TEST(udev_create_empty_seat)
{
	struct libinput *li;
	struct libinput_event *event;
	struct udev *udev;
	int fd;

	udev = udev_new();
	ck_assert(udev != NULL);

	/* expect a libinput reference, but no events */
	li = libinput_create_from_udev(&simple_interface, NULL, udev, "seatdoesntexist");
	ck_assert(li != NULL);

	fd = libinput_get_fd(li);
	ck_assert_int_ge(fd, 0);

	libinput_dispatch(li);
	event = libinput_get_event(li);
	ck_assert(event == NULL);

	libinput_event_destroy(event);
	libinput_destroy(li);
	udev_unref(udev);
}
END_TEST

/**
 * This test only works if there's at least one device in the system that is
 * assigned the default seat. Should cover the 99% case.
 */
START_TEST(udev_added_seat_default)
{
	struct libinput *li;
	struct libinput_event *event;
	struct udev *udev;
	struct libinput_event_added_seat *seat_event;
	struct libinput_seat *seat;
	const char *seat_name;
	enum libinput_event_type type;
	int default_seat_found = 0;

	udev = udev_new();
	ck_assert(udev != NULL);

	li = libinput_create_from_udev(&simple_interface, NULL, udev, "seat0");
	ck_assert(li != NULL);
	libinput_dispatch(li);

	while (!default_seat_found && (event = libinput_get_event(li))) {
		type = libinput_event_get_type(event);
		if (type != LIBINPUT_EVENT_ADDED_SEAT) {
			libinput_event_destroy(event);
			continue;
		}

		seat_event = (struct libinput_event_added_seat*)event;
		seat = libinput_event_added_seat_get_seat(seat_event);
		ck_assert(seat != NULL);

		seat_name = libinput_seat_get_name(seat);
		default_seat_found = !strcmp(seat_name, "default");
		libinput_event_destroy(event);
	}

	ck_assert(default_seat_found);

	libinput_destroy(li);
	udev_unref(udev);
}
END_TEST

START_TEST(udev_removed_seat)
{
	struct libinput *li;
	struct libinput_event *event;
	struct udev *udev;
#define MAX_SEATS 30
	char *seat_names[MAX_SEATS] = {0};
	int idx = 0;

	udev = udev_new();
	ck_assert(udev != NULL);

	li = libinput_create_from_udev(&simple_interface, NULL, udev, "seat0");
	ck_assert(li != NULL);
	libinput_dispatch(li);

	while ((event = libinput_get_event(li))) {
		enum libinput_event_type type;
		struct libinput_event_added_seat *seat_event;
		struct libinput_seat *seat;
		const char *seat_name;

		type = libinput_event_get_type(event);
		if (type != LIBINPUT_EVENT_ADDED_SEAT) {
			libinput_event_destroy(event);
			continue;
		}

		seat_event = (struct libinput_event_added_seat*)event;
		seat = libinput_event_added_seat_get_seat(seat_event);
		ck_assert(seat != NULL);

		seat_name = libinput_seat_get_name(seat);
		seat_names[idx++] = strdup(seat_name);
		ck_assert_int_lt(idx, MAX_SEATS);

		libinput_event_destroy(event);
	}

	libinput_suspend(li);

	while ((event = libinput_get_event(li))) {
		enum libinput_event_type type;
		struct libinput_event_removed_seat *seat_event;
		struct libinput_seat *seat;
		const char *seat_name;
		int i;

		type = libinput_event_get_type(event);
		if (type != LIBINPUT_EVENT_REMOVED_SEAT) {
			libinput_event_destroy(event);
			continue;
		}

		seat_event = (struct libinput_event_removed_seat*)event;
		seat = libinput_event_removed_seat_get_seat(seat_event);
		ck_assert(seat != NULL);

		seat_name = libinput_seat_get_name(seat);

		for (i = 0; i < idx; i++) {
			if (seat_names[i] &&
			    strcmp(seat_names[i], seat_name) == 0) {
				free(seat_names[i]);
				seat_names[i] = NULL;
				break;
			}
		}
		ck_assert_msg(i < idx,
			      "Seat '%s' unknown or already removed\n",
			      seat_name);

		libinput_event_destroy(event);
	}

	while(idx--) {
		ck_assert_msg(seat_names[idx] == NULL,
			      "Seat '%s' not removed\n",
			      seat_names[idx]);
	}

	libinput_destroy(li);
	udev_unref(udev);
#undef MAX_SEATS
}
END_TEST

START_TEST(udev_double_suspend)
{
	struct libinput *li;
	struct libinput_event *event;
	struct udev *udev;
	int fd;

	udev = udev_new();
	ck_assert(udev != NULL);

	li = libinput_create_from_udev(&simple_interface, NULL, udev, "seat0");
	ck_assert(li != NULL);

	fd = libinput_get_fd(li);
	ck_assert_int_ge(fd, 0);

	/* expect at least one event */
	ck_assert_int_ge(libinput_dispatch(li), 0);
	event = libinput_get_event(li);
	ck_assert(event != NULL);

	libinput_suspend(li);
	libinput_suspend(li);
	libinput_resume(li);

	libinput_event_destroy(event);
	libinput_destroy(li);
	udev_unref(udev);
}
END_TEST

START_TEST(udev_double_resume)
{
	struct libinput *li;
	struct libinput_event *event;
	struct udev *udev;
	int fd;

	udev = udev_new();
	ck_assert(udev != NULL);

	li = libinput_create_from_udev(&simple_interface, NULL, udev, "seat0");
	ck_assert(li != NULL);

	fd = libinput_get_fd(li);
	ck_assert_int_ge(fd, 0);

	/* expect at least one event */
	ck_assert_int_ge(libinput_dispatch(li), 0);
	event = libinput_get_event(li);
	ck_assert(event != NULL);

	libinput_suspend(li);
	libinput_resume(li);
	libinput_resume(li);

	libinput_event_destroy(event);
	libinput_destroy(li);
	udev_unref(udev);
}
END_TEST

static void
process_events_count_devices(struct libinput *li, int *device_count)
{
	struct libinput_event *event;

	while ((event = libinput_get_event(li))) {
		switch (libinput_event_get_type(event)) {
		case LIBINPUT_EVENT_REMOVED_DEVICE:
			(*device_count)--;
			break;
		default:
			break;
		}
		libinput_event_destroy(event);
	}
}

static int
suspend_resume_test_new_device(struct libinput_device *device, void *user_data)
{
	int *num_devices = user_data;

	(*num_devices)++;

	return 0;
}

const struct libinput_interface suspend_resume_test_interface = {
	.open_restricted = open_restricted,
	.close_restricted = close_restricted,
	.new_device = suspend_resume_test_new_device,
};


START_TEST(udev_suspend_resume)
{
	struct libinput *li;
	struct udev *udev;
	int fd;
	int num_devices = 0;

	udev = udev_new();
	ck_assert(udev != NULL);

	li = libinput_create_from_udev(&suspend_resume_test_interface,
				       &num_devices, udev, "seat0");
	ck_assert(li != NULL);

	fd = libinput_get_fd(li);
	ck_assert_int_ge(fd, 0);

	/* Check that at least one device was discovered after creation. */
	ck_assert_int_ge(libinput_dispatch(li), 0);
	process_events_count_devices(li, &num_devices);
	ck_assert_int_gt(num_devices, 0);

	/* Check that after a suspend, no devices are left. */
	libinput_suspend(li);
	ck_assert_int_ge(libinput_dispatch(li), 0);
	process_events_count_devices(li, &num_devices);
	ck_assert_int_eq(num_devices, 0);

	/* Check that after a resume, at least one device is discovered. */
	libinput_resume(li);
	ck_assert_int_ge(libinput_dispatch(li), 0);
	process_events_count_devices(li, &num_devices);
	ck_assert_int_gt(num_devices, 0);

	libinput_destroy(li);
	udev_unref(udev);
}
END_TEST

int main (int argc, char **argv) {

	litest_add_no_device("udev:create", udev_create_NULL);
	litest_add_no_device("udev:create", udev_create_seat0);
	litest_add_no_device("udev:create", udev_create_empty_seat);

	litest_add_no_device("udev:seat events", udev_added_seat_default);
	litest_add_no_device("udev:seat events", udev_removed_seat);

	litest_add("udev:suspend", udev_double_suspend, LITEST_ANY, LITEST_ANY);
	litest_add("udev:suspend", udev_double_resume, LITEST_ANY, LITEST_ANY);
	litest_add("udev:suspend", udev_suspend_resume, LITEST_ANY, LITEST_ANY);

	return litest_run(argc, argv);
}
