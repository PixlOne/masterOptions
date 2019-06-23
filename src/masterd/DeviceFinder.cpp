#include <misc/Log.h>
#include <hid/DeviceMonitor.h>
#include <hidpp/SimpleDispatcher.h>
#include <hidpp/Device.h>
#include <hidpp10/Error.h>
#include <hidpp20/Error.h>
#include <cstring>

#include "DeviceFinder.h"

const char* DeviceNotFoundException::what() const throw()
{
    return "Target device not found!";
}

device find_target_device()
{
    auto dl = new DeviceFinder();
    dl->enumerate();
    if (!dl->dev.path.empty())
        return dl->dev;
    throw DeviceNotFoundException();
}

void DeviceFinder::addDevice(const char *path)
{
    try
    {
        HIDPP::SimpleDispatcher dispatcher(path);
        bool has_receiver_index = false;
        for (HIDPP::DeviceIndex index: {
                HIDPP::DefaultDevice, HIDPP::CordedDevice,
                HIDPP::WirelessDevice1, HIDPP::WirelessDevice2,
                HIDPP::WirelessDevice3, HIDPP::WirelessDevice4,
                HIDPP::WirelessDevice5, HIDPP::WirelessDevice6})
        {
            //Skip wireless devices if default device (receiver) has failed
            if (!has_receiver_index && index == HIDPP::WirelessDevice1)
                break;
            try
            {
                HIDPP::Device d(&dispatcher, index);
                auto version = d.protocolVersion();
                if (index == HIDPP::DefaultDevice && version == std::make_tuple(1, 0))
                    has_receiver_index = true;
                if (!strcmp(d.name().c_str(), DEVICE_NAME))
                {
                    dev = {std::string(path), index};
                }
            }
            catch (HIDPP10::Error &e)
            {
                /*if (e.errorCode() != HIDPP10::Error::UnknownDevice && e.errorCode() != HIDPP10::Error::InvalidSubID)
                    fprintf(stderr, "Error while querying %s, wireless device %d: %s\n",
                            path, index, e.what());*/
            }
            catch (HIDPP20::Error &e)
            {
                /*if (e.errorCode() != HIDPP20::Error::UnknownDevice)
                    fprintf(stderr, "Error while querying %s, device %d: %s\n",
                            path, index, e.what());*/
            }
            catch (HIDPP::Dispatcher::TimeoutError &e)
            {
                //fprintf(stderr, "Device %s (index %d) timed out\n", path, index);
            }
        }
    }
    catch(HIDPP::Dispatcher::NoHIDPPReportException &e) {}
    catch(std::system_error &e)
    {
        /*fprintf(stderr, "Failed to open %s: %s\n", path, e.what());*/
    }
}

void DeviceFinder::removeDevice(const char* path) {}
