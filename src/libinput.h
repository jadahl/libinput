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

#ifndef LIBINPUT_H
#define LIBINPUT_H

#include <stdlib.h>
#include <stdint.h>

typedef int32_t li_fixed_t;

enum libinput_seat_capability {
	LIBINPUT_SEAT_CAP_KEYBOARD = 0,
	LIBINPUT_SEAT_CAP_POINTER = 1,
	LIBINPUT_SEAT_CAP_TOUCH = 2,
};

enum libinput_keyboard_key_state {
	LIBINPUT_KEYBOARD_KEY_STATE_RELEASED = 0,
	LIBINPUT_KEYBOARD_KEY_STATE_PRESSED = 1,
};

enum libinput_led {
	LIBINPUT_LED_NUM_LOCK = (1 << 0),
	LIBINPUT_LED_CAPS_LOCK = (1 << 1),
	LIBINPUT_LED_SCROLL_LOCK = (1 << 2),
};

enum libinput_pointer_button_state {
	LIBINPUT_POINTER_BUTTON_STATE_RELEASED = 0,
	LIBINPUT_POINTER_BUTTON_STATE_PRESSED = 1,
};

enum libinput_pointer_axis {
	LIBINPUT_POINTER_AXIS_VERTICAL_SCROLL = 0,
	LIBINPUT_POINTER_AXIS_HORIZONTAL_SCROLL = 1,
};

enum libinput_touch_type {
	LIBINPUT_TOUCH_TYPE_DOWN = 0,
	LIBINPUT_TOUCH_TYPE_UP = 1,
	LIBINPUT_TOUCH_TYPE_MOTION = 2,
	LIBINPUT_TOUCH_TYPE_FRAME = 3,
	LIBINPUT_TOUCH_TYPE_CANCEL = 4,
};

struct libinput_fd_handle;

typedef void (*libinput_fd_callback)(int fd, void *data);

struct libinput_device_interface {
	/* */
	void (*register_capability)(enum libinput_seat_capability capability,
				    void *data);
	void (*unregister_capability)(enum libinput_seat_capability capability,
				      void *data);

	/* */
	void (*get_current_screen_dimensions)(int *width,
					      int *height,
					      void *data);

	/* */
	struct libinput_fd_handle * (*add_fd)(int fd,
					      libinput_fd_callback callback,
					      void *data);
	void (*remove_fd)(struct libinput_fd_handle *fd_container,
			  void *data);

	/* */
	void (*device_lost)(void *data);
};

struct libinput_keyboard_listener {
	void (*notify_key)(uint32_t time,
			   uint32_t key,
			   enum libinput_keyboard_key_state state,
			   void *data);
};

struct libinput_pointer_listener {
	void (*notify_motion)(uint32_t time,
			      li_fixed_t dx,
			      li_fixed_t dy,
			      void *data);
	void (*notify_motion_absolute)(uint32_t time,
				       li_fixed_t x,
				       li_fixed_t y,
				       void *data);
	void (*notify_button)(uint32_t time,
			      int32_t button,
			      enum libinput_pointer_button_state state,
			      void *data);
	void (*notify_axis)(uint32_t time,
			    enum libinput_pointer_axis axis,
			    li_fixed_t value,
			    void *data);
};

struct libinput_touch_listener {
	void (*notify_touch)(uint32_t time,
			     int32_t slot,
			     li_fixed_t x,
			     li_fixed_t y,
			     enum libinput_touch_type touch_type,
			     void *data);
};

struct libinput_seat;
struct libinput_device;

struct libinput_device *
libinput_device_create_evdev(const char *devnode,
			     int fd,
			     const struct libinput_device_interface *interface,
			     void *user_data);

void
libinput_device_set_keyboard_listener(
	struct libinput_device *device,
	const struct libinput_keyboard_listener *listener,
	void *data);

void
libinput_device_set_pointer_listener(
	struct libinput_device *device,
	const struct libinput_pointer_listener *listener,
	void *data);

void
libinput_device_set_touch_listener(
	struct libinput_device *device,
	const struct libinput_touch_listener *listener,
	void *data);

int
libinput_device_dispatch(struct libinput_device *device);

void
libinput_device_destroy(struct libinput_device *device);

void
libinput_device_led_update(struct libinput_device *device,
			   enum libinput_led leds);

int
libinput_device_get_keys(struct libinput_device *device,
			 char *keys, size_t size);

void
libinput_device_calibrate(struct libinput_device *device,
			  float calibration[6]);

#endif /* LIBINPUT_H */
