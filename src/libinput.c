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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libinput.h"
#include "evdev.h"
#include "libinput-private.h"

static void
post_event(struct libinput_device *device,
	   enum libinput_event_type type,
	   struct libinput_event *event);

void
keyboard_notify_key(struct libinput_device *device,
		    uint32_t time,
		    uint32_t key,
		    enum libinput_keyboard_key_state state)
{
	struct libinput_event_keyboard_key *key_event;

	key_event = malloc(sizeof *key_event);
	if (!key_event)
		return;

	*key_event = (struct libinput_event_keyboard_key) {
		.time = time,
		.key = key,
		.state = state,
	};

	post_event(device, LIBINPUT_EVENT_KEYBOARD_KEY, &key_event->base);
}

void
pointer_notify_motion(struct libinput_device *device,
		      uint32_t time,
		      li_fixed_t dx,
		      li_fixed_t dy)
{
	struct libinput_event_pointer_motion *motion_event;

	motion_event = malloc(sizeof *motion_event);
	if (!motion_event)
		return;

	*motion_event = (struct libinput_event_pointer_motion) {
		.time = time,
		.dx = dx,
		.dy = dy,
	};

	post_event(device, LIBINPUT_EVENT_POINTER_MOTION, &motion_event->base);
}

void
pointer_notify_motion_absolute(struct libinput_device *device,
			       uint32_t time,
			       li_fixed_t x,
			       li_fixed_t y)
{
	struct libinput_event_pointer_motion_absolute *motion_absolute_event;

	motion_absolute_event = malloc(sizeof *motion_absolute_event);
	if (!motion_absolute_event)
		return;

	*motion_absolute_event = (struct libinput_event_pointer_motion_absolute) {
		.time = time,
		.x = x,
		.y = y,
	};

	post_event(device,
		   LIBINPUT_EVENT_POINTER_MOTION_ABSOLUTE,
		   &motion_absolute_event->base);
}

void
pointer_notify_button(struct libinput_device *device,
		      uint32_t time,
		      int32_t button,
		      enum libinput_pointer_button_state state)
{
	struct libinput_event_pointer_button *button_event;

	button_event = malloc(sizeof *button_event);
	if (!button_event)
		return;

	*button_event = (struct libinput_event_pointer_button) {
		.time = time,
		.button = button,
		.state = state,
	};

	post_event(device, LIBINPUT_EVENT_POINTER_BUTTON, &button_event->base);
}

void
pointer_notify_axis(struct libinput_device *device,
		    uint32_t time,
		    enum libinput_pointer_axis axis,
		    li_fixed_t value)
{
	struct libinput_event_pointer_axis *axis_event;

	axis_event = malloc(sizeof *axis_event);
	if (!axis_event)
		return;

	*axis_event = (struct libinput_event_pointer_axis) {
		.time = time,
		.axis = axis,
		.value = value,
	};

	post_event(device, LIBINPUT_EVENT_POINTER_AXIS, &axis_event->base);
}

void
touch_notify_touch(struct libinput_device *device,
		   uint32_t time,
		   int32_t slot,
		   li_fixed_t x,
		   li_fixed_t y,
		   enum libinput_touch_type touch_type)
{
	struct libinput_event_touch_touch *touch_event;

	touch_event = malloc(sizeof *touch_event);
	if (!touch_event)
		return;

	*touch_event = (struct libinput_event_touch_touch) {
		.time = time,
		.slot = slot,
		.x = x,
		.y = y,
		.touch_type = touch_type,
	};

	post_event(device, LIBINPUT_EVENT_TOUCH_TOUCH, &touch_event->base);
}

static void
init_event_base(struct libinput_event *event, enum libinput_event_type type)
{
	event->type = type;
}

static void
post_event(struct libinput_device *device,
	   enum libinput_event_type type,
	   struct libinput_event *event)
{
	struct libinput_event **events = device->events;
	size_t events_len = device->events_len;
	size_t events_count = device->events_count;
	size_t move_len;
	size_t new_out;

	events_count++;
	if (events_count > events_len) {
		if (events_len == 0)
			events_len = 4;
		else
			events_len *= 2;
		events = realloc(events, events_len * sizeof *events);
		if (!events) {
			fprintf(stderr, "Failed to reallocate event ring "
				"buffer");
			return;
		}

		if (device->events_count > 0 && device->events_in == 0) {
			device->events_in = device->events_len;
		} else if (device->events_count > 0 &&
			   device->events_out >= device->events_in) {
			move_len = device->events_len - device->events_out;
			new_out = events_len - move_len;
			memmove(events + new_out,
				device->events + device->events_out,
				move_len * sizeof *events);
			device->events_out = new_out;
		}

		device->events = events;
		device->events_len = events_len;
	}

	init_event_base(event, type);

	device->events_count = events_count;
	events[device->events_in] = event;
	device->events_in = (device->events_in + 1) % device->events_len;
}

LIBINPUT_EXPORT struct libinput_event *
libinput_device_get_event(struct libinput_device *device)
{
	struct libinput_event *event;

	if (device->events_count == 0)
		return NULL;

	event = device->events[device->events_out];
	device->events_out = (device->events_out + 1) % device->events_len;
	device->events_count--;

	return event;
}

LIBINPUT_EXPORT int
libinput_device_dispatch(struct libinput_device *device)
{
	return evdev_device_dispatch((struct evdev_device *) device);
}

LIBINPUT_EXPORT void
libinput_device_destroy(struct libinput_device *device)
{
	struct libinput_event *event;

	while ((event = libinput_device_get_event(device)))
	       free(event);
	free(device->events);

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
