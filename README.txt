/*!@mainpage

libinput
========

libinput is a library that handles input devices for display servers and other
applications that need to directly deal with input devices.

It provides device detection, device handling, input device event processing
and abstraction so minimize the amount of custom input code the user of
libinput need to provide the common set of functionality that users expect.
Input event processing includes scaling touch coordinates, generating
pointer events from touchpads, pointer acceleration, etc.

libinput originates from
[weston](http://cgit.freedesktop.org/wayland/weston/), the Wayland reference
compositor.

Architecture
------------

libinput is not used directly by applications, rather it is used by the
xf86-input-libinput X.Org driver or wayland compositors. The typical
software stack for a system running Wayland is:

@dotfile libinput-stack-wayland.gv

Where the Wayland compositor may be Weston, mutter, KWin, etc. Note that
Wayland encourages the use of toolkits, so the Wayland client (your
application) does not usually talk directly to the compositor but rather
employs a toolkit (e.g. GTK) to do so.

The simplified software stack for a system running X.Org is:

@dotfile libinput-stack-xorg.gv

Again, on a modern system the application does not usually talk directly to
the X server using Xlib but rather employs a toolkit to do so.

Source code
-----------

The source code of libinput can be found at:
http://cgit.freedesktop.org/wayland/libinput

For a list of current and past releases visit:
http://www.freedesktop.org/wiki/Software/libinput/

Reporting Bugs
--------------

Bugs can be filed in the libinput component of Wayland:
https://bugs.freedesktop.org/enter_bug.cgi?product=Wayland&component=libinput

Where possible, please provide an
[evemu](http://www.freedesktop.org/wiki/Evemu/) recording of the input
device and/or the event sequence in question.

Documentation
-------------

Developer API documentation:
http://wayland.freedesktop.org/libinput/doc/latest/modules.html

High-level documentation about libinput's features:
http://wayland.freedesktop.org/libinput/doc/latest/pages.html

License
-------

libinput is licensed under the MIT license.

> Permission to use, copy, modify, distribute, and sell this software and its
> documentation for any purpose is hereby granted without fee, provided that
> the above copyright notice appear in all copies and that both that copyright
> notice and this permission notice appear in supporting documentation, and
> that the name of the copyright holders not be used in advertising or
> publicity pertaining to distribution of the software without specific,
> written prior permission.  The copyright holders make no representations
> about the suitability of this software for any purpose.  It is provided "as
> is" without express or implied warranty.

See the [COPYING](http://cgit.freedesktop.org/wayland/libinput/tree/COPYING)
file for the full license information.

*/
