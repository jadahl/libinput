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

#include "litest.h"
#include "litest-int.h"

static void litest_keyboard_setup(void)
{
	struct litest_device *d = litest_create_device(LITEST_KEYBOARD);
	litest_set_current_device(d);
}

static struct input_id input_id = {
	.bustype = 0x11,
	.vendor = 0x1,
	.product = 0x1,
};

static int events[] = {
	EV_KEY, KEY_ESC,
	EV_KEY, KEY_1,
	EV_KEY, KEY_2,
	EV_KEY, KEY_3,
	EV_KEY, KEY_4,
	EV_KEY, KEY_5,
	EV_KEY, KEY_6,
	EV_KEY, KEY_7,
	EV_KEY, KEY_8,
	EV_KEY, KEY_9,
	EV_KEY, KEY_0,
	EV_KEY, KEY_MINUS,
	EV_KEY, KEY_EQUAL,
	EV_KEY, KEY_BACKSPACE,
	EV_KEY, KEY_TAB,
	EV_KEY, KEY_Q,
	EV_KEY, KEY_W,
	EV_KEY, KEY_E,
	EV_KEY, KEY_R,
	EV_KEY, KEY_T,
	EV_KEY, KEY_Y,
	EV_KEY, KEY_U,
	EV_KEY, KEY_I,
	EV_KEY, KEY_O,
	EV_KEY, KEY_P,
	EV_KEY, KEY_LEFTBRACE,
	EV_KEY, KEY_RIGHTBRACE,
	EV_KEY, KEY_ENTER,
	EV_KEY, KEY_LEFTCTRL,
	EV_KEY, KEY_A,
	EV_KEY, KEY_S,
	EV_KEY, KEY_D,
	EV_KEY, KEY_F,
	EV_KEY, KEY_G,
	EV_KEY, KEY_H,
	EV_KEY, KEY_J,
	EV_KEY, KEY_K,
	EV_KEY, KEY_L,
	EV_KEY, KEY_SEMICOLON,
	EV_KEY, KEY_APOSTROPHE,
	EV_KEY, KEY_GRAVE,
	EV_KEY, KEY_LEFTSHIFT,
	EV_KEY, KEY_BACKSLASH,
	EV_KEY, KEY_Z,
	EV_KEY, KEY_X,
	EV_KEY, KEY_C,
	EV_KEY, KEY_V,
	EV_KEY, KEY_B,
	EV_KEY, KEY_N,
	EV_KEY, KEY_M,
	EV_KEY, KEY_COMMA,
	EV_KEY, KEY_DOT,
	EV_KEY, KEY_SLASH,
	EV_KEY, KEY_RIGHTSHIFT,
	EV_KEY, KEY_KPASTERISK,
	EV_KEY, KEY_LEFTALT,
	EV_KEY, KEY_SPACE,
	EV_KEY, KEY_CAPSLOCK,
	EV_KEY, KEY_F1,
	EV_KEY, KEY_F2,
	EV_KEY, KEY_F3,
	EV_KEY, KEY_F4,
	EV_KEY, KEY_F5,
	EV_KEY, KEY_F6,
	EV_KEY, KEY_F7,
	EV_KEY, KEY_F8,
	EV_KEY, KEY_F9,
	EV_KEY, KEY_F10,
	EV_KEY, KEY_NUMLOCK,
	EV_KEY, KEY_SCROLLLOCK,
	EV_KEY, KEY_KP7,
	EV_KEY, KEY_KP8,
	EV_KEY, KEY_KP9,
	EV_KEY, KEY_KPMINUS,
	EV_KEY, KEY_KP4,
	EV_KEY, KEY_KP5,
	EV_KEY, KEY_KP6,
	EV_KEY, KEY_KPPLUS,
	EV_KEY, KEY_KP1,
	EV_KEY, KEY_KP2,
	EV_KEY, KEY_KP3,
	EV_KEY, KEY_KP0,
	EV_KEY, KEY_KPDOT,
	EV_KEY, KEY_ZENKAKUHANKAKU,
	EV_KEY, KEY_102ND,
	EV_KEY, KEY_F11,
	EV_KEY, KEY_F12,
	EV_KEY, KEY_RO,
	EV_KEY, KEY_KATAKANA,
	EV_KEY, KEY_HIRAGANA,
	EV_KEY, KEY_HENKAN,
	EV_KEY, KEY_KATAKANAHIRAGANA,
	EV_KEY, KEY_MUHENKAN,
	EV_KEY, KEY_KPJPCOMMA,
	EV_KEY, KEY_KPENTER,
	EV_KEY, KEY_RIGHTCTRL,
	EV_KEY, KEY_KPSLASH,
	EV_KEY, KEY_SYSRQ,
	EV_KEY, KEY_RIGHTALT,
	EV_KEY, KEY_LINEFEED,
	EV_KEY, KEY_HOME,
	EV_KEY, KEY_UP,
	EV_KEY, KEY_PAGEUP,
	EV_KEY, KEY_LEFT,
	EV_KEY, KEY_RIGHT,
	EV_KEY, KEY_END,
	EV_KEY, KEY_DOWN,
	EV_KEY, KEY_PAGEDOWN,
	EV_KEY, KEY_INSERT,
	EV_KEY, KEY_DELETE,
	EV_KEY, KEY_MACRO,
	EV_KEY, KEY_MUTE,
	EV_KEY, KEY_VOLUMEDOWN,
	EV_KEY, KEY_VOLUMEUP,
	EV_KEY, KEY_POWER,
	EV_KEY, KEY_KPEQUAL,
	EV_KEY, KEY_KPPLUSMINUS,
	EV_KEY, KEY_PAUSE,
	/* EV_KEY,  KEY_SCALE, */
	EV_KEY, KEY_KPCOMMA,
	EV_KEY, KEY_HANGEUL,
	EV_KEY, KEY_HANJA,
	EV_KEY, KEY_YEN,
	EV_KEY, KEY_LEFTMETA,
	EV_KEY, KEY_RIGHTMETA,
	EV_KEY, KEY_COMPOSE,
	EV_KEY, KEY_STOP,

	EV_KEY, KEY_MENU,
	EV_KEY, KEY_CALC,
	EV_KEY, KEY_SETUP,
	EV_KEY, KEY_SLEEP,
	EV_KEY, KEY_WAKEUP,
	EV_KEY, KEY_SCREENLOCK,
	EV_KEY, KEY_DIRECTION,
	EV_KEY, KEY_CYCLEWINDOWS,
	EV_KEY, KEY_MAIL,
	EV_KEY, KEY_BOOKMARKS,
	EV_KEY, KEY_COMPUTER,
	EV_KEY, KEY_BACK,
	EV_KEY, KEY_FORWARD,
	EV_KEY, KEY_NEXTSONG,
	EV_KEY, KEY_PLAYPAUSE,
	EV_KEY, KEY_PREVIOUSSONG,
	EV_KEY, KEY_STOPCD,
	EV_KEY, KEY_HOMEPAGE,
	EV_KEY, KEY_REFRESH,
	EV_KEY, KEY_F14,
	EV_KEY, KEY_F15,
	EV_KEY, KEY_SEARCH,
	EV_KEY, KEY_MEDIA,
	EV_KEY, KEY_FN,
	-1, -1,
};

struct litest_test_device litest_keyboard_device = {
	.type = LITEST_KEYBOARD,
	.features = LITEST_KEYS,
	.shortname = "default keyboard",
	.setup = litest_keyboard_setup,
	.interface = NULL,

	.name = "AT Translated Set 2 keyboard",
	.id = &input_id,
	.events = events,
	.absinfo = NULL,
};
