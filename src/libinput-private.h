/*
 * Copyright © 2013 Jonas Ådahl
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

#ifndef LIBINPUT_PRIVATE_H
#define LIBINPUT_PRIVATE_H

#include "libinput.h"

struct libinput_device {
	const struct libinput_device_interface *device_interface;
	void *device_interface_data;

	const struct libinput_keyboard_listener *keyboard_listener;
	void *keyboard_listener_data;

	const struct libinput_pointer_listener *pointer_listener;
	void *pointer_listener_data;

	const struct libinput_touch_listener *touch_listener;
	void *touch_listener_data;
};

void
keyboard_notify_key(struct libinput_device *device,
		    uint32_t time,
		    uint32_t key,
		    enum libinput_keyboard_key_state state);

void
pointer_notify_motion(struct libinput_device *device,
		      uint32_t time,
		      li_fixed_t dx,
		      li_fixed_t dy);

void
pointer_notify_motion_absolute(struct libinput_device *device,
			       uint32_t time,
			       li_fixed_t x,
			       li_fixed_t y);

void
pointer_notify_button(struct libinput_device *device,
		      uint32_t time,
		      int32_t button,
		      enum libinput_pointer_button_state state);

void
pointer_notify_axis(struct libinput_device *device,
		    uint32_t time,
		    enum libinput_pointer_axis axis,
		    li_fixed_t value);

void
touch_notify_touch(struct libinput_device *device,
		   uint32_t time,
		   int32_t slot,
		   li_fixed_t x,
		   li_fixed_t y,
		   enum libinput_touch_type touch_type);

static inline li_fixed_t li_fixed_from_int(int i)
{
	return i * 256;
}

static inline li_fixed_t
li_fixed_from_double(double d)
{
	union {
		double d;
		int64_t i;
	} u;

	u.d = d + (3LL << (51 - 8));

	return u.i;
}

#define ARRAY_LENGTH(a) (sizeof (a) / sizeof (a)[0])

#define LIBINPUT_EXPORT __attribute__ ((visibility("default")))

#endif /* LIBINPUT_PRIVATE_H */
