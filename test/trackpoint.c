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

#include "libinput-util.h"
#include "litest.h"

START_TEST(trackpoint_middlebutton)
{
	struct litest_device *dev = litest_current_device();
	struct libinput *li = dev->libinput;

	litest_drain_events(li);

	/* A quick middle button click should get reported normally */
	litest_button_click(dev, BTN_MIDDLE, 1);
	litest_button_click(dev, BTN_MIDDLE, 0);

	litest_assert_button_event(li, BTN_MIDDLE, 1);
	litest_assert_button_event(li, BTN_MIDDLE, 0);

	litest_assert_empty_queue(li);
}
END_TEST

START_TEST(trackpoint_scroll)
{
	struct litest_device *dev = litest_current_device();
	struct libinput *li = dev->libinput;

	litest_drain_events(li);

	litest_button_scroll(dev, BTN_MIDDLE, 1, 6);
	litest_assert_scroll(li, LIBINPUT_POINTER_AXIS_SCROLL_VERTICAL, 6);
	litest_button_scroll(dev, BTN_MIDDLE, 1, -7);
	litest_assert_scroll(li, LIBINPUT_POINTER_AXIS_SCROLL_VERTICAL, -7);
	litest_button_scroll(dev, BTN_MIDDLE, 8, 1);
	litest_assert_scroll(li, LIBINPUT_POINTER_AXIS_SCROLL_HORIZONTAL, 8);
	litest_button_scroll(dev, BTN_MIDDLE, -9, 1);
	litest_assert_scroll(li, LIBINPUT_POINTER_AXIS_SCROLL_HORIZONTAL, -9);

	/* scroll smaller than the threshold should not generate events */
	litest_button_scroll(dev, BTN_MIDDLE, 1, 1);
	/* long middle press without movement should not generate events */
	litest_button_scroll(dev, BTN_MIDDLE, 0, 0);

	litest_assert_empty_queue(li);
}
END_TEST

START_TEST(trackpoint_middlebutton_noscroll)
{
	struct litest_device *dev = litest_current_device();
	struct libinput *li = dev->libinput;
	struct libinput_event *event;

	/* Disable middle button scrolling */
	libinput_device_config_scroll_set_method(dev->libinput_device,
					LIBINPUT_CONFIG_SCROLL_NO_SCROLL);

	litest_drain_events(li);

	/* A long middle button click + motion should get reported normally now */
	litest_button_scroll(dev, BTN_MIDDLE, 0, 10);

	litest_assert_button_event(li, BTN_MIDDLE, 1);

	event = libinput_get_event(li);
	ck_assert(event != NULL);
	ck_assert_int_eq(libinput_event_get_type(event), LIBINPUT_EVENT_POINTER_MOTION);
	libinput_event_destroy(event);

	litest_assert_button_event(li, BTN_MIDDLE, 0);

	litest_assert_empty_queue(li);

	/* Restore default scroll behavior */
	libinput_device_config_scroll_set_method(dev->libinput_device,
		libinput_device_config_scroll_get_default_method(
			dev->libinput_device));
}
END_TEST

START_TEST(trackpoint_scroll_source)
{
	struct litest_device *dev = litest_current_device();
	struct libinput *li = dev->libinput;
	struct libinput_event *event;
	struct libinput_event_pointer *ptrev;

	litest_drain_events(li);

	litest_button_scroll(dev, BTN_MIDDLE, 0, 6);
	litest_wait_for_event_of_type(li, LIBINPUT_EVENT_POINTER_AXIS, -1);

	while ((event = libinput_get_event(li))) {
		ptrev = libinput_event_get_pointer_event(event);

		ck_assert_int_eq(libinput_event_pointer_get_axis_source(ptrev),
				 LIBINPUT_POINTER_AXIS_SOURCE_CONTINUOUS);

		libinput_event_destroy(event);
	}
}
END_TEST

int main(int argc, char **argv) {

	litest_add("trackpoint:middlebutton", trackpoint_middlebutton, LITEST_POINTINGSTICK, LITEST_ANY);
	litest_add("trackpoint:middlebutton", trackpoint_middlebutton_noscroll, LITEST_POINTINGSTICK, LITEST_ANY);
	litest_add("trackpoint:scroll", trackpoint_scroll, LITEST_POINTINGSTICK, LITEST_ANY);
	litest_add("trackpoint:scroll", trackpoint_scroll_source, LITEST_POINTINGSTICK, LITEST_ANY);

	return litest_run(argc, argv);
}
