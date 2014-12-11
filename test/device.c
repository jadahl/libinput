/*
 * Copyright © 2014 Red Hat, Inc.
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

#include <config.h>

#include <check.h>
#include <errno.h>
#include <fcntl.h>
#include <libinput.h>
#include <libudev.h>
#include <unistd.h>

#include "litest.h"
#include "libinput-util.h"

START_TEST(device_sendevents_config)
{
	struct litest_device *dev = litest_current_device();
	struct libinput_device *device;
	uint32_t modes;

	device = dev->libinput_device;

	modes = libinput_device_config_send_events_get_modes(device);
	ck_assert_int_eq(modes,
			 LIBINPUT_CONFIG_SEND_EVENTS_DISABLED);
}
END_TEST

START_TEST(device_sendevents_config_invalid)
{
	struct litest_device *dev = litest_current_device();
	struct libinput_device *device;
	enum libinput_config_status status;

	device = dev->libinput_device;

	status = libinput_device_config_send_events_set_mode(device,
			     LIBINPUT_CONFIG_SEND_EVENTS_DISABLED | (1 << 4));
	ck_assert_int_eq(status, LIBINPUT_CONFIG_STATUS_UNSUPPORTED);
}
END_TEST

START_TEST(device_sendevents_config_touchpad)
{
	struct litest_device *dev = litest_current_device();
	struct libinput_device *device;
	uint32_t modes;

	device = dev->libinput_device;

	modes = libinput_device_config_send_events_get_modes(device);
	ck_assert_int_eq(modes,
			 LIBINPUT_CONFIG_SEND_EVENTS_DISABLED_ON_EXTERNAL_MOUSE|
			 LIBINPUT_CONFIG_SEND_EVENTS_DISABLED);
}
END_TEST

START_TEST(device_sendevents_config_touchpad_superset)
{
	struct litest_device *dev = litest_current_device();
	struct libinput_device *device;
	enum libinput_config_status status;
	uint32_t modes;

	device = dev->libinput_device;

	modes = LIBINPUT_CONFIG_SEND_EVENTS_DISABLED |
		LIBINPUT_CONFIG_SEND_EVENTS_DISABLED_ON_EXTERNAL_MOUSE;

	status = libinput_device_config_send_events_set_mode(device,
							     modes);
	ck_assert_int_eq(status, LIBINPUT_CONFIG_STATUS_SUCCESS);

	/* DISABLED supersedes the rest, expect the rest to be dropped */
	modes = libinput_device_config_send_events_get_mode(device);
	ck_assert_int_eq(modes, LIBINPUT_CONFIG_SEND_EVENTS_DISABLED);
}
END_TEST

START_TEST(device_sendevents_config_default)
{
	struct litest_device *dev = litest_current_device();
	struct libinput_device *device;
	uint32_t mode;

	device = dev->libinput_device;

	mode = libinput_device_config_send_events_get_mode(device);
	ck_assert_int_eq(mode,
			 LIBINPUT_CONFIG_SEND_EVENTS_ENABLED);

	mode = libinput_device_config_send_events_get_default_mode(device);
	ck_assert_int_eq(mode,
			 LIBINPUT_CONFIG_SEND_EVENTS_ENABLED);
}
END_TEST

START_TEST(device_disable)
{
	struct litest_device *dev = litest_current_device();
	struct libinput *li = dev->libinput;
	struct libinput_device *device;
	enum libinput_config_status status;
	struct libinput_event *event;
	struct litest_device *tmp;

	device = dev->libinput_device;

	litest_drain_events(li);

	status = libinput_device_config_send_events_set_mode(device,
			LIBINPUT_CONFIG_SEND_EVENTS_DISABLED);
	ck_assert_int_eq(status, LIBINPUT_CONFIG_STATUS_SUCCESS);

	/* no event from disabling */
	litest_assert_empty_queue(li);

	/* no event from disabled device */
	litest_event(dev, EV_REL, REL_X, 10);
	litest_event(dev, EV_SYN, SYN_REPORT, 0);
	litest_assert_empty_queue(li);

	/* create a new device so the resumed fd isn't the same as the
	   suspended one */
	tmp = litest_add_device(li, LITEST_KEYBOARD);
	ck_assert_notnull(tmp);
	litest_drain_events(li);

	/* no event from resuming */
	status = libinput_device_config_send_events_set_mode(device,
			LIBINPUT_CONFIG_SEND_EVENTS_ENABLED);
	ck_assert_int_eq(status, LIBINPUT_CONFIG_STATUS_SUCCESS);
	litest_assert_empty_queue(li);

	/* event from renabled device */
	litest_event(dev, EV_REL, REL_X, 10);
	litest_event(dev, EV_SYN, SYN_REPORT, 0);

	libinput_dispatch(li);
	event = libinput_get_event(li);
	ck_assert_notnull(event);
	ck_assert_int_eq(libinput_event_get_type(event),
			 LIBINPUT_EVENT_POINTER_MOTION);
	libinput_event_destroy(event);

	litest_delete_device(tmp);
}
END_TEST

START_TEST(device_disable_touchpad)
{
	struct litest_device *dev = litest_current_device();
	struct libinput *li = dev->libinput;
	struct libinput_device *device;
	enum libinput_config_status status;

	device = dev->libinput_device;

	litest_drain_events(li);

	status = libinput_device_config_send_events_set_mode(device,
			LIBINPUT_CONFIG_SEND_EVENTS_DISABLED);
	ck_assert_int_eq(status, LIBINPUT_CONFIG_STATUS_SUCCESS);

	/* no event from disabling */
	litest_assert_empty_queue(li);

	litest_touch_down(dev, 0, 50, 50);
	litest_touch_move_to(dev, 0, 50, 50, 90, 90, 10, 0);
	litest_touch_up(dev, 0);


	litest_assert_empty_queue(li);

	/* no event from resuming */
	status = libinput_device_config_send_events_set_mode(device,
			LIBINPUT_CONFIG_SEND_EVENTS_ENABLED);
	ck_assert_int_eq(status, LIBINPUT_CONFIG_STATUS_SUCCESS);
	litest_assert_empty_queue(li);
}
END_TEST

START_TEST(device_disable_events_pending)
{
	struct litest_device *dev = litest_current_device();
	struct libinput *li = dev->libinput;
	struct libinput_device *device;
	enum libinput_config_status status;
	struct libinput_event *event;
	int i;

	device = dev->libinput_device;

	litest_drain_events(li);

	/* put a couple of events in the queue, enough to
	   feed the ptraccel trackers */
	for (i = 0; i < 10; i++) {
		litest_event(dev, EV_REL, REL_X, 10);
		litest_event(dev, EV_SYN, SYN_REPORT, 0);
	}
	libinput_dispatch(li);

	status = libinput_device_config_send_events_set_mode(device,
			LIBINPUT_CONFIG_SEND_EVENTS_DISABLED);
	ck_assert_int_eq(status, LIBINPUT_CONFIG_STATUS_SUCCESS);

	/* expect above events */
	litest_wait_for_event(li);
	while ((event = libinput_get_event(li)) != NULL) {
	       ck_assert_int_eq(libinput_event_get_type(event),
				LIBINPUT_EVENT_POINTER_MOTION);
	       libinput_event_destroy(event);
       }
}
END_TEST

START_TEST(device_double_disable)
{
	struct litest_device *dev = litest_current_device();
	struct libinput *li = dev->libinput;
	struct libinput_device *device;
	enum libinput_config_status status;

	device = dev->libinput_device;

	litest_drain_events(li);

	status = libinput_device_config_send_events_set_mode(device,
			LIBINPUT_CONFIG_SEND_EVENTS_DISABLED);
	ck_assert_int_eq(status, LIBINPUT_CONFIG_STATUS_SUCCESS);

	status = libinput_device_config_send_events_set_mode(device,
			LIBINPUT_CONFIG_SEND_EVENTS_DISABLED);
	ck_assert_int_eq(status, LIBINPUT_CONFIG_STATUS_SUCCESS);

	litest_assert_empty_queue(li);
}
END_TEST

START_TEST(device_double_enable)
{
	struct litest_device *dev = litest_current_device();
	struct libinput *li = dev->libinput;
	struct libinput_device *device;
	enum libinput_config_status status;

	device = dev->libinput_device;

	litest_drain_events(li);

	status = libinput_device_config_send_events_set_mode(device,
			LIBINPUT_CONFIG_SEND_EVENTS_ENABLED);
	ck_assert_int_eq(status, LIBINPUT_CONFIG_STATUS_SUCCESS);

	status = libinput_device_config_send_events_set_mode(device,
			LIBINPUT_CONFIG_SEND_EVENTS_ENABLED);
	ck_assert_int_eq(status, LIBINPUT_CONFIG_STATUS_SUCCESS);

	litest_assert_empty_queue(li);
}
END_TEST

START_TEST(device_reenable_syspath_changed)
{
	struct libinput *li;
	struct litest_device *litest_device;
	struct libinput_device *device1, *device2;
	enum libinput_config_status status;
	struct libinput_event *event;

	li = litest_create_context();
	litest_device = litest_add_device(li, LITEST_MOUSE);
	device1 = litest_device->libinput_device;

	libinput_device_ref(device1);
	status = libinput_device_config_send_events_set_mode(device1,
			LIBINPUT_CONFIG_SEND_EVENTS_DISABLED);
	ck_assert_int_eq(status, LIBINPUT_CONFIG_STATUS_SUCCESS);

	litest_drain_events(li);

	litest_delete_device(litest_device);
	litest_drain_events(li);

	litest_device = litest_add_device(li, LITEST_MOUSE);
	device2 = litest_device->libinput_device;
	ck_assert_str_eq(libinput_device_get_sysname(device1),
			 libinput_device_get_sysname(device2));

	status = libinput_device_config_send_events_set_mode(device1,
			LIBINPUT_CONFIG_SEND_EVENTS_ENABLED);
	ck_assert_int_eq(status, LIBINPUT_CONFIG_STATUS_SUCCESS);

	/* can't really check for much here, other than that if we pump
	   events through libinput, none of them should be from the first
	   device */
	litest_event(litest_device, EV_REL, REL_X, 1);
	litest_event(litest_device, EV_REL, REL_Y, 1);
	litest_event(litest_device, EV_SYN, SYN_REPORT, 0);

	libinput_dispatch(li);
	while ((event = libinput_get_event(li))) {
		ck_assert(libinput_event_get_device(event) != device1);
		libinput_event_destroy(event);
	}

	litest_delete_device(litest_device);
	libinput_device_unref(device1);
	libinput_unref(li);
}
END_TEST

START_TEST(device_reenable_device_removed)
{
	struct libinput *li;
	struct litest_device *litest_device;
	struct libinput_device *device;
	enum libinput_config_status status;

	li = litest_create_context();
	litest_device = litest_add_device(li, LITEST_MOUSE);
	device = litest_device->libinput_device;

	libinput_device_ref(device);
	status = libinput_device_config_send_events_set_mode(device,
			LIBINPUT_CONFIG_SEND_EVENTS_DISABLED);
	ck_assert_int_eq(status, LIBINPUT_CONFIG_STATUS_SUCCESS);

	litest_drain_events(li);

	litest_delete_device(litest_device);
	litest_drain_events(li);

	status = libinput_device_config_send_events_set_mode(device,
			LIBINPUT_CONFIG_SEND_EVENTS_ENABLED);
	ck_assert_int_eq(status, LIBINPUT_CONFIG_STATUS_SUCCESS);

	/* can't really check for much here, this really just exercises the
	   code path. */
	litest_assert_empty_queue(li);

	libinput_device_unref(device);
	libinput_unref(li);
}
END_TEST

START_TEST(device_disable_release_buttons)
{
	struct litest_device *dev = litest_current_device();
	struct libinput *li = dev->libinput;
	struct libinput_device *device;
	struct libinput_event *event;
	struct libinput_event_pointer *ptrevent;
	enum libinput_config_status status;

	device = dev->libinput_device;

	litest_button_click(dev, BTN_LEFT, true);
	litest_drain_events(li);
	litest_assert_empty_queue(li);

	status = libinput_device_config_send_events_set_mode(device,
			LIBINPUT_CONFIG_SEND_EVENTS_DISABLED);
	ck_assert_int_eq(status, LIBINPUT_CONFIG_STATUS_SUCCESS);

	litest_wait_for_event(li);
	event = libinput_get_event(li);

	ck_assert_int_eq(libinput_event_get_type(event),
			 LIBINPUT_EVENT_POINTER_BUTTON);
	ptrevent = libinput_event_get_pointer_event(event);
	ck_assert_int_eq(libinput_event_pointer_get_button(ptrevent),
			 BTN_LEFT);
	ck_assert_int_eq(libinput_event_pointer_get_button_state(ptrevent),
			 LIBINPUT_BUTTON_STATE_RELEASED);

	libinput_event_destroy(event);
	litest_assert_empty_queue(li);
}
END_TEST

START_TEST(device_disable_release_keys)
{
	struct litest_device *dev = litest_current_device();
	struct libinput *li = dev->libinput;
	struct libinput_device *device;
	struct libinput_event *event;
	struct libinput_event_keyboard *kbdevent;
	enum libinput_config_status status;

	device = dev->libinput_device;

	litest_button_click(dev, KEY_A, true);
	litest_drain_events(li);
	litest_assert_empty_queue(li);

	status = libinput_device_config_send_events_set_mode(device,
			LIBINPUT_CONFIG_SEND_EVENTS_DISABLED);
	ck_assert_int_eq(status, LIBINPUT_CONFIG_STATUS_SUCCESS);

	litest_wait_for_event(li);
	event = libinput_get_event(li);

	ck_assert_int_eq(libinput_event_get_type(event),
			 LIBINPUT_EVENT_KEYBOARD_KEY);
	kbdevent = libinput_event_get_keyboard_event(event);
	ck_assert_int_eq(libinput_event_keyboard_get_key(kbdevent),
			 KEY_A);
	ck_assert_int_eq(libinput_event_keyboard_get_key_state(kbdevent),
			 LIBINPUT_KEY_STATE_RELEASED);

	libinput_event_destroy(event);
	litest_assert_empty_queue(li);
}
END_TEST

START_TEST(device_disable_release_tap)
{
	struct litest_device *dev = litest_current_device();
	struct libinput *li = dev->libinput;
	struct libinput_device *device;
	enum libinput_config_status status;

	device = dev->libinput_device;

	libinput_device_config_tap_set_enabled(device,
					       LIBINPUT_CONFIG_TAP_ENABLED);

	litest_drain_events(li);

	litest_touch_down(dev, 0, 50, 50);
	litest_touch_up(dev, 0);

	libinput_dispatch(li);

	status = libinput_device_config_send_events_set_mode(device,
			LIBINPUT_CONFIG_SEND_EVENTS_DISABLED);
	ck_assert_int_eq(status, LIBINPUT_CONFIG_STATUS_SUCCESS);
	/* tap happened before suspending, so we still expect the event */

	litest_timeout_tap();

	litest_assert_button_event(li,
				   BTN_LEFT,
				   LIBINPUT_BUTTON_STATE_PRESSED);
	litest_assert_button_event(li,
				   BTN_LEFT,
				   LIBINPUT_BUTTON_STATE_RELEASED);

	litest_assert_empty_queue(li);

	/* resume, make sure we don't get anything */
	status = libinput_device_config_send_events_set_mode(device,
			LIBINPUT_CONFIG_SEND_EVENTS_ENABLED);
	ck_assert_int_eq(status, LIBINPUT_CONFIG_STATUS_SUCCESS);
	libinput_dispatch(li);
	litest_assert_empty_queue(li);

}
END_TEST

START_TEST(device_disable_release_tap_n_drag)
{
	struct litest_device *dev = litest_current_device();
	struct libinput *li = dev->libinput;
	struct libinput_device *device;
	enum libinput_config_status status;

	device = dev->libinput_device;

	libinput_device_config_tap_set_enabled(device,
					       LIBINPUT_CONFIG_TAP_ENABLED);

	litest_drain_events(li);

	litest_touch_down(dev, 0, 50, 50);
	litest_touch_up(dev, 0);
	litest_touch_down(dev, 0, 50, 50);
	libinput_dispatch(li);
	litest_timeout_tap();
	libinput_dispatch(li);

	status = libinput_device_config_send_events_set_mode(device,
			LIBINPUT_CONFIG_SEND_EVENTS_DISABLED);
	ck_assert_int_eq(status, LIBINPUT_CONFIG_STATUS_SUCCESS);

	libinput_dispatch(li);
	litest_touch_up(dev, 0);

	litest_assert_button_event(li,
				   BTN_LEFT,
				   LIBINPUT_BUTTON_STATE_PRESSED);
	litest_assert_button_event(li,
				   BTN_LEFT,
				   LIBINPUT_BUTTON_STATE_RELEASED);

	litest_assert_empty_queue(li);
}
END_TEST


START_TEST(device_disable_release_softbutton)
{
	struct litest_device *dev = litest_current_device();
	struct libinput *li = dev->libinput;
	struct libinput_device *device;
	enum libinput_config_status status;

	device = dev->libinput_device;

	litest_drain_events(li);

	litest_touch_down(dev, 0, 90, 90);
	litest_button_click(dev, BTN_LEFT, true);

	/* make sure softbutton works */
	litest_assert_button_event(li,
				   BTN_RIGHT,
				   LIBINPUT_BUTTON_STATE_PRESSED);
	/* disable */
	status = libinput_device_config_send_events_set_mode(device,
			LIBINPUT_CONFIG_SEND_EVENTS_DISABLED);
	ck_assert_int_eq(status, LIBINPUT_CONFIG_STATUS_SUCCESS);

	litest_assert_button_event(li,
				   BTN_RIGHT,
				   LIBINPUT_BUTTON_STATE_RELEASED);

	litest_assert_empty_queue(li);

	litest_button_click(dev, BTN_LEFT, false);
	litest_touch_up(dev, 0);

	litest_assert_empty_queue(li);

	/* resume, make sure we don't get anything */
	status = libinput_device_config_send_events_set_mode(device,
			LIBINPUT_CONFIG_SEND_EVENTS_ENABLED);
	ck_assert_int_eq(status, LIBINPUT_CONFIG_STATUS_SUCCESS);
	libinput_dispatch(li);
	litest_assert_empty_queue(li);

}
END_TEST

START_TEST(device_disable_topsoftbutton)
{
	struct litest_device *dev = litest_current_device();
	struct litest_device *trackpoint;
	struct libinput *li = dev->libinput;
	struct libinput_device *device;
	enum libinput_config_status status;

	struct libinput_event *event;
	struct libinput_event_pointer *ptrevent;

	device = dev->libinput_device;

	trackpoint = litest_add_device(li, LITEST_TRACKPOINT);

	status = libinput_device_config_send_events_set_mode(device,
			LIBINPUT_CONFIG_SEND_EVENTS_DISABLED);
	ck_assert_int_eq(status, LIBINPUT_CONFIG_STATUS_SUCCESS);
	litest_drain_events(li);

	litest_touch_down(dev, 0, 90, 10);
	litest_button_click(dev, BTN_LEFT, true);
	litest_button_click(dev, BTN_LEFT, false);
	litest_touch_up(dev, 0);

	litest_wait_for_event(li);
	event = libinput_get_event(li);
	ck_assert_int_eq(libinput_event_get_type(event),
			 LIBINPUT_EVENT_POINTER_BUTTON);
	ck_assert_ptr_eq(libinput_event_get_device(event),
			 trackpoint->libinput_device);
	ptrevent = libinput_event_get_pointer_event(event);
	ck_assert_int_eq(libinput_event_pointer_get_button(ptrevent),
			 BTN_RIGHT);
	ck_assert_int_eq(libinput_event_pointer_get_button_state(ptrevent),
			 LIBINPUT_BUTTON_STATE_PRESSED);
	libinput_event_destroy(event);

	event = libinput_get_event(li);
	ck_assert_int_eq(libinput_event_get_type(event),
			 LIBINPUT_EVENT_POINTER_BUTTON);
	ck_assert_ptr_eq(libinput_event_get_device(event),
			 trackpoint->libinput_device);
	ptrevent = libinput_event_get_pointer_event(event);
	ck_assert_int_eq(libinput_event_pointer_get_button(ptrevent),
			 BTN_RIGHT);
	ck_assert_int_eq(libinput_event_pointer_get_button_state(ptrevent),
			 LIBINPUT_BUTTON_STATE_RELEASED);
	libinput_event_destroy(event);

	litest_assert_empty_queue(li);

	litest_delete_device(trackpoint);
}
END_TEST

START_TEST(device_ids)
{
	struct litest_device *dev = litest_current_device();
	const char *name;
	unsigned int pid, vid;

	name = libevdev_get_name(dev->evdev);
	pid = libevdev_get_id_product(dev->evdev);
	vid = libevdev_get_id_vendor(dev->evdev);

	ck_assert_str_eq(name,
			 libinput_device_get_name(dev->libinput_device));
	ck_assert_int_eq(pid,
			 libinput_device_get_id_product(dev->libinput_device));
	ck_assert_int_eq(vid,
			 libinput_device_get_id_vendor(dev->libinput_device));
}
END_TEST

START_TEST(device_get_udev_handle)
{
	struct litest_device *dev = litest_current_device();
	struct udev_device *udev_device;

	udev_device = libinput_device_get_udev_device(dev->libinput_device);
	ck_assert_notnull(udev_device);
	udev_device_unref(udev_device);
}
END_TEST

START_TEST(device_context)
{
	struct litest_device *dev = litest_current_device();
	struct libinput_seat *seat;

	ck_assert(dev->libinput == libinput_device_get_context(dev->libinput_device));
	seat = libinput_device_get_seat(dev->libinput_device);
	ck_assert(dev->libinput == libinput_seat_get_context(seat));
}
END_TEST

int main (int argc, char **argv)
{
	litest_add("device:sendevents", device_sendevents_config, LITEST_ANY, LITEST_TOUCHPAD);
	litest_add("device:sendevents", device_sendevents_config_invalid, LITEST_ANY, LITEST_ANY);
	litest_add("device:sendevents", device_sendevents_config_touchpad, LITEST_TOUCHPAD, LITEST_ANY);
	litest_add("device:sendevents", device_sendevents_config_touchpad_superset, LITEST_TOUCHPAD, LITEST_ANY);
	litest_add("device:sendevents", device_sendevents_config_default, LITEST_ANY, LITEST_ANY);
	litest_add("device:sendevents", device_disable, LITEST_RELATIVE, LITEST_ANY);
	litest_add("device:sendevents", device_disable_touchpad, LITEST_TOUCHPAD, LITEST_ANY);
	litest_add("device:sendevents", device_disable_events_pending, LITEST_RELATIVE, LITEST_TOUCHPAD);
	litest_add("device:sendevents", device_double_disable, LITEST_ANY, LITEST_ANY);
	litest_add("device:sendevents", device_double_enable, LITEST_ANY, LITEST_ANY);
	litest_add_no_device("device:sendevents", device_reenable_syspath_changed);
	litest_add_no_device("device:sendevents", device_reenable_device_removed);
	litest_add_for_device("device:sendevents", device_disable_release_buttons, LITEST_MOUSE);
	litest_add_for_device("device:sendevents", device_disable_release_keys, LITEST_KEYBOARD);
	litest_add("device:sendevents", device_disable_release_tap, LITEST_TOUCHPAD, LITEST_ANY);
	litest_add("device:sendevents", device_disable_release_tap_n_drag, LITEST_TOUCHPAD, LITEST_ANY);
	litest_add("device:sendevents", device_disable_release_softbutton, LITEST_CLICKPAD, LITEST_APPLE_CLICKPAD);
	litest_add("device:sendevents", device_disable_topsoftbutton, LITEST_TOPBUTTONPAD, LITEST_ANY);
	litest_add("device:id", device_ids, LITEST_ANY, LITEST_ANY);
	litest_add_for_device("device:context", device_context, LITEST_SYNAPTICS_CLICKPAD);

	litest_add("device:udev", device_get_udev_handle, LITEST_ANY, LITEST_ANY);

	return litest_run(argc, argv);
}
