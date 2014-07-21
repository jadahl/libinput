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

static void
litest_synaptics_touchpad_setup(void)
{
	struct litest_device *d = litest_create_device(LITEST_SYNAPTICS_TOUCHPAD);
	litest_set_current_device(d);
}

static struct input_event down[] = {
	{ .type = EV_ABS, .code = ABS_X, .value = LITEST_AUTO_ASSIGN },
	{ .type = EV_ABS, .code = ABS_Y, .value = LITEST_AUTO_ASSIGN },
	{ .type = EV_ABS, .code = ABS_PRESSURE, .value = 30  },
	{ .type = EV_ABS, .code = ABS_TOOL_WIDTH, .value = 7  },
	{ .type = EV_SYN, .code = SYN_REPORT, .value = 0 },
	{ .type = -1, .code = -1 },
};

static struct input_event move[] = {
	{ .type = EV_ABS, .code = ABS_X, .value = LITEST_AUTO_ASSIGN },
	{ .type = EV_ABS, .code = ABS_Y, .value = LITEST_AUTO_ASSIGN },
	{ .type = EV_SYN, .code = SYN_REPORT, .value = 0 },
	{ .type = -1, .code = -1 },
};

struct input_event up[] = {
	{ .type = EV_SYN, .code = SYN_REPORT, .value = 0 },
	{ .type = -1, .code = -1 },
};

static struct litest_device_interface interface = {
	.touch_down_events = down,
	.touch_move_events = move,
	.touch_up_events = up,
};

static struct input_absinfo absinfo[] = {
	{ ABS_X, 1472, 5472, 0, 0, 75 },
	{ ABS_Y, 1408, 4448, 0, 0, 129 },
	{ ABS_PRESSURE, 0, 255, 0, 0, 0 },
	{ ABS_TOOL_WIDTH, 0, 15, 0, 0, 0 },
	{ .value = -1 },
};

static struct input_id input_id = {
	.bustype = 0x11,
	.vendor = 0x2,
	.product = 0x7,
};

static int events[] = {
	EV_KEY, BTN_LEFT,
	EV_KEY, BTN_RIGHT,
	EV_KEY, BTN_TOOL_FINGER,
	EV_KEY, BTN_TOUCH,
	-1, -1,
};

struct litest_test_device litest_synaptics_touchpad_device = {
	.type = LITEST_SYNAPTICS_TOUCHPAD,
	.features = LITEST_TOUCHPAD | LITEST_BUTTON | LITEST_SINGLE_TOUCH,
	.shortname = "synaptics ST",
	.setup = litest_synaptics_touchpad_setup,
	.interface = &interface,

	.name = "SynPS/2 Synaptics TouchPad",
	.id = &input_id,
	.events = events,
	.absinfo = absinfo,
};
