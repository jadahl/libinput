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
#include <libudev.h>

/**
 * @mainpage
 * libinput is a generic input device handling library. It abstracts
 * commonly-used concepts such as keyboard, pointer and touchpad handling
 * behind an API.
 */

/**
 * @ingroup fixed_point
 *
 * libinput 24.8 fixed point real number.
 */
typedef int32_t li_fixed_t;

/**
 * @ingroup device
 *
 * Capabilities on a device. A device may have one or more capabilities
 * at a time, and capabilities may appear or disappear during the
 * lifteime of the device.
 */
enum libinput_device_capability {
	LIBINPUT_DEVICE_CAP_KEYBOARD = 0,
	LIBINPUT_DEVICE_CAP_POINTER = 1,
	LIBINPUT_DEVICE_CAP_TOUCH = 2
};

/**
 * @ingroup device
 *
 * Logical state of a key. Note that the logical state may not represent
 * the physical state of the key.
 */
enum libinput_keyboard_key_state {
	LIBINPUT_KEYBOARD_KEY_STATE_RELEASED = 0,
	LIBINPUT_KEYBOARD_KEY_STATE_PRESSED = 1
};

/**
 * @ingroup device
 *
 * Mask reflecting LEDs on a device.
 */
enum libinput_led {
	LIBINPUT_LED_NUM_LOCK = (1 << 0),
	LIBINPUT_LED_CAPS_LOCK = (1 << 1),
	LIBINPUT_LED_SCROLL_LOCK = (1 << 2)
};

/**
 * @ingroup device
 *
 * Logical state of a physical button. Note that the logical state may not
 * represent the physical state of the button.
 */
enum libinput_pointer_button_state {
	LIBINPUT_POINTER_BUTTON_STATE_RELEASED = 0,
	LIBINPUT_POINTER_BUTTON_STATE_PRESSED = 1
};


/**
 * @ingroup device
 *
 * Axes on a device that are not x or y coordinates.
 */
enum libinput_pointer_axis {
	LIBINPUT_POINTER_AXIS_VERTICAL_SCROLL = 0,
	LIBINPUT_POINTER_AXIS_HORIZONTAL_SCROLL = 1
};

/**
 * @ingroup device
 *
 * Logical touch state of a touch point. A touch point usually follows the
 * sequence down, motion, up, with the number of motion events being zero or
 * greater. If a touch point was used for gesture interpretation internally
 * and will not generate any further events, the touchpoint is cancelled.
 */
enum libinput_touch_type {
	LIBINPUT_TOUCH_TYPE_DOWN = 0,
	LIBINPUT_TOUCH_TYPE_UP = 1,
	LIBINPUT_TOUCH_TYPE_MOTION = 2,
	LIBINPUT_TOUCH_TYPE_CANCEL = 4
};

/**
 * @ingroup base
 *
 * Event type for events returned by libinput_get_event().
 */
enum libinput_event_type {
	/**
	 * This is not a real event type, and is only used to tell the user that
	 * no new event is available in the queue. See
	 * libinput_next_event_type().
	 */
	LIBINPUT_EVENT_NONE = 0,

	/**
	 * Signals that a device has been added to the context. The device will
	 * not be read until the next time the user calls libinput_dispatch()
	 * and data is available.
	 *
	 * This allows setting up initial device configuration before any events
	 * are created.
	 */
	LIBINPUT_EVENT_DEVICE_ADDED,

	/**
	 * Signals that a device has been removed. No more events from the
	 * associated device will be in the queue or be queued after this event.
	 */
	LIBINPUT_EVENT_DEVICE_REMOVED,

	LIBINPUT_EVENT_KEYBOARD_KEY = 300,

	LIBINPUT_EVENT_POINTER_MOTION = 400,
	LIBINPUT_EVENT_POINTER_MOTION_ABSOLUTE,
	LIBINPUT_EVENT_POINTER_BUTTON,
	LIBINPUT_EVENT_POINTER_AXIS,

	LIBINPUT_EVENT_TOUCH_TOUCH = 500,
	/**
	 * Signals the end of a set of touchpoints at one device sample
	 * time. This event has no coordinate information attached.
	 */
	LIBINPUT_EVENT_TOUCH_FRAME
};

struct libinput;
struct libinput_device;
struct libinput_seat;

struct libinput_event;
struct libinput_event_device_notify;
struct libinput_event_keyboard;
struct libinput_event_pointer;

/**
 * @ingroup event_touch
 * @struct libinput_event_touch
 *
 * Touch event representing a touch down, move or up, as well as a touch
 * cancel and touch frame events. Valid event types for this event are @ref
 * LIBINPUT_EVENT_TOUCH_TOUCH and @ref LIBINPUT_EVENT_TOUCH_FRAME.
 */
struct libinput_event_touch;

/**
 * @defgroup fixed_point Fixed point utilities
 */

/**
 * @ingroup fixed_point
 *
 * Convert li_fixed_t to a double
 *
 * @param f fixed point number
 * @return Converted double
 */
static inline double
li_fixed_to_double (li_fixed_t f)
{
	union {
		double d;
		int64_t i;
	} u;

	u.i = ((1023LL + 44LL) << 52) + (1LL << 51) + f;

	return u.d - (3LL << 43);
}

/**
 * @ingroup fixed_point
 *
 * Convert li_fixed_t to a int. The fraction part is discarded.
 *
 * @param f fixed point number
 * @return Converted int
 */
static inline int
li_fixed_to_int(li_fixed_t f)
{
	return f / 256;
}

/**
 * @defgroup event Acessing and destruction of events
 */

/**
 * @ingroup event
 *
 * Destroy the event.
 *
 * @param event An event retrieved by libinput_get_event().
 */
void
libinput_event_destroy(struct libinput_event *event);

/**
 * @ingroup event
 *
 * Get the type of the event.
 *
 * @param event An event retrieved by libinput_get_event().
 */
enum libinput_event_type
libinput_event_get_type(struct libinput_event *event);

/**
 * @ingroup event
 *
 * Get the libinput context from the event.
 *
 * @param event The libinput event
 * @return The libinput context for this event.
 */
struct libinput *
libinput_event_get_context(struct libinput_event *event);

/**
 * @ingroup event
 *
 * Return the device associated with this event, if applicable. For device
 * added/removed events this is the device added or removed. For all other
 * device events, this is the device that generated the event.
 *
 * This device is not refcounted and its lifetime is that of the event. Use
 * libinput_device_ref() before using the device outside of this scope.
 *
 * @return The device associated with this event
 */

struct libinput_device *
libinput_event_get_device(struct libinput_event *event);

/**
 * @ingroup event
 *
 * Return the pointer event that is this input event. If the event type does
 * not match the pointer event types, this function returns NULL.
 *
 * @return A pointer event, or NULL for other events
 */
struct libinput_event_pointer *
libinput_event_get_pointer_event(struct libinput_event *event);

/**
 * @ingroup event
 *
 * Return the keyboard event that is this input event. If the event type does
 * not match the keyboard event types, this function returns NULL.
 *
 * @return A keyboard event, or NULL for other events
 */
struct libinput_event_keyboard *
libinput_event_get_keyboard_event(struct libinput_event *event);

/**
 * @ingroup event
 *
 * Return the touch event that is this input event. If the event type does
 * not match the touch event types, this function returns NULL.
 *
 * @return A touch event, or NULL for other events
 */
struct libinput_event_touch *
libinput_event_get_touch_event(struct libinput_event *event);

/**
 * @ingroup event
 *
 * Return the device event that is this input event. If the event type does
 * not match the device event types, this function returns NULL.
 *
 * @return A device event, or NULL for other events
 */
struct libinput_event_device_notify *
libinput_event_get_device_notify_event(struct libinput_event *event);

/**
 * @defgroup event_keyboard Keyboard events
 *
 * Key events are generated when a key changes its logical state, usually by
 * being pressed or released.
 */

/**
 * @ingroup event_keyboard
 *
 * @return The event time for this event
 */
uint32_t
libinput_event_keyboard_get_time(
	struct libinput_event_keyboard *event);

/**
 * @ingroup event_keyboard
 *
 * @return The keycode that triggered this key event
 */
uint32_t
libinput_event_keyboard_get_key(
	struct libinput_event_keyboard *event);

/**
 * @ingroup event_keyboard
 *
 * @return The state change of the key
 */
enum libinput_keyboard_key_state
libinput_event_keyboard_get_key_state(
	struct libinput_event_keyboard *event);

/**
 * @defgroup event_pointer Pointer events
 *
 * Pointer events reflect motion, button and scroll events, as well as
 * events from other axes.
 */

/**
 * @ingroup event_pointer
 *
 * @return The event time for this event
 */
uint32_t
libinput_event_pointer_get_time(
	struct libinput_event_pointer *event);

/**
 * @ingroup event_pointer
 *
 * Return the delta between the last event and the current event. The axis'
 * positive direction is device-specific. For pointer events that are
 * not of type LIBINPUT_EVENT_POINTER_MOTION, this function returns 0.
 *
 * @note It is an application bug to call this function for events other than
 * LIBINPUT_EVENT_POINTER_MOTION.
 *
 * @return the relative x movement since the last event
 */
li_fixed_t
libinput_event_pointer_get_dx(
	struct libinput_event_pointer *event);

/**
 * @ingroup event_pointer
 *
 * Return the delta between the last event and the current event. The
 * axis' positive direction is device-specific. For pointer events that are
 * not of type LIBINPUT_EVENT_POINTER_MOTION, this function returns 0.
 *
 * @note It is an application bug to call this function for events other than
 * LIBINPUT_EVENT_POINTER_MOTION.
 *
 * @return the relative y movement since the last event
 */
li_fixed_t
libinput_event_pointer_get_dy(
	struct libinput_event_pointer *event);

/**
 * @ingroup event_pointer
 *
 * Return the absolute x coordinate of the device, scaled to screen
 * coordinates.
 * The axes' positive direction is device-specific. For pointer events that
 * are not of type LIBINPUT_EVENT_POINTER_MOTION_ABSOLUTE, this function
 * returns 0.
 *
 * @note It is an application bug to call this function for events other than
 * LIBINPUT_EVENT_POINTER_MOTION_ABSOLUTE.
 *
 * @return the current absolute x coordinate scaled to screen coordinates.
 */
li_fixed_t
libinput_event_pointer_get_absolute_x(
	struct libinput_event_pointer *event);

/**
 * @ingroup event_pointer
 *
 * Return the absolute y coordinate of the device, scaled to screen coordinates.
 * The axes' positive direction is device-specific. For pointer events that
 * are not of type LIBINPUT_EVENT_POINTER_MOTION_ABSOLUTE, this function
 * returns 0.
 *
 * @note It is an application bug to call this function for events other than
 * LIBINPUT_EVENT_POINTER_MOTION_ABSOLUTE.
 *
 * @return the current absolute y coordinate scaled to screen coordinates.
 */
li_fixed_t
libinput_event_pointer_get_absolute_y(
	struct libinput_event_pointer *event);

/**
 * @ingroup event_pointer
 *
 * Return the button that triggered this event.
 * For pointer events that are not of type LIBINPUT_EVENT_POINTER_BUTTON,
 * this function returns 0.
 *
 * @note It is an application bug to call this function for events other than
 * LIBINPUT_EVENT_POINTER_BUTTON.
 *
 * @return the button triggering this event
 */
uint32_t
libinput_event_pointer_get_button(
	struct libinput_event_pointer *event);

/**
 * @ingroup event_pointer
 *
 * Return the button state that triggered this event.
 * For pointer events that are not of type LIBINPUT_EVENT_POINTER_BUTTON,
 * this function returns 0.
 *
 * @note It is an application bug to call this function for events other than
 * LIBINPUT_EVENT_POINTER_BUTTON.
 *
 * @return the button state triggering this event
 */
enum libinput_pointer_button_state
libinput_event_pointer_get_button_state(
	struct libinput_event_pointer *event);

/**
 * @ingroup event_pointer
 *
 * Return the axis that triggered this event.
 * For pointer events that are not of type LIBINPUT_EVENT_POINTER_AXIS,
 * this function returns 0.
 *
 * @note It is an application bug to call this function for events other than
 * LIBINPUT_EVENT_POINTER_AXIS.
 *
 * @return the axis triggering this event
 */
enum libinput_pointer_axis
libinput_event_pointer_get_axis(
	struct libinput_event_pointer *event);

/**
 * @ingroup event_pointer
 *
 * Return the axis value of the given axis. The interpretation of the value
 * is dependent on the axis. For the two scrolling axes
 * LIBINPUT_POINTER_AXIS_VERTICAL_SCROLL and
 * LIBINPUT_POINTER_AXIS_HORIZONTAL_SCROLL, the value of the event is in
 * relative scroll units, with the positive direction being down or right,
 * respectively. The dimension of a scroll unit is equal to one unit of
 * motion in the respective axis, where applicable (e.g. touchpad two-finger
 * scrolling).
 *
 * For pointer events that are not of type LIBINPUT_EVENT_POINTER_AXIS,
 * this function returns 0.
 *
 * @note It is an application bug to call this function for events other than
 * LIBINPUT_EVENT_POINTER_AXIS.
 *
 * @return the axis value of this event
 */
li_fixed_t
libinput_event_pointer_get_axis_value(
	struct libinput_event_pointer *event);

/**
 * @defgroup event_touch Touch events
 *
 * Events from absolute touch devices.
 */

/**
 * @ingroup event_touch
 *
 * @return The event time for this event
 */
uint32_t
libinput_event_touch_get_time(
	struct libinput_event_touch *event);

/**
 * @ingroup event_touch
 *
 * Get the currently active slot on this device. See the kernel's multitouch
 * protocol B documentation for more information.
 *
 * @note this function should not be called for LIBINPUT_EVENT_TOUCH_FRAME.
 *
 * @return The currently active slot on this multitouch device
 */
uint32_t
libinput_event_touch_get_slot(
	struct libinput_event_touch *event);

/**
 * @ingroup event_touch
 *
 * @note this function should not be called for LIBINPUT_EVENT_TOUCH_FRAME.
 *
 * @return the absolute X coordinate on this touch device, scaled to screen coordinates.
 */
li_fixed_t
libinput_event_touch_get_x(
	struct libinput_event_touch *event);

/**
 * @ingroup event_touch
 *
 * @note this function should not be called for LIBINPUT_EVENT_TOUCH_FRAME.
 *
 * @return the absolute X coordinate on this touch device, scaled to screen coordinates.
 */
li_fixed_t
libinput_event_touch_get_y(
	struct libinput_event_touch *event);

/**
 * @ingroup event_touch
 *
 * @note this function should not be called for LIBINPUT_EVENT_TOUCH_FRAME.
 *
 * @return the type of touch that occured on the device
 */
enum libinput_touch_type
libinput_event_touch_get_touch_type(
	struct libinput_event_touch *event);

/**
 * @defgroup base Initialization and manipulation of libinput contexts
 */

struct libinput_interface {
	/**
	 * Open the device at the given path with the flags provided and
	 * return the fd.
	 *
	 * @param path The device path to open
	 * @param flags Flags as defined by open(2)
	 * @param user_data The user_data provided in
	 * libinput_create_from_udev()
	 *
	 * @return the file descriptor, or a negative errno on failure.
	 */
	int (*open_restricted)(const char *path, int flags, void *user_data);
	/**
	 * Close the file descriptor.
	 *
	 * @param fd The file descriptor to close
	 * @param user_data The user_data provided in
	 * libinput_create_from_udev()
	 */
	void (*close_restricted)(int fd, void *user_data);

	void (*get_current_screen_dimensions)(struct libinput_device *device,
					      int *width,
					      int *height,
					      void *user_data);
};

/**
 * @ingroup base
 *
 * Create a new libinput context from udev, for input devices matching
 * the given seat ID. New devices or devices removed will appear as events
 * during libinput_dispatch.
 *
 * @param interface The callback interface
 * @param user_data Caller-specific data passed to the various callback
 * interfaces.
 * @param udev An already initialized udev context
 * @param seat_id A seat identifier. This string must not be NULL.
 *
 * @return An initialized libinput context, ready to handle events or NULL on
 * error.
 */
struct libinput *
libinput_create_from_udev(const struct libinput_interface *interface,
			  void *user_data,
			  struct udev *udev,
			  const char *seat_id);
/**
 * @ingroup base
 *
 * Create a new libinput context from the given path. This context
 * represents one single device only, it will not respond to new devices
 * being added and reading from the device after it was removed will fail.
 *
 * @param interface The callback interface
 * @param user_data Caller-specific data passed to the various callback
 * interfaces.
 * @param path Path to an input device
 *
 * @return An initialized libinput context, ready to handle events or NULL on
 * error.
 */
struct libinput *
libinput_create_from_path(const struct libinput_interface *interface,
			  void *user_data,
			  const char *path);

/**
 * @ingroup base
 *
 * libinput keeps a single file descriptor for all events. Call into
 * libinput_dispatch() if any events become available on this fd.
 *
 * @return the file descriptor used to notify of pending events.
 */
int
libinput_get_fd(struct libinput *libinput);

/**
 * @ingroup base
 *
 * Main event dispatchment function. Reads events of the file descriptors
 * and processes them internally. Use libinput_get_event() to retrieve the
 * events.
 *
 * Dispatching does not necessarily queue libinput events.
 *
 * @param libinput A previously initialized libinput context
 *
 * @return 0 on success, or a negative errno on failure
 */
int
libinput_dispatch(struct libinput *libinput);

/**
 * @ingroup base
 *
 * Retrieve the next event from libinput's internal event queue.
 *
 * After handling the retrieved event, the caller must destroy it using
 * libinput_event_destroy().
 *
 * @param libinput A previously initialized libinput context
 * @return The next available event, or NULL if no event is available.
 */
struct libinput_event *
libinput_get_event(struct libinput *libinput);

/**
 * @ingroup base
 *
 * Return the type of the next event in the internal queue. This function
 * does not pop the event off the queue and the next call to
 * libinput_get_event() returns that event.
 *
 * @param libinput A previously initialized libinput context
 * @return The event type of the next available event or LIBINPUT_EVENT_NONE
 * if no event is availble.
 */
enum libinput_event_type
libinput_next_event_type(struct libinput *libinput);

/**
 * @ingroup base
 *
 * @param libinput A previously initialized libinput context
 * @return the caller-specific data previously assigned in
 * libinput_create_udev().
 */
void *
libinput_get_user_data(struct libinput *libinput);

/**
 * @ingroup base
 *
 * Resume a suspended libinput context. This re-enables device
 * monitoring and adds existing devices.
 *
 * @param libinput A previously initialized libinput context
 * @see libinput_suspend
 *
 * @return 0 on success or -1 on failure
 */
int
libinput_resume(struct libinput *libinput);

/**
 * @ingroup base
 *
 * Suspend monitoring for new devices and close existing devices.
 * This all but terminates libinput but does keep the context
 * valid to be resumed with libinput_resume().
 *
 * @param libinput A previously initialized libinput context
 */
void
libinput_suspend(struct libinput *libinput);

/**
 * @ingroup base
 *
 * Destroy the libinput context. After this, object references associated with
 * the destroyed context are invalid and may not be interacted with.
 *
 * @param libinput A previously initialized libinput context
 */
void
libinput_destroy(struct libinput *libinput);

/**
 * @defgroup seat Initialization and manipulation of seats
 *
 * A seat has two identifiers, the physical name and the logical name. The
 * physical name is summarized as the list of devices a process on the same
 * physical seat has access to.
 *
 * The logical seat name is the seat name for a logical group of devices. A
 * compositor may use that to create additonal seats as independent device
 * sets. Alternatively, a compositor may limit itself to a single logical
 * seat, leaving a second compositor to manage devices on the other logical
 * seats.
 *
 * @code
 * +---+--------+------------+------------------------+------------+
 * |   | event0 |            |                        | log seat A |
 * | K +--------+            |                        +------------+
 * | e | event1 | phys seat0 |    libinput context 1  |            |
 * | r +--------+            |                        | log seat B |
 * | n | event2 |            |                        |            |
 * | e +--------+------------+------------------------+------------+
 * | l | event3 | phys seat1 |    libinput context 2  | log seat C |
 * +---+--------+------------+------------------------+------------+
 * @endcode
 */

/**
 * @ingroup seat
 *
 * Increase the refcount of the seat. A seat will be freed whenever the
 * refcount reaches 0. This may happen during dispatch if the
 * seat was removed from the system. A caller must ensure to reference
 * the seat correctly to avoid dangling pointers.
 *
 * @param seat A previously obtained seat
 */
void
libinput_seat_ref(struct libinput_seat *seat);

/**
 * @ingroup seat
 *
 * Decrease the refcount of the seat. A seat will be freed whenever the
 * refcount reaches 0. This may happen during dispatch if the
 * seat was removed from the system. A caller must ensure to reference
 * the seat correctly to avoid dangling pointers.
 *
 * @param seat A previously obtained seat
 */
void
libinput_seat_unref(struct libinput_seat *seat);

/**
 * @ingroup seat
 *
 * Set caller-specific data associated with this seat. libinput does
 * not manage, look at, or modify this data. The caller must ensure the
 * data is valid.
 *
 * @param seat A previously obtained seat
 * @param user_data Caller-specific data pointer
 * @see libinput_seat_get_user_data
 */
void
libinput_seat_set_user_data(struct libinput_seat *seat, void *user_data);

/**
 * @ingroup seat
 *
 * Get the caller-specific data associated with this seat, if any.
 *
 * @param seat A previously obtained seat
 * @return Caller-specific data pointer or NULL if none was set
 * @see libinput_seat_set_user_data
 */
void *
libinput_seat_get_user_data(struct libinput_seat *seat);

/**
 * @ingroup seat
 *
 * Return the physical name of the seat. For libinput contexts created from
 * udev, this is always the same value as passed into
 * libinput_create_from_udev() and all seats from that context will have the
 * same physical name.
 *
 * The physical name of the seat is one that is usually set by the system or
 * lower levels of the stack. In most cases, this is the base filter for
 * devices - devices assigned to seats outside the current seat will not
 * be available to the caller.
 *
 * @param seat A previously obtained seat
 * @return the physical name of this seat
 */
const char *
libinput_seat_get_physical_name(struct libinput_seat *seat);

/**
 * @ingroup seat
 *
 * Return the logical name of the seat. This is an identifier to group sets
 * of devices within the compositor.
 *
 * @param seat A previously obtained seat
 * @return the logical name of this seat
 */
const char *
libinput_seat_get_logical_name(struct libinput_seat *seat);

/**
 * @defgroup device Initialization and manipulation of input devices
 */

/**
 * @ingroup device
 *
 * Increase the refcount of the input device. An input device will be freed
 * whenever the refcount reaches 0. This may happen during dispatch if the
 * device was removed from the system. A caller must ensure to reference
 * the device correctly to avoid dangling pointers.
 *
 * @param device A previously obtained device
 */
void
libinput_device_ref(struct libinput_device *device);

/**
 * @ingroup device
 *
 * Decrease the refcount of the input device. An input device will be freed
 * whenever the refcount reaches 0. This may happen during dispatch if the
 * device was removed from the system. A caller must ensure to reference
 * the device correctly to avoid dangling pointers.
 *
 * @param device A previously obtained device
 */
void
libinput_device_unref(struct libinput_device *device);

/**
 * @ingroup device
 *
 * Set caller-specific data associated with this input device. libinput does
 * not manage, look at, or modify this data. The caller must ensure the
 * data is valid.
 *
 * @param device A previously obtained device
 * @param user_data Caller-specific data pointer
 * @see libinput_device_get_user_data
 */
void
libinput_device_set_user_data(struct libinput_device *device, void *user_data);

/**
 * @ingroup device
 *
 * Get the caller-specific data associated with this input device, if any.
 *
 * @param device A previously obtained device
 * @return Caller-specific data pointer or NULL if none was set
 * @see libinput_device_set_user_data
 */
void *
libinput_device_get_user_data(struct libinput_device *device);

/**
 * @ingroup device
 *
 * Get the system name of the device.
 *
 * @param device A previously obtained device
 * @return System name of the device
 */
const char *
libinput_device_get_sysname(struct libinput_device *device);

/**
 * @ingroup device
 *
 * A device may be mapped to a single output, or all available outputs. If a
 * device is mapped to a single output only, a relative device may not move
 * beyond the boundaries of this output. An absolute device has its input
 * coordinates mapped to the extents of this output.
 *
 * @return the name of the output this device is mapped to, or NULL if no
 * output is set
 */
const char *
libinput_device_get_output_name(struct libinput_device *device);

/**
 * @ingroup device
 *
 * Get the seat associated with this input device.
 *
 * @param device A previously obtained device
 * @return The seat this input device belongs to
 */
struct libinput_seat *
libinput_device_get_seat(struct libinput_device *device);

/**
 * @ingroup device
 *
 * Update the LEDs on the device, if any. If the device does not have
 * LEDs, or does not have one or more of the LEDs given in the mask, this
 * function does nothing.
 *
 * @param device A previously obtained device
 * @param leds A mask of the LEDs to set, or unset.
 */
void
libinput_device_led_update(struct libinput_device *device,
			   enum libinput_led leds);

/**
 * @ingroup device
 *
 * Set the bitmask in keys to the bitmask of the keys present on the device
 * (see linux/input.h), up to size characters.
 *
 * @param device A current input device
 * @param keys An array filled with the bitmask for the keys
 * @param size Size of the keys array
 */
int
libinput_device_get_keys(struct libinput_device *device,
			 char *keys, size_t size);

/**
 * @ingroup device
 *
 * Apply the 3x3 transformation matrix to absolute device coordinates. This
 * matrix has no effect on relative events.
 *
 * Given a 6-element array [a, b, c, d, e, f], the matrix is applied as
 * @code
 * [ a  b  c ]   [ x ]
 * [ d  e  f ] * [ y ]
 * [ 0  0  1 ]   [ 1 ]
 * @endcode
 */
void
libinput_device_calibrate(struct libinput_device *device,
			  float calibration[6]);

/**
 * @ingroup device
 *
 * Check if the given device has the specified capability
 *
 * @return 1 if the given device has the capability or 0 if not
 */
int
libinput_device_has_capability(struct libinput_device *device,
			       enum libinput_device_capability capability);

#endif /* LIBINPUT_H */
