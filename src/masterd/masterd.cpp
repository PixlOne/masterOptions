#include <hidpp/SimpleDispatcher.h>
#include <hidpp/DispatcherThread.h>
#include <hidpp20/Device.h>
#include <hidpp20/Error.h>
#include <hidpp20/IReprogControlsV4.h>
#include <hidpp20/UnsupportedFeature.h>
#include <hid/DeviceMonitor.h>
#include <algorithm>
#include <cstdio>
#include <stdlib.h>

#include "Logger.h"
#include "Configuration.h"
#include "EvdevDevice.h"
#include "DeviceFinder.h"

#define evdev_name "masterd"

using namespace std;

LogLevel global_verbosity = DEBUG;

Configuration* config;

EvdevDevice* global_evdev;

int main(int argc, char** argv)
{
    try
    {
        global_evdev = new EvdevDevice(evdev_name);
    }
    catch(std::system_error& e)
    {
        log_printf(ERROR, "Could not create evdev device: %s", e.what());
        return EXIT_FAILURE;
    }
    try
    {
        config = new Configuration("masterd.cfg");
    }
    catch (std::exception &e)
    {
        return EXIT_FAILURE;
    }

    find_device();
    return EXIT_SUCCESS;
}
