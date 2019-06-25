#include <libevdev/libevdev.h>
#include <libevdev/libevdev-uinput.h>

#include "EvdevDevice.h"

EvdevDevice::EvdevDevice(const char* name)
{
    device = libevdev_new();
    libevdev_set_name(device, name);

    libevdev_enable_event_type(device, EV_KEY);
    libevdev_enable_event_code(device, EV_KEY, KEY_A, nullptr);

    int err = libevdev_uinput_create_from_device(device, LIBEVDEV_UINPUT_OPEN_MANAGED, &ui_device);

    if(err != 0)
        throw err;
}

EvdevDevice::~EvdevDevice()
{
    libevdev_free(device);
}