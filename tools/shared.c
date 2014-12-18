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

#define _GNU_SOURCE
#include <config.h>

#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shared.h"

enum options {
	OPT_DEVICE,
	OPT_UDEV,
	OPT_HELP,
	OPT_VERBOSE,
	OPT_TAP_ENABLE,
	OPT_TAP_DISABLE,
};

void
tools_usage()
{
	printf("Usage: %s [options] [--udev [<seat>]|--device /dev/input/event0]\n"
	       "--udev <seat>.... Use udev device discovery (default).\n"
	       "		  Specifying a seat ID is optional.\n"
	       "--device /path/to/device .... open the given device only\n"
	       "\n"
	       "Features:\n"
	       "--enable-tap\n"
	       "--disable-tap.... enable/disable tapping\n"
	       "\n"
	       "These options apply to all applicable devices, if a feature\n"
	       "is not explicitly specified it is left at each device's default.\n"
	       "\n"
	       "Other options:\n"
	       "--verbose ....... Print debugging output.\n"
	       "--help .......... Print this help.\n",
		program_invocation_short_name);
}

void
tools_init_options(struct tools_options *options)
{
	memset(options, 0, sizeof(*options));
	options->tapping = -1;
	options->backend = BACKEND_UDEV;
	options->seat = "seat0";
}

int
tools_parse_args(int argc, char **argv, struct tools_options *options)
{
	while (1) {
		int c;
		int option_index = 0;
		static struct option opts[] = {
			{ "device", 1, 0, OPT_DEVICE },
			{ "udev", 0, 0, OPT_UDEV },
			{ "help", 0, 0, OPT_HELP },
			{ "verbose", 0, 0, OPT_VERBOSE },
			{ "enable-tap", 0, 0, OPT_TAP_ENABLE },
			{ "disable-tap", 0, 0, OPT_TAP_DISABLE },
			{ 0, 0, 0, 0}
		};

		c = getopt_long(argc, argv, "h", opts, &option_index);
		if (c == -1)
			break;

		switch(c) {
			case 'h': /* --help */
			case OPT_HELP:
				tools_usage();
				exit(0);
			case OPT_DEVICE: /* --device */
				options->backend = BACKEND_DEVICE;
				if (!optarg) {
					tools_usage();
					return 1;
				}
				options->device = optarg;
				break;
			case OPT_UDEV: /* --udev */
				options->backend = BACKEND_UDEV;
				if (optarg)
					options->seat = optarg;
				break;
			case OPT_VERBOSE: /* --verbose */
				options->verbose = 1;
				break;
			case OPT_TAP_ENABLE:
				options->tapping = 1;
				break;
			case OPT_TAP_DISABLE:
				options->tapping = 0;
				break;
			default:
				tools_usage();
				return 1;
		}

	}

	if (optind < argc) {
		tools_usage();
		return 1;
	}

	return 0;
}
