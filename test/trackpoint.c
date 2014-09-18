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

static void
test_2fg_scroll(struct litest_device *dev, double dx, double dy)
{
	struct libinput *li = dev->libinput;

	litest_button_click(dev, BTN_MIDDLE, 1);

	libinput_dispatch(li);
	msleep(300);
	libinput_dispatch(li);

	litest_event(dev, EV_REL, REL_X, dx);
	litest_event(dev, EV_REL, REL_Y, dy);
	litest_event(dev, EV_SYN, SYN_REPORT, 0);

	litest_button_click(dev, BTN_MIDDLE, 0);

	libinput_dispatch(li);
}

START_TEST(trackpoint_scroll)
{
	struct litest_device *dev = litest_current_device();
	struct libinput *li = dev->libinput;

	litest_drain_events(li);

	test_2fg_scroll(dev, 1, 6);
	litest_assert_scroll(li, LIBINPUT_POINTER_AXIS_SCROLL_VERTICAL, 6);
	test_2fg_scroll(dev, 1, -7);
	litest_assert_scroll(li, LIBINPUT_POINTER_AXIS_SCROLL_VERTICAL, -7);
	test_2fg_scroll(dev, 8, 1);
	litest_assert_scroll(li, LIBINPUT_POINTER_AXIS_SCROLL_HORIZONTAL, 8);
	test_2fg_scroll(dev, -9, 1);
	litest_assert_scroll(li, LIBINPUT_POINTER_AXIS_SCROLL_HORIZONTAL, -9);

	/* scroll smaller than the threshold should not generate events */
	test_2fg_scroll(dev, 1, 1);
	/* long middle press without movement should not generate events */
	test_2fg_scroll(dev, 0, 0);

	litest_assert_empty_queue(li);
}
END_TEST

int main(int argc, char **argv) {

	litest_add("trackpoint:middlebutton", trackpoint_middlebutton, LITEST_POINTINGSTICK, LITEST_ANY);
	litest_add("trackpoint:scroll", trackpoint_scroll, LITEST_POINTINGSTICK, LITEST_ANY);

	return litest_run(argc, argv);
}
