/*
 * Copyright © 2015 Red Hat, Inc.
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

#include <assert.h>

#include "libinput-util.h"

#include "litest.h"
#include "litest-int.h"

static void
atmel_hover_create(struct litest_device *d);

static void
litest_atmel_hover_setup(void)
{
	struct litest_device *d = litest_create_device(LITEST_ATMEL_HOVER);
	litest_set_current_device(d);
}

static struct input_event down[] = {
	{ .type = EV_ABS, .code = ABS_X, .value = LITEST_AUTO_ASSIGN  },
	{ .type = EV_ABS, .code = ABS_Y, .value = LITEST_AUTO_ASSIGN },
	{ .type = EV_ABS, .code = ABS_PRESSURE, .value = LITEST_AUTO_ASSIGN  },
	{ .type = EV_ABS, .code = ABS_MT_SLOT, .value = LITEST_AUTO_ASSIGN },
	{ .type = EV_ABS, .code = ABS_MT_TRACKING_ID, .value = LITEST_AUTO_ASSIGN },
	{ .type = EV_ABS, .code = ABS_MT_POSITION_X, .value = LITEST_AUTO_ASSIGN },
	{ .type = EV_ABS, .code = ABS_MT_POSITION_Y, .value = LITEST_AUTO_ASSIGN },
	{ .type = EV_ABS, .code = ABS_MT_DISTANCE, .value = LITEST_AUTO_ASSIGN },
	{ .type = EV_ABS, .code = ABS_MT_PRESSURE, .value = LITEST_AUTO_ASSIGN  },
	{ .type = EV_SYN, .code = SYN_REPORT, .value = 0 },
	{ .type = -1, .code = -1 },
};

static struct input_event move[] = {
	{ .type = EV_ABS, .code = ABS_MT_SLOT, .value = LITEST_AUTO_ASSIGN },
	{ .type = EV_ABS, .code = ABS_X, .value = LITEST_AUTO_ASSIGN },
	{ .type = EV_ABS, .code = ABS_Y, .value = LITEST_AUTO_ASSIGN },
	{ .type = EV_ABS, .code = ABS_PRESSURE, .value = LITEST_AUTO_ASSIGN  },
	{ .type = EV_ABS, .code = ABS_MT_POSITION_X, .value = LITEST_AUTO_ASSIGN },
	{ .type = EV_ABS, .code = ABS_MT_POSITION_Y, .value = LITEST_AUTO_ASSIGN },
	{ .type = EV_ABS, .code = ABS_MT_DISTANCE, .value = LITEST_AUTO_ASSIGN },
	{ .type = EV_ABS, .code = ABS_MT_PRESSURE, .value = LITEST_AUTO_ASSIGN  },
	{ .type = EV_SYN, .code = SYN_REPORT, .value = 0 },
	{ .type = -1, .code = -1 },
};

static struct input_event up[] = {
	{ .type = EV_ABS, .code = ABS_MT_SLOT, .value = LITEST_AUTO_ASSIGN },
	{ .type = EV_ABS, .code = ABS_MT_TRACKING_ID, .value = -1 },
	{ .type = EV_ABS, .code = ABS_MT_DISTANCE, .value = 1 },
	{ .type = EV_ABS, .code = ABS_MT_PRESSURE, .value = 0  },
	{ .type = EV_SYN, .code = SYN_REPORT, .value = 0 },
	{ .type = -1, .code = -1 },
};

static struct litest_device_interface interface = {
	.touch_down_events = down,
	.touch_move_events = move,
	.touch_up_events = up,
};

static struct input_id input_id = {
	.bustype = 0x18,
	.vendor = 0x0,
	.product = 0x0,
};

static int events[] = {
	EV_KEY, BTN_LEFT,
	EV_KEY, BTN_TOOL_FINGER,
	EV_KEY, BTN_TOUCH,
	EV_KEY, BTN_TOOL_DOUBLETAP,
	EV_KEY, BTN_TOOL_TRIPLETAP,
	EV_KEY, BTN_TOOL_QUADTAP,
	EV_KEY, BTN_TOOL_QUINTTAP,
	INPUT_PROP_MAX, INPUT_PROP_POINTER,
	INPUT_PROP_MAX, INPUT_PROP_BUTTONPAD,
	-1, -1,
};

static struct input_absinfo absinfo[] = {
	{ ABS_X, 0, 960, 0, 0, 10 },
	{ ABS_Y, 0, 540, 0, 0, 10 },
	{ ABS_PRESSURE, 0, 255, 0, 0, 0 },
	{ ABS_MT_SLOT, 0, 9, 0, 0, 0 },
	{ ABS_MT_TOUCH_MAJOR, 0, 255, 0, 0, 0 },
	{ ABS_MT_ORIENTATION, 0, 255, 0, 0, 0 },
	{ ABS_MT_POSITION_X, 0, 960, 0, 0, 10 },
	{ ABS_MT_POSITION_Y, 0, 540, 0, 0, 10 },
	{ ABS_MT_TOOL_TYPE, 0, 2, 0, 0, 0 },
	{ ABS_MT_TRACKING_ID, 0, 65535, 0, 0, 0 },
	{ ABS_MT_PRESSURE, 0, 255, 0, 0, 0 },
	{ ABS_MT_DISTANCE, 0, 1, 0, 0, 0 },
	{ .value = -1 }
};

struct litest_test_device litest_atmel_hover_device = {
	.type = LITEST_ATMEL_HOVER,
	.features = LITEST_TOUCHPAD | LITEST_BUTTON | LITEST_CLICKPAD | LITEST_HOVER,
	.shortname = "atmel hover",
	.setup = litest_atmel_hover_setup,
	.interface = &interface,
	.create = atmel_hover_create,

	.name = "Atmel maXTouch Touchpad",
	.id = &input_id,
	.events = events,
	.absinfo = absinfo,
};

static void
atmel_hover_create(struct litest_device *d)
{
	struct litest_semi_mt *semi_mt = zalloc(sizeof(*semi_mt));
	assert(semi_mt);

	d->private = semi_mt;

	d->uinput = litest_create_uinput_device_from_description(
			litest_atmel_hover_device.name,
			litest_atmel_hover_device.id,
			absinfo,
			events);
	d->interface = &interface;
}
