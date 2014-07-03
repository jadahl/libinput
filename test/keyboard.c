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

int
main(int argc, char **argv)
{
	litest_add_no_device("keyboard:seat key count", keyboard_seat_key_count);

	return litest_run(argc, argv);
}
