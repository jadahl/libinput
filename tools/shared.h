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

#ifndef _SHARED_H_
#define _SHARED_H_

#include <libinput.h>

enum tools_backend {
	BACKEND_DEVICE,
	BACKEND_UDEV
};

struct tools_options {
	enum tools_backend backend;
	const char *device; /* if backend is BACKEND_DEVICE */
	const char *seat; /* if backend is BACKEND_UDEV */

	int verbose;
	int tapping;
	int natural_scroll;
	int left_handed;
	enum libinput_config_click_method click_method;
};

void tools_init_options(struct tools_options *options);
int tools_parse_args(int argc, char **argv, struct tools_options *options);
struct libinput* tools_open_backend(struct tools_options *options,
				    void *userdata,
				    const struct libinput_interface *interface);
void tools_device_apply_config(struct libinput_device *device,
			       struct tools_options *options);
void tools_usage();

#endif
