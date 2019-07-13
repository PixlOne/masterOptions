#ifndef MASTEROPTIONS_DEVICEHANDLER_H
#define MASTEROPTIONS_DEVICEHANDLER_H

class DeviceHandler
{
public:
    DeviceHandler(const std::string& name, const std::string& path, HIDPP::DeviceIndex index);
    void start();
    const std::string& getName() const;

    bool DeviceRemoved;
    std::string path;
    const HIDPP::DeviceIndex index;

private:
	std::string name;
};

extern bool SCANNING_DEVICE;

#endif //MASTEROPTIONS_DEVICEHANDLER_H
