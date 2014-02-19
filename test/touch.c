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
#include <unistd.h>

#include "libinput-util.h"
#include "litest.h"

START_TEST(touch_frame_events)
{
	struct litest_device *dev = litest_current_device();
	struct libinput *li = dev->libinput;
	struct libinput_event *event;
	int have_frame_event = 0;

	litest_drain_events(dev->libinput);

	litest_touch_down(dev, 0, 10, 10);
	libinput_dispatch(li);

	while ((event = libinput_get_event(li))) {
		if (libinput_event_get_type(event) == LIBINPUT_EVENT_TOUCH_FRAME)
			have_frame_event++;
		libinput_event_destroy(event);
	}
	ck_assert_int_eq(have_frame_event, 1);

	litest_touch_down(dev, 1, 10, 10);
	libinput_dispatch(li);

	while ((event = libinput_get_event(li))) {
		if (libinput_event_get_type(event) == LIBINPUT_EVENT_TOUCH_FRAME)
			have_frame_event++;
		libinput_event_destroy(event);
	}
	ck_assert_int_eq(have_frame_event, 2);
}
END_TEST

START_TEST(touch_abs_transform)
{
	struct litest_device *dev = litest_current_device();
	struct libinput *libinput = dev->libinput;
	struct libinput_event *ev;
	struct libinput_event_touch *tev;
	li_fixed_t fx, fy;
	bool tested = false;

	litest_touch_down(dev, 0, 100, 100);

	libinput_dispatch(libinput);

	while ((ev = libinput_get_event(libinput))) {
		if (libinput_event_get_type(ev) != LIBINPUT_EVENT_TOUCH_DOWN)
			continue;

		tev = libinput_event_get_touch_event(ev);
		fx = libinput_event_touch_get_x_transformed(tev, 1920);
		ck_assert_int_eq(li_fixed_to_int(fx), 1919);
		fy = libinput_event_touch_get_y_transformed(tev, 720);
		ck_assert_int_eq(li_fixed_to_int(fy), 719);

		tested = true;
	}

	ck_assert(tested);
}
END_TEST


int
main(int argc, char **argv)
{
	litest_add("touch:frame", touch_frame_events, LITEST_TOUCH, LITEST_ANY);
	litest_add("touch:abs-transform", touch_abs_transform,
		   LITEST_TOUCH, LITEST_ANY);

	return litest_run(argc, argv);
}
