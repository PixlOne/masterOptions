#ifndef MASTEROPTIONS_DEVICEFINDER_H
#define MASTEROPTIONS_DEVICEFINDER_H

#define DEVICE_NAME "MX Master"

struct device
{
    std::string path;
    HIDPP::DeviceIndex index;
};

struct DeviceNotFoundException : public std::exception
{
    const char* what() const throw();
};

class DeviceFinder : public HID::DeviceMonitor
{
public:
    device dev;
protected:
    void addDevice(const char* path);
    void removeDevice(const char* path);
};

device find_target_device();

#endif //MASTEROPTIONS_DEVICEFINDER_H
