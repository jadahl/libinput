/*
 * Copyright © 2015 Red Hat, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include "litest.h"
#include "litest-int.h"

static void litest_mouse_setup(void)
{
	struct litest_device *d = litest_create_device(LITEST_MOUSE_LOW_DPI);
	litest_set_current_device(d);
}

static struct input_id input_id = {
	.bustype = 0x3,
	.vendor = 0x1,
	.product = 0x1,
};

static int events[] = {
	EV_KEY, BTN_LEFT,
	EV_KEY, BTN_RIGHT,
	EV_KEY, BTN_MIDDLE,
	EV_REL, REL_X,
	EV_REL, REL_Y,
	EV_REL, REL_WHEEL,
	-1 , -1,
};

static const char udev_rule[] =
"ACTION==\"remove\", GOTO=\"touchpad_end\"\n"
"KERNEL!=\"event*\", GOTO=\"touchpad_end\"\n"
"ENV{ID_INPUT_TOUCHPAD}==\"\", GOTO=\"touchpad_end\"\n"
"\n"
"ATTRS{name}==\"litest Low DPI Mouse*\",\\\n"
"    ENV{MOUSE_DPI}=\"400@125\"\n"
"\n"
"LABEL=\"touchpad_end\"";

struct litest_test_device litest_mouse_low_dpi_device = {
	.type = LITEST_MOUSE_LOW_DPI,
	.features = LITEST_RELATIVE | LITEST_BUTTON | LITEST_WHEEL,
	.shortname = "low-dpi mouse",
	.setup = litest_mouse_setup,
	.interface = NULL,

	.name = "Low DPI Mouse",
	.id = &input_id,
	.absinfo = NULL,
	.events = events,
	.udev_rule = udev_rule,
};
