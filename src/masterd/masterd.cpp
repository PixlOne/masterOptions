#include <hidpp/SimpleDispatcher.h>
#include <hidpp/DispatcherThread.h>
#include <hidpp20/Device.h>
#include <hidpp20/Error.h>
#include <hidpp20/IReprogControlsV4.h>
#include <hidpp20/UnsupportedFeature.h>
#include <hid/DeviceMonitor.h>
#include <algorithm>
#include <cstdio>

#include "DeviceFinder.h"

int main()
{
    find_device();
    return EXIT_SUCCESS;
}