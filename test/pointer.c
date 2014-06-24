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

#include <stdio.h>
#include <check.h>
#include <errno.h>
#include <fcntl.h>
#include <libinput.h>
#include <math.h>
#include <unistd.h>

#include "libinput-util.h"
#include "litest.h"

static void
test_relative_event(struct litest_device *dev, int dx, int dy)
{
	struct libinput *li = dev->libinput;
	struct libinput_event *event;
	struct libinput_event_pointer *ptrev;
	double ev_dx, ev_dy;
	double expected_dir;
	double expected_length;
	double actual_dir;
	double actual_length;

	/* Send two deltas, as the first one may be eaten up by an
	 * acceleration filter. */
	litest_event(dev, EV_REL, REL_X, dx);
	litest_event(dev, EV_REL, REL_Y, dy);
	litest_event(dev, EV_SYN, SYN_REPORT, 0);
	litest_event(dev, EV_REL, REL_X, dx);
	litest_event(dev, EV_REL, REL_Y, dy);
	litest_event(dev, EV_SYN, SYN_REPORT, 0);

	libinput_dispatch(li);

	event = libinput_get_event(li);
	ck_assert(event != NULL);
	ck_assert_int_eq(libinput_event_get_type(event), LIBINPUT_EVENT_POINTER_MOTION);

	ptrev = libinput_event_get_pointer_event(event);
	ck_assert(ptrev != NULL);

	expected_length = sqrt(dx*dx + dy*dy);
	expected_dir = atan2(dx, dy);

	ev_dx = libinput_event_pointer_get_dx(ptrev);
	ev_dy = libinput_event_pointer_get_dy(ptrev);
	actual_length = sqrt(ev_dx*ev_dx + ev_dy*ev_dy);
	actual_dir = atan2(ev_dx, ev_dy);

	/* Check the length of the motion vector (tolerate 1.0 indifference). */
	ck_assert(fabs(expected_length - actual_length) < 1.0);

	/* Check the direction of the motion vector (tolerate 2π/4 radians
	 * indifference). */
	ck_assert(fabs(expected_dir - actual_dir) < M_PI_2);

	libinput_event_destroy(event);

	litest_drain_events(dev->libinput);
}

START_TEST(pointer_motion_relative)
{
	struct litest_device *dev = litest_current_device();

	litest_drain_events(dev->libinput);

	test_relative_event(dev, 1, 0);
	test_relative_event(dev, 1, 1);
	test_relative_event(dev, 1, -1);
	test_relative_event(dev, 0, 1);

	test_relative_event(dev, -1, 0);
	test_relative_event(dev, -1, 1);
	test_relative_event(dev, -1, -1);
	test_relative_event(dev, 0, -1);
}
END_TEST

static void
test_button_event(struct litest_device *dev, int button, int state)
{
	struct libinput *li = dev->libinput;
	struct libinput_event *event;
	struct libinput_event_pointer *ptrev;

	litest_event(dev, EV_KEY, button, state);
	litest_event(dev, EV_SYN, SYN_REPORT, 0);

	libinput_dispatch(li);

	event = libinput_get_event(li);
	ck_assert(event != NULL);
	ck_assert_int_eq(libinput_event_get_type(event), LIBINPUT_EVENT_POINTER_BUTTON);

	ptrev = libinput_event_get_pointer_event(event);
	ck_assert(ptrev != NULL);
	ck_assert_int_eq(libinput_event_pointer_get_button(ptrev), button);
	ck_assert_int_eq(libinput_event_pointer_get_button_state(ptrev),
			 state ?
				LIBINPUT_BUTTON_STATE_PRESSED :
				LIBINPUT_BUTTON_STATE_RELEASED);
	libinput_event_destroy(event);
}

START_TEST(pointer_button)
{
	struct litest_device *dev = litest_current_device();

	litest_drain_events(dev->libinput);

	test_button_event(dev, BTN_LEFT, 1);
	test_button_event(dev, BTN_LEFT, 0);

	/* press it twice for good measure */
	test_button_event(dev, BTN_LEFT, 1);
	test_button_event(dev, BTN_LEFT, 0);

	if (libevdev_has_event_code(dev->evdev, EV_KEY, BTN_RIGHT)) {
		test_button_event(dev, BTN_RIGHT, 1);
		test_button_event(dev, BTN_RIGHT, 0);
	}

	if (libevdev_has_event_code(dev->evdev, EV_KEY, BTN_MIDDLE)) {
		test_button_event(dev, BTN_MIDDLE, 1);
		test_button_event(dev, BTN_MIDDLE, 0);
	}
}
END_TEST

static void
test_wheel_event(struct litest_device *dev, int which, int amount)
{
	struct libinput *li = dev->libinput;
	struct libinput_event *event;
	struct libinput_event_pointer *ptrev;

	/* the current evdev implementation scales the scroll wheel events
	   up by a factor 10 */
	const int scroll_step = 10;
	int expected = amount * scroll_step;

	/* mouse scroll wheels are 'upside down' */
	if (which == REL_WHEEL)
		amount *= -1;
	litest_event(dev, EV_REL, which, amount);
	litest_event(dev, EV_SYN, SYN_REPORT, 0);

	libinput_dispatch(li);

	event = libinput_get_event(li);
	ck_assert(event != NULL);
	ck_assert_int_eq(libinput_event_get_type(event),
			  LIBINPUT_EVENT_POINTER_AXIS);

	ptrev = libinput_event_get_pointer_event(event);
	ck_assert(ptrev != NULL);
	ck_assert_int_eq(libinput_event_pointer_get_axis(ptrev),
			 which == REL_WHEEL ?
				LIBINPUT_POINTER_AXIS_SCROLL_VERTICAL :
				LIBINPUT_POINTER_AXIS_SCROLL_HORIZONTAL);
	ck_assert_int_eq(libinput_event_pointer_get_axis_value(ptrev), expected);
	libinput_event_destroy(event);
}

START_TEST(pointer_scroll_wheel)
{
	struct litest_device *dev = litest_current_device();

	litest_drain_events(dev->libinput);

	test_wheel_event(dev, REL_WHEEL, -1);
	test_wheel_event(dev, REL_WHEEL, 1);

	test_wheel_event(dev, REL_WHEEL, -5);
	test_wheel_event(dev, REL_WHEEL, 6);

	if (libevdev_has_event_code(dev->evdev, EV_REL, REL_HWHEEL)) {
		test_wheel_event(dev, REL_HWHEEL, -1);
		test_wheel_event(dev, REL_HWHEEL, 1);

		test_wheel_event(dev, REL_HWHEEL, -5);
		test_wheel_event(dev, REL_HWHEEL, 6);
	}
}
END_TEST

START_TEST(pointer_seat_button_count)
{
	const int num_devices = 4;
	struct litest_device *devices[num_devices];
	struct libinput *libinput;
	struct libinput_event *ev;
	struct libinput_event_pointer *tev;
	int i;
	int seat_button_count;
	int expected_seat_button_count = 0;
	char device_name[255];

	libinput = litest_create_context();
	for (i = 0; i < num_devices; ++i) {
		sprintf(device_name, "Generic mouse (%d)", i);
		devices[i] = litest_add_device_with_overrides(libinput,
							      LITEST_MOUSE,
							      device_name,
							      NULL, NULL, NULL);
	}

	for (i = 0; i < num_devices; ++i)
		litest_button_click(devices[i], BTN_LEFT, true);

	libinput_dispatch(libinput);
	while ((ev = libinput_get_event(libinput))) {
		if (libinput_event_get_type(ev) !=
		    LIBINPUT_EVENT_POINTER_BUTTON) {
			libinput_event_destroy(ev);
			libinput_dispatch(libinput);
			continue;
		}

		tev = libinput_event_get_pointer_event(ev);
		ck_assert_notnull(tev);
		ck_assert_int_eq(libinput_event_pointer_get_button(tev),
				 BTN_LEFT);
		ck_assert_int_eq(libinput_event_pointer_get_button_state(tev),
				 LIBINPUT_BUTTON_STATE_PRESSED);

		++expected_seat_button_count;
		seat_button_count =
			libinput_event_pointer_get_seat_button_count(tev);
		ck_assert_int_eq(expected_seat_button_count, seat_button_count);

		libinput_event_destroy(ev);
		libinput_dispatch(libinput);
	}

	ck_assert_int_eq(seat_button_count, num_devices);

	for (i = 0; i < num_devices; ++i)
		litest_button_click(devices[i], BTN_LEFT, false);

	libinput_dispatch(libinput);
	while ((ev = libinput_get_event(libinput))) {
		if (libinput_event_get_type(ev) !=
		    LIBINPUT_EVENT_POINTER_BUTTON) {
			libinput_event_destroy(ev);
			libinput_dispatch(libinput);
			continue;
		}

		tev = libinput_event_get_pointer_event(ev);
		ck_assert_notnull(tev);
		ck_assert_int_eq(libinput_event_pointer_get_button(tev),
				 BTN_LEFT);
		ck_assert_int_eq(libinput_event_pointer_get_button_state(tev),
				 LIBINPUT_BUTTON_STATE_RELEASED);

		--expected_seat_button_count;
		seat_button_count =
			libinput_event_pointer_get_seat_button_count(tev);
		ck_assert_int_eq(expected_seat_button_count, seat_button_count);

		libinput_event_destroy(ev);
		libinput_dispatch(libinput);
	}

	ck_assert_int_eq(seat_button_count, 0);

	for (i = 0; i < num_devices; ++i)
		litest_delete_device(devices[i]);
	libinput_unref(libinput);
}
END_TEST

int main (int argc, char **argv) {

	litest_add("pointer:motion", pointer_motion_relative, LITEST_POINTER, LITEST_ANY);
	litest_add("pointer:button", pointer_button, LITEST_BUTTON, LITEST_CLICKPAD);
	litest_add("pointer:scroll", pointer_scroll_wheel, LITEST_WHEEL, LITEST_ANY);
	litest_add_no_device("pointer:seat button count", pointer_seat_button_count);

	return litest_run(argc, argv);
}
