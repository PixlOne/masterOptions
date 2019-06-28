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

#include "Configuration.h"
#include "EvdevDevice.h"
#include "DeviceFinder.h"

using namespace std;

int main()
{
    try
    {
        config = new Configuration("masterd.cfg");
    }
    catch (std::exception &e)
    {
        // return EXIT_FAILURE;
    }

    find_device();
    return EXIT_SUCCESS;
}