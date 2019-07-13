#ifndef MASTEROPTIONS_DEVICEFINDER_H
#define MASTEROPTIONS_DEVICEFINDER_H

#define MX_MASTER_NAME "MX Master"
#define MX_MASTER_2S_NAME "MX Master 2S"

struct handler_pair;

class DeviceFinder : public HID::DeviceMonitor
{
protected:
    void addDevice(const char* path);
    void removeDevice(const char* path);
    std::vector<handler_pair*> handlers;
};

void find_device();

#endif //MASTEROPTIONS_DEVICEFINDER_H
