#include <hid/DeviceMonitor.h>
#include <hidpp/SimpleDispatcher.h>
#include <hidpp/Device.h>
#include <hidpp10/Error.h>
#include <hidpp20/Error.h>
#include <cstring>
#include <unistd.h>
#include <future>

#include "DeviceFinder.h"
#include "DeviceHandler.h"

struct handler_pair
{
    DeviceHandler* handler;
    std::future<void> future;
};

void find_device()
{
    auto df = new DeviceFinder();
    df->run();
}

void DeviceFinder::addDevice(const char *path)
{
    SCANNING_DEVICE = true;
    const int max_tries = 10;
    const int try_delay = 500000;
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
            /// TODO: CONTINUOUSLY SCAN ALL DEVICES ON RECEIVER
            //Skip wireless devices if default device (receiver) has failed
            if (!has_receiver_index && index == HIDPP::WirelessDevice1)
                break;
            for(int i = 0; i < max_tries; i++)
            {
                try
                {
                    HIDPP::Device d(&dispatcher, index);
                    auto version = d.protocolVersion();
                    if (index == HIDPP::DefaultDevice && version == std::make_tuple(1, 0))
                        has_receiver_index = true;
                    if (!strcmp(d.name().c_str(), DEVICE_NAME))
                    {
                        DeviceHandler *dh = new DeviceHandler(std::string(path), index);
                        handlers.push_back(new handler_pair{
                                dh, std::async(std::launch::async, &DeviceHandler::start, dh)
                        });
                        printf("%s detected: device %d on %s\n", DEVICE_NAME, index, path);
                    }
                    i = max_tries;
                }
                catch (HIDPP10::Error &e)
                {
                    if (e.errorCode() != HIDPP10::Error::UnknownDevice && e.errorCode() != HIDPP10::Error::InvalidSubID)
                    {
                        if (i >= max_tries-1)
                            fprintf(stderr, "Error while querying %s, wireless device %d: %s\n",
                                    path, index, e.what());
                        else usleep(try_delay);
                    } else break;
                }
                catch (HIDPP20::Error &e)
                {
                    if (e.errorCode() != HIDPP20::Error::UnknownDevice && i >= max_tries)
                    {
                        if (i >= max_tries-1)
                            fprintf(stderr, "Error while querying %s, device %d: %s\n",
                                    path, index, e.what());
                        else usleep(try_delay);
                    } else break;

                }
                catch (HIDPP::Dispatcher::TimeoutError &e)
                {
                    if (i >= max_tries)
                        fprintf(stderr, "Device %s (index %d) timed out\n", path, index);
                    else usleep(try_delay);
                }
            }
        }
    }
    catch(HIDPP::Dispatcher::NoHIDPPReportException &e) {}
    catch(std::system_error &e)
    {
        /*fprintf(stderr, "Failed to open %s: %s\n", path, e.what());*/
    }
    SCANNING_DEVICE = false;
}

void DeviceFinder::removeDevice(const char* path)
{
    auto it = handlers.begin();
    while (it != handlers.end())
    {
        if(!strcmp((*it)->handler->path.c_str(), path))
        {
            printf("%s on %s removed\n", DEVICE_NAME, (*it)->handler->path.c_str());
            (*it)->handler->DeviceRemoved = true;
            (*it)->future.wait();
            handlers.erase(it);
        }
        else
        {
            it++;
        }
    }
}