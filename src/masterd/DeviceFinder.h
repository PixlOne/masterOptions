#ifndef MASTEROPTIONS_DEVICEFINDER_H
#define MASTEROPTIONS_DEVICEFINDER_H

#include <hid/DeviceMonitor.h>
#include "DeviceHandler.h"

#define DEVICE_NAME "MX Master"

struct handler_pair
{
    DeviceHandler* handler;
    std::future<void> future;
};

class DeviceFinder : public HID::DeviceMonitor
{
protected:
    void addDevice(const char* path);
    void removeDevice(const char* path);
    std::vector<handler_pair> handlers;
public:
	~DeviceFinder(void);
};

void find_device();

#endif //MASTEROPTIONS_DEVICEFINDER_H
