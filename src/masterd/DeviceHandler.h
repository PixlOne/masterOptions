#ifndef MASTEROPTIONS_DEVICEHANDLER_H
#define MASTEROPTIONS_DEVICEHANDLER_H

class DeviceHandler
{
public:
    DeviceHandler(const std::string p, HIDPP::DeviceIndex i):
            path (p), index (i)
    {
    }
    void start();
    bool DeviceRemoved = false;
    std::string path;
    const HIDPP::DeviceIndex index;
};

extern bool SCANNING_DEVICE;

#endif //MASTEROPTIONS_DEVICEHANDLER_H
