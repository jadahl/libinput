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
#include "libinput-util.h"

struct libinput {
	int epoll_fd;
	struct list source_destroy_list;

	struct libinput_event **events;
	size_t events_count;
	size_t events_len;
	size_t events_in;
	size_t events_out;
};

struct libinput_device {
	struct libinput *libinput;
	const struct libinput_device_interface *device_interface;
	void *device_interface_data;
	int terminated;
};

typedef void (*libinput_source_dispatch_t)(void *data);

struct libinput_source;

struct libinput_source *
libinput_add_fd(struct libinput *libinput,
		int fd,
		libinput_source_dispatch_t dispatch,
		void *data);

void
libinput_remove_source(struct libinput *libinput,
		       struct libinput_source *source);

void
libinput_post_event(struct libinput *libinput,
		    struct libinput_event *event);

void
device_register_capability(struct libinput_device *device,
			   enum libinput_device_capability capability);

void
device_unregister_capability(struct libinput_device *device,
			     enum libinput_device_capability capability);

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

#endif /* LIBINPUT_PRIVATE_H */
