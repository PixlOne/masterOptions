#include <hidpp/defs.h>
#include <hidpp/SimpleDispatcher.h>
#include <hidpp10/Error.h>
#include <future>
#include <unistd.h>
#include <hidpp/Device.h>
#include <cassert>

#include "Diverter.h"
#include "Listener.h"
#include "DeviceHandler.h"

bool SCANNING_DEVICE = false;

void DeviceHandler::start()
{
    while(SCANNING_DEVICE) {}

    divert_buttons(path.c_str(), index);

    auto listener = new ListenerThread(path.c_str(), index);
    auto listener_future = std::async(std::launch::async, &ListenerThread::listen, listener);

    HIDPP::SimpleDispatcher dispatcher(path.c_str());
    HIDPP20::Device d(&dispatcher, index);
    while(!DeviceRemoved)
    {
        if(sizeof(DIVERT_OPTIONS))
        {
            while (SCANNING_DEVICE) {}
            HIDPP20::IReprogControlsV4 irc4(&d);
            uint8_t flags;
            irc4.getControlReporting(DIVERT_OPTIONS[0], flags);
            if (!(flags & HIDPP20::IReprogControlsV4::TemporaryDiverted))
                divert_buttons(path.c_str(), index);
            usleep(20000);
        }
    }

    listener->stop();
    listener_future.wait();
}