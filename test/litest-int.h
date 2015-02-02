/*
 * Copyright © 2013 Red Hat, Inc.
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
#include <limits.h>

#ifndef LITEST_INT_H
#define LITEST_INT_H
#include "litest.h"

/* Use as designater for litest to change the value */
#define LITEST_AUTO_ASSIGN INT_MIN

struct litest_test_device {
       enum litest_device_type type;
       enum litest_device_feature features;
       const char *shortname;
       void (*setup)(void); /* test fixture, used by check */
       void (*teardown)(void); /* test fixture, used by check */
       /**
	* If create is non-NULL it will be called to initialize the device.
	* For such devices, no overrides are possible. If create is NULL,
	* the information in name, id, events, absinfo is used to
	* create the device instead.
	*/
       void (*create)(struct litest_device *d);

       /**
	* The device name. Only used when create is NULL.
	*/
       const char *name;
       /**
	* The device id. Only used when create is NULL.
	*/
       const struct input_id *id;
       /**
	* List of event type/code tuples, terminated with -1, e.g.
	* EV_REL, REL_X, EV_KEY, BTN_LEFT, -1
	* Special tuple is INPUT_PROP_MAX, <actual property> to set.
	*
	* Any EV_ABS codes in this list will be initialized with a default
	* axis range.
	*/
       int *events;
       /**
	* List of abs codes to enable, with absinfo.value determining the
	* code to set. List must be terminated with absinfo.value -1
	*/
       struct input_absinfo *absinfo;
       struct litest_device_interface *interface;

       const char *udev_rule;
};

struct litest_device_interface {
	void (*touch_down)(struct litest_device *d, unsigned int slot, double x, double y);
	void (*touch_move)(struct litest_device *d, unsigned int slot, double x, double y);
	void (*touch_up)(struct litest_device *d, unsigned int slot);

	/**
	 * Set of of events to execute on touch down, terminated by a .type
	 * and .code value of -1. If the event value is LITEST_AUTO_ASSIGN,
	 * it will be automatically assigned by the framework (valid for x,
	 * y, tracking id and slot).
	 *
	 * These events are only used if touch_down is NULL.
	 */
	struct input_event *touch_down_events;
	struct input_event *touch_move_events;
	struct input_event *touch_up_events;

	int min[2];
	int max[2];
};

void litest_set_current_device(struct litest_device *device);
int litest_scale(const struct litest_device *d, unsigned int axis, double val);
void litest_generic_device_teardown(void);

#endif
