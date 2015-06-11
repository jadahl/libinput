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

static void litest_wheel_only_setup(void)
{
	struct litest_device *d = litest_create_device(LITEST_WHEEL_ONLY);
	litest_set_current_device(d);
}

static struct input_id input_id = {
	.bustype = 0x3,
	.vendor = 0x1,
	.product = 0x2,
};

static int events[] = {
	EV_REL, REL_WHEEL,
	-1 , -1,
};

static const char udev_rule[] =
"ACTION==\"remove\", GOTO=\"wheel_only_end\"\n"
"KERNEL!=\"event*\", GOTO=\"wheel_only_end\"\n"
"\n"
"ATTRS{name}==\"litest wheel only device*\",\\\n"
"    ENV{ID_INPUT_KEY}=\"1\"\n"
"\n"
"LABEL=\"wheel_only_end\"";

struct litest_test_device litest_wheel_only_device = {
	.type = LITEST_WHEEL_ONLY,
	.features = LITEST_WHEEL,
	.shortname = "wheel only",
	.setup = litest_wheel_only_setup,
	.interface = NULL,

	.name = "wheel only device",
	.id = &input_id,
	.absinfo = NULL,
	.events = events,
	.udev_rule = udev_rule,
};
