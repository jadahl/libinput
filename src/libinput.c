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

#include "config.h"

#include <stdlib.h>

#include "libinput.h"
#include "evdev.h"
#include "libinput-private.h"

void
keyboard_notify_key(struct libinput_device *device,
		    uint32_t time,
		    uint32_t key,
		    enum libinput_keyboard_key_state state)
{
	if (device->keyboard_listener)
		device->keyboard_listener->notify_key(
			time, key, state,
			device->keyboard_listener_data);
}

void
pointer_notify_motion(struct libinput_device *device,
		      uint32_t time,
		      li_fixed_t dx,
		      li_fixed_t dy)
{
	if (device->pointer_listener)
		device->pointer_listener->notify_motion(
			time, dx, dy,
			device->pointer_listener_data);
}

void
pointer_notify_motion_absolute(struct libinput_device *device,
			       uint32_t time,
			       li_fixed_t x,
			       li_fixed_t y)
{
	if (device->pointer_listener)
		device->pointer_listener->notify_motion_absolute(
			time, x, y,
			device->pointer_listener_data);
}

void
pointer_notify_button(struct libinput_device *device,
		      uint32_t time,
		      int32_t button,
		      enum libinput_pointer_button_state state)
{
	if (device->pointer_listener)
		device->pointer_listener->notify_button(
			time, button, state,
			device->pointer_listener_data);
}

void
pointer_notify_axis(struct libinput_device *device,
		    uint32_t time,
		    enum libinput_pointer_axis axis,
		    li_fixed_t value)
{
	if (device->pointer_listener)
		device->pointer_listener->notify_axis(
			time, axis, value,
			device->pointer_listener_data);
}

void
touch_notify_touch(struct libinput_device *device,
		   uint32_t time,
		   int32_t slot,
		   li_fixed_t x,
		   li_fixed_t y,
		   enum libinput_touch_type touch_type)
{
	if (device->touch_listener)
		device->touch_listener->notify_touch(
			time, slot, x, y, touch_type,
			device->touch_listener_data);
}

LIBINPUT_EXPORT void
libinput_device_set_keyboard_listener(
	struct libinput_device *device,
	const struct libinput_keyboard_listener *listener,
	void *data)
{
	device->keyboard_listener = listener;
	device->keyboard_listener_data = data;
}

LIBINPUT_EXPORT void
libinput_device_set_pointer_listener(
	struct libinput_device *device,
	const struct libinput_pointer_listener *listener,
	void *data)
{
	device->pointer_listener = listener;
	device->pointer_listener_data = data;
}

LIBINPUT_EXPORT void
libinput_device_set_touch_listener(
	struct libinput_device *device,
	const struct libinput_touch_listener *listener,
	void *data)
{
	device->touch_listener = listener;
	device->touch_listener_data = data;
}

LIBINPUT_EXPORT int
libinput_device_dispatch(struct libinput_device *device)
{
	return evdev_device_dispatch((struct evdev_device *) device);
}

LIBINPUT_EXPORT void
libinput_device_destroy(struct libinput_device *device)
{
	evdev_device_destroy((struct evdev_device *) device);
}

LIBINPUT_EXPORT void
libinput_device_led_update(struct libinput_device *device,
			   enum libinput_led leds)
{
	evdev_device_led_update((struct evdev_device *) device, leds);
}

LIBINPUT_EXPORT int
libinput_device_get_keys(struct libinput_device *device,
			 char *keys, size_t size)
{
	return evdev_device_get_keys((struct evdev_device *) device,
				     keys,
				     size);
}

LIBINPUT_EXPORT void
libinput_device_calibrate(struct libinput_device *device,
			  float calibration[6])
{
	evdev_device_calibrate((struct evdev_device *) device, calibration);
}
