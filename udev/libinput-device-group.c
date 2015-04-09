#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libudev.h>

int main(int argc, char **argv)
{
	int rc = 1;
	struct udev *udev = NULL;
	struct udev_device *device = NULL;
	const char *syspath,
	           *phys = NULL;
	const char *product;
	char group[1024];
	char *str;

	if (argc != 2)
		return 1;

	syspath = argv[1];

	udev = udev_new();
	if (!udev)
		goto out;

	device = udev_device_new_from_syspath(udev, syspath);
	if (!device)
		goto out;

	/* Find the first parent with ATTRS{phys} set. For tablets that
	 * value looks like usb-0000:00:14.0-1/input1. Drop the /input1
	 * bit and use the remainder as device group identifier */
	while (device != NULL) {
		struct udev_device *parent;

		phys = udev_device_get_sysattr_value(device, "phys");
		if (phys)
			break;

		parent = udev_device_get_parent(device);
		udev_device_ref(parent);
		udev_device_unref(device);
		device = parent;
	}

	if (!phys)
		goto out;

	/* udev sets PRODUCT on the same device we find PHYS on, let's rely
	   on that*/
	product = udev_device_get_property_value(device, "PRODUCT");
	if (!product)
		product = "";
	snprintf(group, sizeof(group), "%s:%s", product, phys);

	str = strstr(group, "/input");
	if (str)
		*str = '\0';

	/* Cintiq 22HD Touch has
	   usb-0000:00:14.0-6.3.1/input0 for the touch
	   usb-0000:00:14.0-6.3.0/input0 for the pen
	   Check if there's a . after the last -, if so, cut off the string
	   there.
	  */
	str = strrchr(group, '.');
	if (str && str > strrchr(group, '-'))
		*str = '\0';

	printf("%s\n", group);

	rc = 0;
out:
	if (device)
		udev_device_unref(device);
	if (udev)
		udev_unref(udev);

	return rc;
}
