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

#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

#include "libinput-util.h"

struct libinput;

struct libinput_timer {
	struct libinput *libinput;
	struct list link;
	uint64_t expire; /* in absolute ms CLOCK_MONOTONIC */
	void (*timer_func)(uint64_t now, void *timer_func_data);
	void *timer_func_data;
};

void
libinput_timer_init(struct libinput_timer *timer, struct libinput *libinput,
		    void (*timer_func)(uint64_t now, void *timer_func_data),
		    void *timer_func_data);

/* Set timer expire time, in absolute ms CLOCK_MONOTONIC */
void
libinput_timer_set(struct libinput_timer *timer, uint64_t expire);

void
libinput_timer_cancel(struct libinput_timer *timer);

int
libinput_timer_subsys_init(struct libinput *libinput);

void
libinput_timer_subsys_destroy(struct libinput *libinput);

#endif
