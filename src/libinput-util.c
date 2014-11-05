/*
 * Copyright © 2008-2011 Kristian Høgsberg
 * Copyright © 2011 Intel Corporation
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

/*
 * This list data structure is verbatim copy from wayland-util.h from the
 * Wayland project; except that wl_ prefix has been removed.
 */

#include "config.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "libinput-util.h"
#include "libinput-private.h"

void
list_init(struct list *list)
{
	list->prev = list;
	list->next = list;
}

void
list_insert(struct list *list, struct list *elm)
{
	elm->prev = list;
	elm->next = list->next;
	list->next = elm;
	elm->next->prev = elm;
}

void
list_remove(struct list *elm)
{
	elm->prev->next = elm->next;
	elm->next->prev = elm->prev;
	elm->next = NULL;
	elm->prev = NULL;
}

int
list_empty(const struct list *list)
{
	return list->next == list;
}

void
ratelimit_init(struct ratelimit *r, uint64_t ival_ms, unsigned int burst)
{
	r->interval = ival_ms;
	r->begin = 0;
	r->burst = burst;
	r->num = 0;
}

/*
 * Perform rate-limit test. Returns RATELIMIT_PASS if the rate-limited action
 * is still allowed, RATELIMIT_THRESHOLD if the limit has been reached with
 * this call, and RATELIMIT_EXCEEDED if you're beyond the threshold.
 * It's safe to treat the return-value as boolean, if you're not interested in
 * the exact state. It evaluates to "true" if the threshold hasn't been
 * exceeded, yet.
 *
 * The ratelimit object must be initialized via ratelimit_init().
 *
 * Modelled after Linux' lib/ratelimit.c by Dave Young
 * <hidave.darkstar@gmail.com>, which is licensed GPLv2.
 */
enum ratelimit_state
ratelimit_test(struct ratelimit *r)
{
	struct timespec ts;
	uint64_t mtime;

	if (r->interval <= 0 || r->burst <= 0)
		return RATELIMIT_PASS;

	clock_gettime(CLOCK_MONOTONIC, &ts);
	mtime = ts.tv_sec * 1000 + ts.tv_nsec / 1000 / 1000;

	if (r->begin <= 0 || r->begin + r->interval < mtime) {
		/* reset counter */
		r->begin = mtime;
		r->num = 1;
		return RATELIMIT_PASS;
	} else if (r->num < r->burst) {
		/* continue burst */
		return (++r->num == r->burst) ? RATELIMIT_THRESHOLD
					      : RATELIMIT_PASS;
	}

	return RATELIMIT_EXCEEDED;
}
