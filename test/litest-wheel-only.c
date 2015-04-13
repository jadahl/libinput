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
