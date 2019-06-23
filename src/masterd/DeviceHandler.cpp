#include <hidpp/defs.h>
#include <hidpp/SimpleDispatcher.h>
#include <hidpp10/Error.h>
#include <future>
#include <unistd.h>
#include <hidpp/Device.h>

#include "Diverter.h"
#include "Listener.h"
#include "DeviceHandler.h"

bool SCANNING_DEVICE = false;

void DeviceHandler::start()
{
    EventListener** listener;
    while(!DeviceRemoved)
    {
        try
        {
            HIDPP::SimpleDispatcher dispatcher(path.c_str());
        }
        catch (HIDPP10::Error &e)
        {
            if (e.errorCode() == HIDPP10::Error::UnknownDevice)
                continue;
        }
        while(SCANNING_DEVICE) {}
        divert_buttons(path.c_str(), index);
        auto listener_future = std::async(std::launch::async, listen, path.c_str(), index, listener);
        HIDPP::SimpleDispatcher dispatcher(path.c_str());
        while(true)
        {
            while(SCANNING_DEVICE) {}
            try
            {
                HIDPP::Device d(&dispatcher, index);
                usleep(500000);
            }
            catch (HIDPP10::Error &e)
            {
                if (e.errorCode() == HIDPP10::Error::UnknownDevice)
                {
                    (*listener)->stop();
                    break;
                }
                printf("%s", e.what());
            }
        }
    }
    (*listener)->stop();
}