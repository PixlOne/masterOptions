#ifndef MASTEROPTIONS_EVDEVDEVICE_H
#define MASTEROPTIONS_EVDEVDEVICE_H


class EvdevDevice
{
public:
    EvdevDevice(const char* name);
    ~EvdevDevice();

protected:
    struct libevdev* device;
    struct libevdev_uinput* ui_device;
};


#endif //MASTEROPTIONS_EVDEVDEVICE_H
