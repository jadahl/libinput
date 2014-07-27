/*
 * Copyright © 2014 Jonas Ådahl <jadahl@gmail.com>
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

#include "config.h"

#include <check.h>
#include <stdio.h>

#include "litest.h"

START_TEST(keyboard_seat_key_count)
{
	const int num_devices = 4;
	struct litest_device *devices[num_devices];
	struct libinput *libinput;
	struct libinput_event *ev;
	struct libinput_event_keyboard *kev;
	int i;
	int seat_key_count;
	int expected_key_button_count = 0;
	char device_name[255];

	libinput = litest_create_context();
	for (i = 0; i < num_devices; ++i) {
		sprintf(device_name, "litest Generic keyboard (%d)", i);
		devices[i] = litest_add_device_with_overrides(libinput,
							      LITEST_KEYBOARD,
							      device_name,
							      NULL, NULL, NULL);
	}

	for (i = 0; i < num_devices; ++i)
		litest_keyboard_key(devices[i], KEY_A, true);

	libinput_dispatch(libinput);
	while ((ev = libinput_get_event(libinput))) {
		if (libinput_event_get_type(ev) !=
		    LIBINPUT_EVENT_KEYBOARD_KEY) {
			libinput_event_destroy(ev);
			libinput_dispatch(libinput);
			continue;
		}

		kev = libinput_event_get_keyboard_event(ev);
		ck_assert_notnull(kev);
		ck_assert_int_eq(libinput_event_keyboard_get_key(kev), KEY_A);
		ck_assert_int_eq(libinput_event_keyboard_get_key_state(kev),
				 LIBINPUT_KEY_STATE_PRESSED);

		++expected_key_button_count;
		seat_key_count =
			libinput_event_keyboard_get_seat_key_count(kev);
		ck_assert_int_eq(expected_key_button_count, seat_key_count);

		libinput_event_destroy(ev);
		libinput_dispatch(libinput);
	}

	ck_assert_int_eq(seat_key_count, num_devices);

	for (i = 0; i < num_devices; ++i)
		litest_keyboard_key(devices[i], KEY_A, false);

	libinput_dispatch(libinput);
	while ((ev = libinput_get_event(libinput))) {
		if (libinput_event_get_type(ev) !=
		    LIBINPUT_EVENT_KEYBOARD_KEY) {
			libinput_event_destroy(ev);
			libinput_dispatch(libinput);
			continue;
		}

		kev = libinput_event_get_keyboard_event(ev);
		ck_assert_notnull(kev);
		ck_assert_int_eq(libinput_event_keyboard_get_key(kev), KEY_A);
		ck_assert_int_eq(libinput_event_keyboard_get_key_state(kev),
				 LIBINPUT_KEY_STATE_RELEASED);

		--expected_key_button_count;
		seat_key_count =
			libinput_event_keyboard_get_seat_key_count(kev);
		ck_assert_int_eq(expected_key_button_count, seat_key_count);

		libinput_event_destroy(ev);
		libinput_dispatch(libinput);
	}

	ck_assert_int_eq(seat_key_count, 0);

	for (i = 0; i < num_devices; ++i)
		litest_delete_device(devices[i]);
	libinput_unref(libinput);
}
END_TEST

START_TEST(keyboard_ignore_no_pressed_release)
{
	struct litest_device *dev;
	struct libinput *unused_libinput;
	struct libinput *libinput;
	struct libinput_event *event;
	struct libinput_event_keyboard *kevent;
	int events[] = {
		EV_KEY, KEY_A,
		-1, -1,
	};
	enum libinput_key_state *state;
	enum libinput_key_state expected_states[] = {
		LIBINPUT_KEY_STATE_PRESSED,
		LIBINPUT_KEY_STATE_RELEASED,
	};

	/* We can't send pressed -> released -> pressed events using uinput
	 * as such non-symmetric events are dropped. Work-around this by first
	 * adding the test device to the tested context after having sent an
	 * initial pressed event. */
	unused_libinput = litest_create_context();
	dev = litest_add_device_with_overrides(unused_libinput,
					       LITEST_KEYBOARD,
					       "Generic keyboard",
					       NULL, NULL, events);

	litest_keyboard_key(dev, KEY_A, true);
	litest_drain_events(unused_libinput);

	libinput = litest_create_context();
	libinput_path_add_device(libinput,
				 libevdev_uinput_get_devnode(dev->uinput));
	litest_drain_events(libinput);

	litest_keyboard_key(dev, KEY_A, false);
	litest_keyboard_key(dev, KEY_A, true);
	litest_keyboard_key(dev, KEY_A, false);

	libinput_dispatch(libinput);

	ARRAY_FOR_EACH(expected_states, state) {
		event = libinput_get_event(libinput);
		ck_assert_notnull(event);
		ck_assert_int_eq(libinput_event_get_type(event),
				 LIBINPUT_EVENT_KEYBOARD_KEY);
		kevent = libinput_event_get_keyboard_event(event);
		ck_assert_int_eq(libinput_event_keyboard_get_key(kevent),
				 KEY_A);
		ck_assert_int_eq(libinput_event_keyboard_get_key_state(kevent),
				 *state);
		libinput_event_destroy(event);
		libinput_dispatch(libinput);
	}

	litest_assert_empty_queue(libinput);
	litest_delete_device(dev);
	libinput_unref(libinput);
	libinput_unref(unused_libinput);
}
END_TEST

int
main(int argc, char **argv)
{
	litest_add_no_device("keyboard:seat key count", keyboard_seat_key_count);
	litest_add_no_device("keyboard:key counting", keyboard_ignore_no_pressed_release);

	return litest_run(argc, argv);
}
