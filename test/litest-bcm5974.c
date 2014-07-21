/*
 * Copyright © 2013 Red Hat, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting documentation, and
 * that the name of the copyright holders not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  The copyright holders make no representations
 * about the suitability of this software for any purpose.  It is provided "as
 * is" without express or implied warranty.
 *
 * THE COPYRIGHT HOLDERS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include "litest.h"
#include "litest-int.h"

static void litest_bcm5974_setup(void)
{
	struct litest_device *d = litest_create_device(LITEST_BCM5974);
	litest_set_current_device(d);
}

struct input_event down[] = {
	{ .type = EV_ABS, .code = ABS_X, .value = LITEST_AUTO_ASSIGN  },
	{ .type = EV_ABS, .code = ABS_Y, .value = LITEST_AUTO_ASSIGN },
	{ .type = EV_ABS, .code = ABS_PRESSURE, .value = 30  },
	{ .type = EV_ABS, .code = ABS_MT_SLOT, .value = LITEST_AUTO_ASSIGN },
	{ .type = EV_ABS, .code = ABS_MT_TRACKING_ID, .value = LITEST_AUTO_ASSIGN },
	{ .type = EV_ABS, .code = ABS_MT_POSITION_X, .value = LITEST_AUTO_ASSIGN },
	{ .type = EV_ABS, .code = ABS_MT_POSITION_Y, .value = LITEST_AUTO_ASSIGN },
	{ .type = EV_SYN, .code = SYN_REPORT, .value = 0 },
	{ .type = -1, .code = -1 },
};

static struct input_event move[] = {
	{ .type = EV_ABS, .code = ABS_MT_SLOT, .value = LITEST_AUTO_ASSIGN },
	{ .type = EV_ABS, .code = ABS_X, .value = LITEST_AUTO_ASSIGN },
	{ .type = EV_ABS, .code = ABS_Y, .value = LITEST_AUTO_ASSIGN },
	{ .type = EV_ABS, .code = ABS_MT_POSITION_X, .value = LITEST_AUTO_ASSIGN },
	{ .type = EV_ABS, .code = ABS_MT_POSITION_Y, .value = LITEST_AUTO_ASSIGN },
	{ .type = EV_SYN, .code = SYN_REPORT, .value = 0 },
	{ .type = -1, .code = -1 },
};

static struct litest_device_interface interface = {
	.touch_down_events = down,
	.touch_move_events = move,
};

static struct input_absinfo absinfo[] = {
	{ ABS_X, -4824, 4824, 0, 0, 0 },
	{ ABS_Y, -172, 4290, 0, 0, 0 },
	{ ABS_PRESSURE, 0, 256, 5, 0, 0 },
	{ ABS_TOOL_WIDTH, 0, 16, 0, 0, 0 },
	{ ABS_MT_SLOT, 0, 15, 0, 0, 0 },
	{ ABS_MT_POSITION_X, -4824, 4824, 17, 0, 0 },
	{ ABS_MT_POSITION_Y, -172, 4290, 17, 0, 0 },
	{ ABS_MT_ORIENTATION, -16384, 16384, 3276, 0, 0 },
	{ ABS_MT_TOUCH_MAJOR, 0, 2048, 81, 0, 0 },
	{ ABS_MT_TOUCH_MINOR, 0, 2048, 81, 0, 0 },
	{ ABS_MT_WIDTH_MAJOR, 0, 2048, 81, 0, 0 },
	{ ABS_MT_WIDTH_MINOR, 0, 2048, 81, 0, 0 },
	{ ABS_MT_TRACKING_ID, 0, 65535, 0, 0, 0 },
	{ ABS_MT_PRESSURE, 0, 255, 0, 0, 0 },
	{ .value = -1 },
};

static struct input_id input_id = {
	.bustype = 0x3,
	.vendor = 0x5ac,
	.product = 0x249,
};

static int events[] = {
	EV_KEY, BTN_LEFT,
	EV_KEY, BTN_TOOL_FINGER,
	EV_KEY, BTN_TOOL_QUINTTAP,
	EV_KEY, BTN_TOUCH,
	EV_KEY, BTN_TOOL_DOUBLETAP,
	EV_KEY, BTN_TOOL_TRIPLETAP,
	EV_KEY, BTN_TOOL_QUADTAP,
	INPUT_PROP_MAX, INPUT_PROP_BUTTONPAD,
	-1, -1
};

struct litest_test_device litest_bcm5974_device = {
	.type = LITEST_BCM5974,
	.features = LITEST_TOUCHPAD | LITEST_CLICKPAD |
		    LITEST_BUTTON | LITEST_APPLE_CLICKPAD,
	.shortname = "bcm5974",
	.setup = litest_bcm5974_setup,
	.interface = &interface,

	.name = "bcm5974",
	.id = &input_id,
	.events = events,
	.absinfo = absinfo,
};
